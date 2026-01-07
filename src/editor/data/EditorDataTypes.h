/**
 * @file EditorDataTypes.h
 * @brief Data structures for editor serialization
 * 
 * These structs represent the persistent state of the editor/game world.
 * They are serialized to/from JSON files.
 */
#pragma once

#include <string>
#include <vector>
#include <optional>
#include <map>
#include "engine/world/GridTypes.h"
#include <nlohmann/json.hpp>

namespace editor {

using json = nlohmann::json;

/**
 * @brief Actor data - shared across World/Level/Scene
 * 
 * Actors can be placed at any level in the hierarchy.
 */
struct ActorData {
    std::string id;
    std::string name;
    std::string type;  // "sprite", "npc", "trigger", etc.
    float x = 0.0f;
    float y = 0.0f;
    float width = 32.0f;
    float height = 32.0f;
    std::string sprite;  // Sprite path if applicable
    
    // Optional properties
    std::map<std::string, std::string> properties;
};

// JSON serialization for ActorData
inline void to_json(json& j, const ActorData& a) {
    j = json{
        {"id", a.id},
        {"name", a.name},
        {"type", a.type},
        {"x", a.x},
        {"y", a.y},
        {"width", a.width},
        {"height", a.height}
    };
    if (!a.sprite.empty()) j["sprite"] = a.sprite;
    if (!a.properties.empty()) j["properties"] = a.properties;
}

inline void from_json(const json& j, ActorData& a) {
    a.id = j.value("id", "");
    a.name = j.value("name", "");
    a.type = j.value("type", "sprite");
    a.x = j.value("x", 0.0f);
    a.y = j.value("y", 0.0f);
    a.width = j.value("width", 32.0f);
    a.height = j.value("height", 32.0f);
    a.sprite = j.value("sprite", "");
    if (j.contains("properties")) {
        a.properties = j["properties"].get<std::map<std::string, std::string>>();
    }
}

/**
 * @brief Level data for world.json
 */
struct LevelData {
    std::string id;
    std::string name;
    std::string description;
    engine::GridPosition gridPosition;
    std::vector<std::string> sceneIds;  // References to scenes in scenes.json
    std::vector<ActorData> actors;      // Actors placed at Level scope
    
    // Optional metadata
    std::string musicTrack;
    std::string ambientSound;
};

// JSON serialization for LevelData
inline void to_json(json& j, const LevelData& d) {
    j = json{
        {"id", d.id},
        {"name", d.name},
        {"description", d.description},
        {"sceneIds", d.sceneIds}
    };
    
    // GridPosition
    j["gridPosition"] = {
        {"gridX", d.gridPosition.gridX},
        {"gridY", d.gridPosition.gridY},
        {"pixelWidth", d.gridPosition.pixelWidth},
        {"pixelHeight", d.gridPosition.pixelHeight}
    };
    
    // Actors (only if not empty)
    if (!d.actors.empty()) j["actors"] = d.actors;
    
    if (!d.musicTrack.empty()) j["musicTrack"] = d.musicTrack;
    if (!d.ambientSound.empty()) j["ambientSound"] = d.ambientSound;
}

inline void from_json(const json& j, LevelData& d) {
    d.id = j.value("id", "");
    d.name = j.value("name", "");
    d.description = j.value("description", "");
    d.sceneIds = j.value("sceneIds", std::vector<std::string>{});
    d.musicTrack = j.value("musicTrack", "");
    d.ambientSound = j.value("ambientSound", "");
    
    if (j.contains("gridPosition")) {
        const auto& gp = j["gridPosition"];
        d.gridPosition.gridX = gp.value("gridX", 0);
        d.gridPosition.gridY = gp.value("gridY", 0);
        d.gridPosition.pixelWidth = gp.value("pixelWidth", 640);
        d.gridPosition.pixelHeight = gp.value("pixelHeight", 400);
    }
    
    // Load actors if present
    if (j.contains("actors")) {
        d.actors = j["actors"].get<std::vector<ActorData>>();
    }
}

/**
 * @brief World data for world.json
 */
struct WorldData {
    std::string name = "Game World";
    std::string version = "1.0";
    std::string startLevelId;
    std::string startSceneId;
    std::vector<LevelData> levels;
    std::vector<ActorData> actors;  // Actors placed at World scope (global)
};

// JSON serialization for WorldData
inline void to_json(json& j, const WorldData& d) {
    j = json{
        {"name", d.name},
        {"version", d.version},
        {"startLevelId", d.startLevelId},
        {"startSceneId", d.startSceneId},
        {"levels", d.levels}
    };
    
    // Actors (only if not empty)
    if (!d.actors.empty()) j["actors"] = d.actors;
}

inline void from_json(const json& j, WorldData& d) {
    d.name = j.value("name", "Game World");
    d.version = j.value("version", "1.0");
    d.startLevelId = j.value("startLevelId", "");
    d.startSceneId = j.value("startSceneId", "");
    d.levels = j.value("levels", std::vector<LevelData>{});
    
    // Load actors if present
    if (j.contains("actors")) {
        d.actors = j["actors"].get<std::vector<ActorData>>();
    }
}

/**
 * @brief Editor preferences (separate from game data)
 */
struct EditorPreferences {
    // View settings
    float defaultZoom = 1.0f;
    bool showGrid = true;
    bool showHotspots = true;
    bool showWalkArea = true;
    int gridSize = 32;
    
    // Recently opened
    std::vector<std::string> recentProjects;
    std::string lastOpenedLevel;
    std::string lastOpenedScene;
    
    // Window layout (ImGui handles most of this via imgui.ini)
    bool darkMode = true;
};

// JSON serialization for EditorPreferences
inline void to_json(json& j, const EditorPreferences& p) {
    j = json{
        {"defaultZoom", p.defaultZoom},
        {"showGrid", p.showGrid},
        {"showHotspots", p.showHotspots},
        {"showWalkArea", p.showWalkArea},
        {"gridSize", p.gridSize},
        {"recentProjects", p.recentProjects},
        {"lastOpenedLevel", p.lastOpenedLevel},
        {"lastOpenedScene", p.lastOpenedScene},
        {"darkMode", p.darkMode}
    };
}

inline void from_json(const json& j, EditorPreferences& p) {
    p.defaultZoom = j.value("defaultZoom", 1.0f);
    p.showGrid = j.value("showGrid", true);
    p.showHotspots = j.value("showHotspots", true);
    p.showWalkArea = j.value("showWalkArea", true);
    p.gridSize = j.value("gridSize", 32);
    p.recentProjects = j.value("recentProjects", std::vector<std::string>{});
    p.lastOpenedLevel = j.value("lastOpenedLevel", "");
    p.lastOpenedScene = j.value("lastOpenedScene", "");
    p.darkMode = j.value("darkMode", true);
}

} // namespace editor
