/**
 * @file LevelTools.cpp
 * @brief Implementation of level and world manipulation tools
 */
#include "LevelTools.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"
#include "editor/managers/EditorWorldManager.h"
#include "editor/core/EditorContext.h"
#include "editor/data/EditorDataTypes.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"

namespace ai {

ToolResult ListLevelsTool::execute(const nlohmann::json& params) {
    (void)params;
    
    auto* worldManager = EditorWorldManager::getInstance();
    if (!worldManager) {
        return ToolResult::error("EditorWorldManager not available");
    }
    
    const auto& worldData = worldManager->getWorldData();
    
    if (worldData.levels.empty()) {
        return ToolResult::ok("No levels found in the world", {{"count", 0}});
    }
    
    nlohmann::json levelList = nlohmann::json::array();
    for (const auto& level : worldData.levels) {
        levelList.push_back({
            {"id", level.id},
            {"name", level.name},
            {"description", level.description},
            {"scene_count", level.sceneIds.size()},
            {"scenes", level.sceneIds}
        });
    }
    
    std::string summary = "Found " + std::to_string(worldData.levels.size()) + " levels:\n";
    for (const auto& level : worldData.levels) {
        summary += "- " + level.name + " (" + level.id + "): " + 
                   std::to_string(level.sceneIds.size()) + " scenes\n";
    }
    
    return ToolResult::ok(summary, {
        {"levels", levelList},
        {"count", worldData.levels.size()}
    });
}

ToolResult CreateLevelTool::execute(const nlohmann::json& params) {
    if (!params.contains("id") || !params.contains("name")) {
        return ToolResult::error("id and name are required");
    }
    
    std::string id = params["id"].get<std::string>();
    std::string name = params["name"].get<std::string>();
    std::string description = params.value("description", "");
    std::string musicTrack = params.value("music_track", "");
    
    auto* worldManager = EditorWorldManager::getInstance();
    if (!worldManager) {
        return ToolResult::error("EditorWorldManager not available");
    }
    
    // Check if level already exists
    auto& worldData = worldManager->getWorldData();
    for (const auto& level : worldData.levels) {
        if (level.id == id) {
            return ToolResult::error("Level with ID '" + id + "' already exists");
        }
    }
    
    // Create new level
    editor::LevelData newLevel;
    newLevel.id = id;
    newLevel.name = name;
    newLevel.description = description;
    newLevel.musicTrack = musicTrack;
    
    // Set grid position based on existing levels
    int nextGridY = 0;
    for (const auto& level : worldData.levels) {
        nextGridY = std::max(nextGridY, level.gridPosition.gridY + 1);
    }
    newLevel.gridPosition = {0, nextGridY, 640, 400};
    
    // Add to world data
    worldData.levels.push_back(newLevel);
    
    // Create engine::Level
    if (worldManager->getWorld()) {
        auto engineLevel = std::make_unique<engine::Level>(id, name);
        worldManager->getWorld()->addLevel(std::move(engineLevel));
        LOG_INFO("[AI] Created engine::Level: " + id);
    }
    
    // Save world.json
    worldManager->saveWorldData();
    
    LOG_INFO("[AI] Created level: " + name + " (ID: " + id + ")");
    
    return ToolResult::ok("Level created: " + name + " (ID: " + id + ")", {
        {"id", id},
        {"name", name},
        {"description", description}
    });
}

ToolResult AddSceneToLevelTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id")) {
        return ToolResult::error("scene_id is required");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string levelId = params.value("level_id", "");
    
    auto* worldManager = EditorWorldManager::getInstance();
    if (!worldManager) {
        return ToolResult::error("EditorWorldManager not available");
    }
    
    // Use active level if not specified
    if (levelId.empty()) {
        levelId = worldManager->getActiveLevelId();
    }
    
    if (levelId.empty()) {
        return ToolResult::error("No level specified and no active level");
    }
    
    // Add scene to level
    if (worldManager->addSceneToLevel(sceneId, levelId)) {
        worldManager->refreshViewport();
        return ToolResult::ok("Scene '" + sceneId + "' added to level '" + levelId + "'", {
            {"scene_id", sceneId},
            {"level_id", levelId}
        });
    } else {
        return ToolResult::error("Failed to add scene to level");
    }
}

