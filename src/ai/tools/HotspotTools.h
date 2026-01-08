/**
 * @file HotspotTools.h
 * @brief AI tools for hotspot manipulation
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ListHotspotsTool
 * @brief List all hotspots in a scene
 */
class ListHotspotsTool : public IEditorTool {
public:
    const char* getName() const override { return "list_hotspots"; }
    const char* getDescription() const override { 
        return "List all hotspots in a scene"; 
    }
    const char* getCategory() const override { return "hotspots"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene to list hotspots from"}
                }}
            }},
            {"required", {"scene_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class CreateHotspotTool
 * @brief Create a new hotspot in a scene
 */
class CreateHotspotTool : public IEditorTool {
public:
    const char* getName() const override { return "create_hotspot"; }
    const char* getDescription() const override { 
        return "Create a new interactive hotspot in a scene"; 
    }
    const char* getCategory() const override { return "hotspots"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene to add hotspot to"}
                }},
                {"id", {
                    {"type", "string"},
                    {"description", "Unique hotspot ID (lowercase, underscores)"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "Display name for the hotspot"}
                }},
                {"type", {
                    {"type", "string"},
                    {"enum", {"npc", "item", "exit", "examine", "gateway"}},
                    {"description", "Type of hotspot"}
                }},
                {"x", {{"type", "integer"}, {"description", "X position"}}},
                {"y", {{"type", "integer"}, {"description", "Y position"}}},
                {"width", {{"type", "integer"}, {"description", "Width in pixels"}}},
                {"height", {{"type", "integer"}, {"description", "Height in pixels"}}},
                {"target_scene", {
                    {"type", "string"},
                    {"description", "Target scene ID for exit hotspots"}
                }},
                {"dialog_id", {
                    {"type", "string"},
                    {"description", "Dialog ID for NPC hotspots"}
                }},
                {"examine_text", {
                    {"type", "string"},
                    {"description", "Text shown when examining the hotspot"}
                }}
            }},
            {"required", {"scene_id", "id", "name", "type", "x", "y", "width", "height"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class ModifyHotspotTool
 * @brief Modify an existing hotspot
 */
class ModifyHotspotTool : public IEditorTool {
public:
    const char* getName() const override { return "modify_hotspot"; }
    const char* getDescription() const override { 
        return "Modify properties of an existing hotspot"; 
    }
    const char* getCategory() const override { return "hotspots"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene containing the hotspot"}
                }},
                {"hotspot_id", {
                    {"type", "string"},
                    {"description", "ID of the hotspot to modify"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "New display name (optional)"}
                }},
                {"x", {{"type", "integer"}, {"description", "New X position"}}},
                {"y", {{"type", "integer"}, {"description", "New Y position"}}},
                {"width", {{"type", "integer"}, {"description", "New width"}}},
                {"height", {{"type", "integer"}, {"description", "New height"}}},
                {"target_scene", {
                    {"type", "string"},
                    {"description", "New target scene (for exits)"}
                }},
                {"dialog_id", {
                    {"type", "string"},
                    {"description", "New dialog ID (for NPCs)"}
                }},
                {"examine_text", {
                    {"type", "string"},
                    {"description", "New examine text"}
                }}
            }},
            {"required", {"scene_id", "hotspot_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class DeleteHotspotTool
 * @brief Delete a hotspot from a scene
 */
class DeleteHotspotTool : public IEditorTool {
public:
    const char* getName() const override { return "delete_hotspot"; }
    const char* getDescription() const override { 
        return "Delete a hotspot from a scene"; 
    }
    const char* getCategory() const override { return "hotspots"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene containing the hotspot"}
                }},
                {"hotspot_id", {
                    {"type", "string"},
                    {"description", "ID of the hotspot to delete"}
                }}
            }},
            {"required", {"scene_id", "hotspot_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
