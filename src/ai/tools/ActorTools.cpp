/**
 * @file ActorTools.cpp
 * @brief Implementation of actor manipulation tools
 * 
 * @note Currently works at data-level (HotspotData in SceneData).
 *       Future versions will work directly with runtime Actor objects.
 */
#include "ActorTools.h"
#include "engine/data/DataLoader.h"
#include "engine/data/GameData.h"
#include "engine/utils/Logger.h"
#include "editor/managers/EditorWorldManager.h"
#include "editor/core/EditorContext.h"
#include "editor/core/EditorState.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/actors/InteractiveActor.h"

namespace ai {

// Helper to find scene by ID in DataLoader
static SceneData* findSceneDataById(const std::string& id) {
    auto& scenes = DataLoader::instance().getScenes();
    for (size_t i = 0; i < scenes.size(); ++i) {
        if (scenes[i].id == id) {
            return &scenes[i];
        }
    }
    return nullptr;
}

// Helper to find scene in EditorContext.rooms (for viewport display)
static SceneData* findEditorContextRoom(const std::string& id) {
    auto* worldManager = EditorWorldManager::getInstance();
    if (!worldManager) return nullptr;
    
    auto& context = worldManager->getContext();
    for (size_t i = 0; i < context.rooms.size(); ++i) {
        if (context.rooms[i].id == id) {
            return &context.rooms[i];
        }
    }
    return nullptr;
}

// Helper to find engine::Scene by ID
static engine::Scene* findEngineSceneById(const std::string& id) {
    auto* worldManager = EditorWorldManager::getInstance();
    if (!worldManager) {
        LOG_WARNING("[AI] EditorWorldManager::getInstance() returned nullptr");
        return nullptr;
    }
    if (!worldManager->getWorld()) {
        LOG_WARNING("[AI] EditorWorldManager::getWorld() returned nullptr");
        return nullptr;
    }
    
    LOG_DEBUG("[AI] Looking for engine::Scene with id: " + id);
    for (auto& level : worldManager->getWorld()->getLevels()) {
        LOG_DEBUG("[AI] Checking level: " + level->getName() + " with " + std::to_string(level->getScenes().size()) + " scenes");
        for (auto& scene : level->getScenes()) {
            LOG_DEBUG("[AI] Checking scene: " + scene->getName());
            if (scene->getId() == id) {
                LOG_INFO("[AI] Found engine::Scene: " + id);
                return scene.get();
            }
        }
    }
    LOG_WARNING("[AI] engine::Scene not found: " + id);
    return nullptr;
}

ToolResult ListActorsTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id")) {
        return ToolResult::error("scene_id is required");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string typeFilter = params.value("type_filter", "all");
    
    // For now, we work with SceneData (hotspots) as a proxy
    // In a full implementation, we'd access the runtime Scene's actor list
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    nlohmann::json actorList = nlohmann::json::array();
    
    // List hotspots as interactive actors
    if (typeFilter == "all" || typeFilter == "interactive") {
        for (size_t i = 0; i < sceneData->hotspots.size(); ++i) {
            const HotspotData& hs = sceneData->hotspots[i];
            actorList.push_back({
                {"name", hs.name},
                {"type", "interactive"},
                {"subtype", hs.type},
                {"x", hs.x},
                {"y", hs.y},
                {"width", hs.w},
                {"height", hs.h}
            });
        }
    }
    
    // TODO: Add other actor types when runtime Scene access is available
    
    std::string msg = "Found " + std::to_string(actorList.size()) + 
                      " actors in scene '" + sceneId + "'";
    if (typeFilter != "all") {
        msg += " (filter: " + typeFilter + ")";
    }
    
    return ToolResult::ok(msg, actorList);
}

ToolResult GetActorTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name")) {
        return ToolResult::error("scene_id and actor_name are required");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string actorName = params["actor_name"].get<std::string>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // Search in hotspots
    for (size_t i = 0; i < sceneData->hotspots.size(); ++i) {
        const HotspotData& hs = sceneData->hotspots[i];
        if (hs.name == actorName || hs.id == actorName) {
            return ToolResult::ok("Actor found: " + hs.name, {
                {"name", hs.name},
                {"id", hs.id},
                {"type", "interactive"},
                {"subtype", hs.type},
                {"x", hs.x},
                {"y", hs.y},
                {"width", hs.w},
                {"height", hs.h},
                {"interaction_text", hs.examineText},
                {"dialog_id", hs.dialogId},
                {"target_scene", hs.targetScene}
            });
        }
    }
    
    return ToolResult::error("Actor not found: " + actorName);
}