ToolResult SetStartSceneTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("level_id")) {
        return ToolResult::error("scene_id and level_id are required");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string levelId = params["level_id"].get<std::string>();
    
    auto* worldManager = EditorWorldManager::getInstance();
    if (!worldManager) {
        return ToolResult::error("EditorWorldManager not available");
    }
    
    auto& worldData = worldManager->getWorldData();
    
    // Verify level exists
    bool levelFound = false;
    bool sceneFound = false;
    for (const auto& level : worldData.levels) {
        if (level.id == levelId) {
            levelFound = true;
            for (const auto& sid : level.sceneIds) {
                if (sid == sceneId) {
                    sceneFound = true;
                    break;
                }
            }
            break;
        }
    }
    
    if (!levelFound) {
        return ToolResult::error("Level '" + levelId + "' not found");
    }
    if (!sceneFound) {
        return ToolResult::error("Scene '" + sceneId + "' not found in level '" + levelId + "'");
    }
    
    // Set start scene/level
    worldData.startLevelId = levelId;
    worldData.startSceneId = sceneId;
    
    // Save
    worldManager->saveWorldData();
    
    LOG_INFO("[AI] Set start scene: " + sceneId + " in level: " + levelId);
    
    return ToolResult::ok("Start scene set to '" + sceneId + "' in level '" + levelId + "'", {
        {"start_scene_id", sceneId},
        {"start_level_id", levelId}
    });
}

ToolResult GetWorldInfoTool::execute(const nlohmann::json& params) {
    (void)params;
    
    auto* worldManager = EditorWorldManager::getInstance();
    if (!worldManager) {
        return ToolResult::error("EditorWorldManager not available");
    }
    
    const auto& worldData = worldManager->getWorldData();
    const auto& scenes = DataLoader::instance().getScenes();
    const auto& npcs = DataLoader::instance().getNPCs();
    const auto& items = DataLoader::instance().getItems();
    const auto& dialogs = DataLoader::instance().getDialogs();
    const auto& quests = DataLoader::instance().getQuests();
    
    // Build level info
    nlohmann::json levelInfo = nlohmann::json::array();
    for (const auto& level : worldData.levels) {
        nlohmann::json sceneInfo = nlohmann::json::array();
        for (const auto& sceneId : level.sceneIds) {
            // Find scene data
            for (const auto& scene : scenes) {
                if (scene.id == sceneId) {
                    sceneInfo.push_back({
                        {"id", scene.id},
                        {"name", scene.name},
                        {"hotspot_count", scene.hotspots.size()}
                    });
                    break;
                }
            }
        }
        
        levelInfo.push_back({
            {"id", level.id},
            {"name", level.name},
            {"description", level.description},
            {"scenes", sceneInfo}
        });
    }
    
    // Build summary
    std::string summary = "World: " + worldData.name + "\n";
    summary += "Start: " + worldData.startSceneId + " in " + worldData.startLevelId + "\n\n";
    summary += "Levels: " + std::to_string(worldData.levels.size()) + "\n";
    summary += "Scenes: " + std::to_string(scenes.size()) + "\n";
    summary += "NPCs: " + std::to_string(npcs.size()) + "\n";
    summary += "Items: " + std::to_string(items.size()) + "\n";
    summary += "Dialogs: " + std::to_string(dialogs.size()) + "\n";
    summary += "Quests: " + std::to_string(quests.size()) + "\n";
    
    return ToolResult::ok(summary, {
        {"world_name", worldData.name},
        {"start_level_id", worldData.startLevelId},
        {"start_scene_id", worldData.startSceneId},
        {"levels", levelInfo},
        {"stats", {
            {"level_count", worldData.levels.size()},
            {"scene_count", scenes.size()},
            {"npc_count", npcs.size()},
            {"item_count", items.size()},
            {"dialog_count", dialogs.size()},
            {"quest_count", quests.size()}
        }}
    });
}

} // namespace ai
