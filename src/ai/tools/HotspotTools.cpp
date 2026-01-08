/**
 * @file HotspotTools.cpp
 * @brief Implementation of AI tools for hotspot manipulation
 */
#include "HotspotTools.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"

namespace ai {

ToolResult ListHotspotsTool::execute(const nlohmann::json& params) {
    std::string sceneId = params.value("scene_id", "");
    
    if (sceneId.empty()) {
        return ToolResult::error("scene_id is required");
    }
    
    const SceneData* scene = DataLoader::getSceneById(sceneId);
    if (!scene) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    nlohmann::json hotspots = nlohmann::json::array();
    for (const auto& hs : scene->hotspots) {
        hotspots.push_back({
            {"id", hs.id},
            {"name", hs.name},
            {"type", hs.type},
            {"x", hs.x},
            {"y", hs.y},
            {"width", hs.w},
            {"height", hs.h},
            {"target_scene", hs.targetScene},
            {"dialog_id", hs.dialogId},
            {"examine_text", hs.examineText}
        });
    }
    
    nlohmann::json result = {
        {"scene_id", sceneId},
        {"hotspot_count", scene->hotspots.size()},
        {"hotspots", hotspots}
    };
    
    return ToolResult::ok("Found " + std::to_string(scene->hotspots.size()) + " hotspots", result);
}

ToolResult CreateHotspotTool::execute(const nlohmann::json& params) {
    std::string sceneId = params.value("scene_id", "");
    std::string id = params.value("id", "");
    std::string name = params.value("name", "");
    std::string type = params.value("type", "examine");
    
    if (sceneId.empty() || id.empty() || name.empty()) {
        return ToolResult::error("scene_id, id, and name are required");
    }
    
    SceneData* scene = DataLoader::getSceneByIdMutable(sceneId);
    if (!scene) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // Check for duplicate ID
    for (const auto& hs : scene->hotspots) {
        if (hs.id == id) {
            return ToolResult::error("Hotspot with ID '" + id + "' already exists");
        }
    }
    
    // Create new hotspot
    HotspotData hotspot;
    hotspot.id = id;
    hotspot.name = name;
    hotspot.type = type;
    hotspot.x = params.value("x", 100);
    hotspot.y = params.value("y", 100);
    hotspot.w = params.value("width", 64);
    hotspot.h = params.value("height", 64);
    hotspot.targetScene = params.value("target_scene", "");
    hotspot.dialogId = params.value("dialog_id", "");
    hotspot.examineText = params.value("examine_text", "");
    
    scene->hotspots.push_back(hotspot);
    
    // Save to file
    if (!DataLoader::saveScenes()) {
        return ToolResult::error("Failed to save scene data");
    }
    
    LOG_INFO("[AI] Created hotspot '" + name + "' in scene '" + sceneId + "'");
    
    nlohmann::json result = {
        {"scene_id", sceneId},
        {"hotspot", {
            {"id", hotspot.id},
            {"name", hotspot.name},
            {"type", hotspot.type},
            {"x", hotspot.x},
            {"y", hotspot.y},
            {"width", hotspot.w},
            {"height", hotspot.h}
        }}
    };
    
    return ToolResult::ok("Created hotspot '" + name + "'", result);
}

ToolResult ModifyHotspotTool::execute(const nlohmann::json& params) {
    std::string sceneId = params.value("scene_id", "");
    std::string hotspotId = params.value("hotspot_id", "");
    
    if (sceneId.empty() || hotspotId.empty()) {
        return ToolResult::error("scene_id and hotspot_id are required");
    }
    
    SceneData* scene = DataLoader::getSceneByIdMutable(sceneId);
    if (!scene) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // Find hotspot
    HotspotData* hotspot = nullptr;
    for (auto& hs : scene->hotspots) {
        if (hs.id == hotspotId) {
            hotspot = &hs;
            break;
        }
    }
    
    if (!hotspot) {
        return ToolResult::error("Hotspot not found: " + hotspotId);
    }
    
    // Apply modifications
    if (params.contains("name")) hotspot->name = params["name"].get<std::string>();
    if (params.contains("x")) hotspot->x = params["x"].get<int>();
    if (params.contains("y")) hotspot->y = params["y"].get<int>();
    if (params.contains("width")) hotspot->w = params["width"].get<int>();
    if (params.contains("height")) hotspot->h = params["height"].get<int>();
    if (params.contains("target_scene")) hotspot->targetScene = params["target_scene"].get<std::string>();
    if (params.contains("dialog_id")) hotspot->dialogId = params["dialog_id"].get<std::string>();
    if (params.contains("examine_text")) hotspot->examineText = params["examine_text"].get<std::string>();
    
    // Save to file
    if (!DataLoader::saveScenes()) {
        return ToolResult::error("Failed to save scene data");
    }
    
    LOG_INFO("[AI] Modified hotspot '" + hotspotId + "' in scene '" + sceneId + "'");
    
    nlohmann::json result = {
        {"scene_id", sceneId},
        {"hotspot_id", hotspotId},
        {"modified", true}
    };
    
    return ToolResult::ok("Modified hotspot '" + hotspotId + "'", result);
}

ToolResult DeleteHotspotTool::execute(const nlohmann::json& params) {
    std::string sceneId = params.value("scene_id", "");
    std::string hotspotId = params.value("hotspot_id", "");
    
    if (sceneId.empty() || hotspotId.empty()) {
        return ToolResult::error("scene_id and hotspot_id are required");
    }
    
    SceneData* scene = DataLoader::getSceneByIdMutable(sceneId);
    if (!scene) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // Find and remove hotspot
    auto it = std::find_if(scene->hotspots.begin(), scene->hotspots.end(),
        [&hotspotId](const HotspotData& hs) { return hs.id == hotspotId; });
    
    if (it == scene->hotspots.end()) {
        return ToolResult::error("Hotspot not found: " + hotspotId);
    }
    
    std::string name = it->name;
    scene->hotspots.erase(it);
    
    // Save to file
    if (!DataLoader::saveScenes()) {
        return ToolResult::error("Failed to save scene data");
    }
    
    LOG_INFO("[AI] Deleted hotspot '" + hotspotId + "' from scene '" + sceneId + "'");
    
    nlohmann::json result = {
        {"scene_id", sceneId},
        {"deleted_hotspot", hotspotId},
        {"deleted_name", name}
    };
    
    return ToolResult::ok("Deleted hotspot '" + name + "'", result);
}

} // namespace ai