ToolResult CreateActorTool::execute(const nlohmann::json& params) {
    // Validate required params
    std::vector<std::string> required = {"scene_id", "name", "type", "x", "y"};
    for (size_t i = 0; i < required.size(); ++i) {
        if (!params.contains(required[i])) {
            return ToolResult::error("Missing required field: " + required[i]);
        }
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string name = params["name"].get<std::string>();
    std::string type = params["type"].get<std::string>();
    float x = params["x"].get<float>();
    float y = params["y"].get<float>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // For interactive actors, create as HotspotData (data-level)
    if (type == "interactive") {
        HotspotData hs;
        hs.id = name;  // Use name as ID for simplicity
        hs.name = name;
        hs.type = "examine";  // Default subtype
        hs.x = static_cast<int>(x);
        hs.y = static_cast<int>(y);
        hs.w = params.value("width", 50);
        hs.h = params.value("height", 50);
        
        if (params.contains("interaction_text")) {
            hs.examineText = params["interaction_text"].get<std::string>();
        }
        
        // Check for duplicate
        for (size_t i = 0; i < sceneData->hotspots.size(); ++i) {
            if (sceneData->hotspots[i].id == hs.id) {
                return ToolResult::error("Actor with name '" + name + "' already exists");
            }
        }
        
        sceneData->hotspots.push_back(hs);
        
        // Also add to EditorContext.rooms for viewport display (hybrid rendering)
        SceneData* editorRoom = findEditorContextRoom(sceneId);
        if (editorRoom) {
            editorRoom->hotspots.push_back(hs);
            LOG_INFO("[AI] Added interactive hotspot '" + name + "' to EditorContext.rooms");
        } else {
            LOG_WARNING("[AI] EditorContext room not found for: " + sceneId);
        }
        
        // Also add to engine::Scene
        engine::Scene* engineScene = findEngineSceneById(sceneId);
        if (engineScene) {
            auto actor = std::make_unique<engine::InteractiveActor>(name);
            actor->setPosition(x, y);
            actor->setInteractionText(params.value("interaction_text", name));
            engineScene->addActor(std::move(actor));
            LOG_DEBUG("[AI] Added interactive actor to engine::Scene");
        }
        
        // Save to file
        DataLoader::saveScenes();
        
        return ToolResult::ok("Interactive actor created: " + name, {
            {"name", name},
            {"type", type},
            {"scene_id", sceneId}
        });
    }
    
    // For NPC actors
    if (type == "npc") {
        HotspotData hs;
        hs.id = name;
        hs.name = name;
        hs.type = "npc";
        hs.x = static_cast<int>(x);
        hs.y = static_cast<int>(y);
        hs.w = params.value("width", 32);
        hs.h = params.value("height", 64);
        
        if (params.contains("dialog_id")) {
            hs.dialogId = params["dialog_id"].get<std::string>();
        }
        
        sceneData->hotspots.push_back(hs);
        
        // Also add to EditorContext.rooms for viewport display (hybrid rendering)
        SceneData* editorRoom = findEditorContextRoom(sceneId);
        if (editorRoom) {
            editorRoom->hotspots.push_back(hs);
            LOG_INFO("[AI] Added NPC hotspot '" + name + "' to EditorContext.rooms");
        } else {
            LOG_WARNING("[AI] EditorContext room not found for: " + sceneId);
        }
        
        // Add to DataLoader NPCs list for Hierarchy panel
        NPCData npcData;
        npcData.id = name;
        npcData.name = name;
        npcData.room = sceneId;
        npcData.x = static_cast<int>(x);
        npcData.y = static_cast<int>(y);
        npcData.sprite = params.value("sprite", "");
        if (params.contains("dialog_id")) {
            npcData.dialogId = params["dialog_id"].get<std::string>();
        }
        DataLoader::instance().getNPCs().push_back(npcData);
        LOG_INFO("[AI] Added NPC '" + name + "' to DataLoader.npcs for Hierarchy");
        
        // Also add to engine::Scene
        engine::Scene* engineScene = findEngineSceneById(sceneId);
        if (engineScene) {
            auto actor = std::make_unique<engine::InteractiveActor>(name);
            actor->setPosition(x, y);
            actor->setInteractionText(name);
            engineScene->addActor(std::move(actor));
            LOG_DEBUG("[AI] Added NPC actor to engine::Scene");
        }
        
        // Save to file
        DataLoader::saveScenes();
        DataLoader::saveNPCs();
        
        return ToolResult::ok("NPC actor created: " + name, {
            {"name", name},
            {"type", type},
            {"scene_id", sceneId}
        });
    }
    
    // For item actors
    if (type == "item") {
        HotspotData hs;
        hs.id = name;
        hs.name = name;
        hs.type = "item";
        hs.x = static_cast<int>(x);
        hs.y = static_cast<int>(y);
        hs.w = params.value("width", 32);
        hs.h = params.value("height", 32);
        
        sceneData->hotspots.push_back(hs);
        
        // Also add to EditorContext.rooms for viewport display (hybrid rendering)
        SceneData* editorRoom = findEditorContextRoom(sceneId);
        if (editorRoom) {
            editorRoom->hotspots.push_back(hs);
            LOG_INFO("[AI] Added item hotspot '" + name + "' to EditorContext.rooms");
        } else {
            LOG_WARNING("[AI] EditorContext room not found for: " + sceneId);
        }
        
        // Add to DataLoader Items list for Hierarchy panel
        ItemData itemData;
        itemData.id = name;
        itemData.name = name;
        itemData.icon = params.value("sprite", "");
        itemData.description = params.value("description", "");
        DataLoader::instance().getItems().push_back(itemData);
        LOG_INFO("[AI] Added item '" + name + "' to DataLoader.items for Hierarchy");
        
        // Also add to engine::Scene
        engine::Scene* engineScene = findEngineSceneById(sceneId);
        if (engineScene) {
            auto actor = std::make_unique<engine::InteractiveActor>(name);
            actor->setPosition(x, y);
            actor->setInteractionText(name);
            engineScene->addActor(std::move(actor));
            LOG_DEBUG("[AI] Added item actor to engine::Scene");
        }
        
        // Save to file
        DataLoader::saveScenes();
        DataLoader::saveItems();
        
        return ToolResult::ok("Item actor created: " + name, {
            {"name", name},
            {"type", type},
            {"scene_id", sceneId}
        });
    }
    
    // For prop/sprite actors - store as hotspot with special type for now
    if (type == "prop" || type == "sprite") {
        HotspotData hs;
        hs.id = name;
        hs.name = name;
        hs.type = type;
        hs.x = static_cast<int>(x);
        hs.y = static_cast<int>(y);
        hs.w = params.value("width", 64);
        hs.h = params.value("height", 64);
        
        sceneData->hotspots.push_back(hs);
        
        return ToolResult::ok("Actor created: " + name, {
            {"name", name},
            {"type", type},
            {"scene_id", sceneId}
        });
    }
    
    return ToolResult::error("Unknown actor type: " + type);
}

ToolResult ModifyActorTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name")) {
        return ToolResult::error("scene_id and actor_name are required");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string actorName = params["actor_name"].get<std::string>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // Find actor in hotspots
    HotspotData* actor = nullptr;
    for (size_t i = 0; i < sceneData->hotspots.size(); ++i) {
        if (sceneData->hotspots[i].name == actorName || 
            sceneData->hotspots[i].id == actorName) {
            actor = &sceneData->hotspots[i];
            break;
        }
    }
    
    if (!actor) {
        return ToolResult::error("Actor not found: " + actorName);
    }
    
    std::vector<std::string> changes;
    
    if (params.contains("new_name")) {
        actor->name = params["new_name"].get<std::string>();
        actor->id = actor->name;
        changes.push_back("name");
    }
    
    if (params.contains("x")) {
        actor->x = static_cast<int>(params["x"].get<float>());
        changes.push_back("x");
    }
    
    if (params.contains("y")) {
        actor->y = static_cast<int>(params["y"].get<float>());
        changes.push_back("y");
    }
    
    if (changes.empty()) {
        return ToolResult::ok("No changes made");
    }
    
    std::string msg = "Updated actor '" + actorName + "': ";
    for (size_t i = 0; i < changes.size(); ++i) {
        if (i > 0) msg += ", ";
        msg += changes[i];
    }
    
    return ToolResult::ok(msg);
}

