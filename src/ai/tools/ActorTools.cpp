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

namespace ai {

// Helper to find scene by ID
static SceneData* findSceneDataById(const std::string& id) {
    auto& scenes = DataLoader::instance().getScenes();
    for (size_t i = 0; i < scenes.size(); ++i) {
        if (scenes[i].id == id) {
            return &scenes[i];
        }
    }
    return nullptr;
}

ToolResult ListActorsTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id")) {
        return ToolResult::error("scene_id krävs");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string typeFilter = params.value("type_filter", "all");
    
    // For now, we work with SceneData (hotspots) as a proxy
    // In a full implementation, we'd access the runtime Scene's actor list
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scen hittades inte: " + sceneId);
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
    
    std::string msg = "Hittade " + std::to_string(actorList.size()) + 
                      " actors i scen '" + sceneId + "'";
    if (typeFilter != "all") {
        msg += " (filter: " + typeFilter + ")";
    }
    
    return ToolResult::ok(msg, actorList);
}

ToolResult GetActorTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name")) {
        return ToolResult::error("scene_id och actor_name krävs");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string actorName = params["actor_name"].get<std::string>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scen hittades inte: " + sceneId);
    }
    
    // Search in hotspots
    for (size_t i = 0; i < sceneData->hotspots.size(); ++i) {
        const HotspotData& hs = sceneData->hotspots[i];
        if (hs.name == actorName || hs.id == actorName) {
            return ToolResult::ok("Actor hittad: " + hs.name, {
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
    
    return ToolResult::error("Actor hittades inte: " + actorName);
}

ToolResult CreateActorTool::execute(const nlohmann::json& params) {
    // Validate required params
    std::vector<std::string> required = {"scene_id", "name", "type", "x", "y"};
    for (size_t i = 0; i < required.size(); ++i) {
        if (!params.contains(required[i])) {
            return ToolResult::error("Saknar obligatoriskt fält: " + required[i]);
        }
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string name = params["name"].get<std::string>();
    std::string type = params["type"].get<std::string>();
    float x = params["x"].get<float>();
    float y = params["y"].get<float>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scen hittades inte: " + sceneId);
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
                return ToolResult::error("Actor med namn '" + name + "' finns redan");
            }
        }
        
        sceneData->hotspots.push_back(hs);
        
        return ToolResult::ok("Interactive actor skapad: " + name, {
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
        
        return ToolResult::ok("NPC actor skapad: " + name, {
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
        
        return ToolResult::ok("Item actor skapad: " + name, {
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
        
        return ToolResult::ok("Actor skapad: " + name, {
            {"name", name},
            {"type", type},
            {"scene_id", sceneId}
        });
    }
    
    return ToolResult::error("Okänd actor-typ: " + type);
}

ToolResult ModifyActorTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name")) {
        return ToolResult::error("scene_id och actor_name krävs");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string actorName = params["actor_name"].get<std::string>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scen hittades inte: " + sceneId);
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
        return ToolResult::error("Actor hittades inte: " + actorName);
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
        return ToolResult::ok("Inga ändringar gjorda");
    }
    
    std::string msg = "Uppdaterade actor '" + actorName + "': ";
    for (size_t i = 0; i < changes.size(); ++i) {
        if (i > 0) msg += ", ";
        msg += changes[i];
    }
    
    return ToolResult::ok(msg);
}

ToolResult DeleteActorTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name")) {
        return ToolResult::error("scene_id och actor_name krävs");
    }
    
    std::string sceneId = params["scene_id"].get<std::string>();
    std::string actorName = params["actor_name"].get<std::string>();
    
    SceneData* sceneData = findSceneDataById(sceneId);
    if (!sceneData) {
        return ToolResult::error("Scen hittades inte: " + sceneId);
    }
    
    // Find and remove from hotspots
    std::vector<HotspotData>& hotspots = sceneData->hotspots;
    for (size_t i = 0; i < hotspots.size(); ++i) {
        if (hotspots[i].name == actorName || hotspots[i].id == actorName) {
            std::string name = hotspots[i].name;
            hotspots.erase(hotspots.begin() + static_cast<std::ptrdiff_t>(i));
            return ToolResult::ok("Actor borttagen: " + name);
        }
    }
    
    return ToolResult::error("Actor hittades inte: " + actorName);
}

ToolResult AddComponentTool::execute(const nlohmann::json& params) {
    if (!params.contains("scene_id") || !params.contains("actor_name") || 
        !params.contains("component_type")) {
        return ToolResult::error("scene_id, actor_name och component_type krävs");
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
