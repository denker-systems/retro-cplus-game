/**
 * @file SceneTools.cpp
 * @brief Implementation of scene manipulation tools
 */
#include "SceneTools.h"
#include "engine/data/DataLoader.h"
#include "engine/data/GameData.h"

namespace ai {

// Helper to find scene by ID
static SceneData* findSceneById(const std::string& id) {
    auto& scenes = DataLoader::instance().getScenes();
    for (size_t i = 0; i < scenes.size(); ++i) {
        if (scenes[i].id == id) {
            return &scenes[i];
        }
    }
    return nullptr;
}

ToolResult ListScenesTool::execute(const nlohmann::json& params) {
    (void)params;
    
    const auto& scenes = DataLoader::instance().getScenes();
    
    if (scenes.empty()) {
        return ToolResult::ok("Inga scener hittades.", nlohmann::json::array());
    }
    
    nlohmann::json sceneList = nlohmann::json::array();
    for (size_t i = 0; i < scenes.size(); ++i) {
        const SceneData& scene = scenes[i];
        sceneList.push_back({
            {"id", scene.id},
            {"name", scene.name},
            {"background", scene.background},
            {"hotspot_count", scene.hotspots.size()}
        });
    }
    
    std::string msg = "Hittade " + std::to_string(scenes.size()) + " scener.";
    return ToolResult::ok(msg, sceneList);
}

ToolResult GetSceneTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id")) {
        return ToolResult::error("scene_id krävs");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    const auto& scenes = DataLoader::instance().getScenes();
    
    for (size_t i = 0; i < scenes.size(); ++i) {
        const SceneData& scene = scenes[i];
        if (scene.id == sceneId) {
            nlohmann::json sceneData = {
                {"id", scene.id},
                {"name", scene.name},
                {"background", scene.background},
                {"walk_area", {
                    {"min_x", scene.walkArea.minX},
                    {"max_x", scene.walkArea.maxX},
                    {"min_y", scene.walkArea.minY},
                    {"max_y", scene.walkArea.maxY}
                }},
                {"player_spawn", {
                    {"x", scene.playerSpawnX},
                    {"y", scene.playerSpawnY}
                }},
                {"hotspots", nlohmann::json::array()}
            };
            
            for (size_t j = 0; j < scene.hotspots.size(); ++j) {
                const HotspotData& hs = scene.hotspots[j];
                sceneData["hotspots"].push_back({
                    {"id", hs.id},
                    {"name", hs.name},
                    {"type", hs.type},
                    {"x", hs.x},
                    {"y", hs.y},
                    {"w", hs.w},
                    {"h", hs.h}
                });
            }
            
            return ToolResult::ok("Scen hittad: " + scene.name, sceneData);
        }
    }
    
    return ToolResult::error("Scen hittades inte: " + sceneId);
}

ToolResult CreateSceneTool::execute(const nlohmann::json& params) {
    if (!params.contains("id") || !params.contains("name")) {
        return ToolResult::error("id och name krävs");
    }
    
    std::string id = params["id"].get<std::string>();
    std::string name = params["name"].get<std::string>();
    
    // Check if scene already exists
    const auto& scenes = DataLoader::instance().getScenes();
    for (size_t i = 0; i < scenes.size(); ++i) {
        if (scenes[i].id == id) {
            return ToolResult::error("Scen med id '" + id + "' finns redan");
        }
    }
    
    // Create new scene data
    SceneData newScene;
    newScene.id = id;
    newScene.name = name;
    newScene.background = params.value("background", "backgrounds/default.png");
    
    // Walk area defaults
    newScene.walkArea.minX = 0;
    newScene.walkArea.maxX = 640;
    newScene.walkArea.minY = 260;
    newScene.walkArea.maxY = 400;
    
    if (params.contains("walk_area")) {
        const auto& wa = params["walk_area"];
        newScene.walkArea.minX = wa.value("min_x", 0);
        newScene.walkArea.maxX = wa.value("max_x", 640);
        newScene.walkArea.minY = wa.value("min_y", 260);
        newScene.walkArea.maxY = wa.value("max_y", 400);
    }
    
    // Player spawn defaults
    newScene.playerSpawnX = 320.0f;
    newScene.playerSpawnY = 300.0f;
    
    // Add to DataLoader
    DataLoader::instance().getScenes().push_back(newScene);
    
    // TODO: Create IEditorCommand for undo support
    
    return ToolResult::ok("Scen skapad: " + name, {
        {"id", id},
        {"name", name}
    });
}

ToolResult ModifySceneTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id")) {
        return ToolResult::error("scene_id krävs");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    
    // Find scene
    SceneData* scene = findSceneById(sceneId);
    if (!scene) {
        return ToolResult::error("Scen hittades inte: " + sceneId);
    }
    
    // Apply changes
    std::vector<std::string> changes;
    
    if (params.contains("name")) {
        scene->name = params["name"].get<std::string>();
        changes.push_back("name");
    }
    
    if (params.contains("background")) {
        scene->background = params["background"].get<std::string>();
        changes.push_back("background");
    }
    
    if (params.contains("walk_area")) {
        const auto& wa = params["walk_area"];
        if (wa.contains("min_x")) scene->walkArea.minX = wa["min_x"].get<int>();
        if (wa.contains("max_x")) scene->walkArea.maxX = wa["max_x"].get<int>();
        if (wa.contains("min_y")) scene->walkArea.minY = wa["min_y"].get<int>();
        if (wa.contains("max_y")) scene->walkArea.maxY = wa["max_y"].get<int>();
        changes.push_back("walk_area");
    }
    
    if (changes.empty()) {
        return ToolResult::ok("Inga ändringar gjorda");
    }
    
    // TODO: Create IEditorCommand for undo support
    
    std::string msg = "Uppdaterade scen '" + sceneId + "': ";
    for (size_t i = 0; i < changes.size(); ++i) {
        if (i > 0) msg += ", ";
        msg += changes[i];
    }
    
    return ToolResult::ok(msg);
}

} // namespace ai