ToolResult DeleteActorTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name")) {
        return ToolResult::error("scene_id and actor_name are required");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string actorName = params["actor_name"].get<std::string>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scene not found: " + sceneId);
    }
    
    // Find and remove from hotspots
    std::vector<HotspotData>& hotspots = sceneData->hotspots;
    for (size_t i = 0; i < hotspots.size(); ++i) {
        if (hotspots[i].name == actorName || hotspots[i].id == actorName) {
            std::string name = hotspots[i].name;
            hotspots.erase(hotspots.begin() + static_cast<std::ptrdiff_t>(i));
            return ToolResult::ok("Actor deleted: " + name);
        }
    }
    
    return ToolResult::error("Actor not found: " + actorName);
}

ToolResult AddComponentTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name") || 
        !params.contains("component_type")) {
        return ToolResult::error("scene_id, actor_name and component_type are required");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string actorName = params["actor_name"].get<std::string>();
    std::string componentType = params["component_type"].get<std::string>();
    
    // TODO: Implement component addition when runtime Scene access is available
    // For now, return a placeholder response
    
    return ToolResult::ok(
        "Component '" + componentType + "' markerad för actor '" + actorName + "'. "
        "Komponenten läggs till vid nästa scene-laddning.",
        {
            {"actor_name", actorName},
            {"component_type", componentType},
            {"status", "pending"}
        }
    );
}

} // namespace ai
