/**
 * @file ContextTools.cpp
 * @brief Implementation of AI tools for editor context
 */
#include "ContextTools.h"
#include "ai/core/AIAgentSystem.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"

namespace ai {

ToolResult GetEditorContextTool::execute(const nlohmann::json& params) {
    const auto& context = AIAgentSystem::instance().getContext();
    
    nlohmann::json result = {
        {"current_scene", context.currentRoom.empty() ? nullptr : nlohmann::json(context.currentRoom)},
        {"current_level", context.currentLevel.empty() ? nullptr : nlohmann::json(context.currentLevel)},
        {"selected_actor", context.selectedActor.empty() ? nullptr : nlohmann::json(context.selectedActor)}
    };
    
    // Add available scenes
    const auto& scenes = DataLoader::instance().getScenes();
    nlohmann::json sceneList = nlohmann::json::array();
    for (const auto& scene : scenes) {
        sceneList.push_back(scene.id);
    }
    result["available_scenes"] = sceneList;
    
    std::string message = "Current context: ";
    if (!context.currentRoom.empty()) {
        message += "scene=" + context.currentRoom;
    }
    if (!context.selectedActor.empty()) {
        message += ", actor=" + context.selectedActor;
    }
    if (context.currentRoom.empty() && context.selectedActor.empty()) {
        message += "nothing selected";
    }
    
    return ToolResult::ok(message, result);
}

ToolResult SelectSceneTool::execute(const nlohmann::json& params) {
    std::string sceneId = params.value("scene_id", "");
    
    if (sceneId.empty()) {
        return ToolResult::error("scene_id is required");
    }
    
    // Verify scene exists
    const SceneData* scene = DataLoader::getSceneById(sceneId);
    if (!scene) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // Update context
    auto& context = AIAgentSystem::instance().getContextMutable();
    context.currentRoom = sceneId;
    context.selectedActor.clear();  // Clear actor selection when changing scene
    
    LOG_INFO("[AI] Selected scene: " + sceneId);
    
    nlohmann::json result = {
        {"scene_id", sceneId},
        {"scene_name", scene->name}
    };
    
    return ToolResult::ok("Selected scene '" + scene->name + "'", result);
}

ToolResult SelectActorTool::execute(const nlohmann::json& params) {
    std::string actorName = params.value("actor_name", "");
    
    if (actorName.empty()) {
        return ToolResult::error("actor_name is required");
    }
    
    // Update context
    auto& context = AIAgentSystem::instance().getContextMutable();
    context.selectedActor = actorName;
    
    LOG_INFO("[AI] Selected actor: " + actorName);
    
    nlohmann::json result = {
        {"actor_name", actorName},
        {"scene_id", context.currentRoom}
    };
    
    return ToolResult::ok("Selected actor '" + actorName + "'", result);
}

} // namespace ai
