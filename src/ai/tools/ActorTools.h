/**
 * @file ActorTools.h
 * @brief AI tools for actor manipulation
 * 
 * @details
 * Actor-centric tools that work with the engine's Actor hierarchy:
 * - InteractiveActor (hotspots, triggers)
 * - CharacterActor (NPCs, players)
 * - EnvironmentActor (props, tilemaps)
 * - ItemActor (pickups)
 * - SpriteActor (visual elements)
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ListActorsTool
 * @brief List all actors in a scene with optional type filter
 */
class ListActorsTool : public IEditorTool {
public:
    const char* getName() const override { return "list_actors"; }
    const char* getDescription() const override { 
        return "List all actors in a scene, optionally filtered by type"; 
    }
    const char* getCategory() const override { return "actors"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene to list actors from"}
                }},
                {"type_filter", {
                    {"type", "string"},
                    {"enum", {"all", "interactive", "character", "npc", "environment", "item", "sprite"}},
                    {"description", "Filter by actor type (default: all)"}
                }}
            }},
            {"required", {"scene_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class GetActorTool
 * @brief Get details of a specific actor
 */
class GetActorTool : public IEditorTool {
public:
    const char* getName() const override { return "get_actor"; }
    const char* getDescription() const override { 
        return "Get detailed information about a specific actor"; 
    }
    const char* getCategory() const override { return "actors"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene containing the actor"}
                }},
                {"actor_name", {
                    {"type", "string"},
                    {"description", "Name of the actor to get"}
                }}
            }},
            {"required", {"scene_id", "actor_name"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class CreateActorTool
 * @brief Create a new actor of any type
 */
class CreateActorTool : public IEditorTool {
public:
    const char* getName() const override { return "create_actor"; }
    const char* getDescription() const override { 
        return "Create a new actor in a scene (interactive, npc, item, prop, sprite)"; 
    }
    const char* getCategory() const override { return "actors"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene to add actor to"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "Unique name for the actor"}
                }},
                {"type", {
                    {"type", "string"},
                    {"enum", {"interactive", "npc", "item", "prop", "sprite"}},
                    {"description", "Type of actor to create"}
                }},
                {"x", {{"type", "number"}, {"description", "X position"}}},
                {"y", {{"type", "number"}, {"description", "Y position"}}},
                {"width", {{"type", "number"}, {"description", "Width (for interactive areas)"}}},
                {"height", {{"type", "number"}, {"description", "Height (for interactive areas)"}}},
                {"interaction_text", {
                    {"type", "string"},
                    {"description", "Text shown when hovering (for interactive actors)"}
                }},
                {"dialog_id", {
                    {"type", "string"},
                    {"description", "Dialog ID for NPC actors"}
                }},
                {"sprite", {
                    {"type", "string"},
                    {"description", "Sprite/texture path for visual actors"}
                }},
                {"item_id", {
                    {"type", "string"},
                    {"description", "Item ID for item actors"}
                }}
            }},
            {"required", {"scene_id", "name", "type", "x", "y"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class ModifyActorTool
 * @brief Modify properties of an existing actor
 */
class ModifyActorTool : public IEditorTool {
public:
    const char* getName() const override { return "modify_actor"; }
    const char* getDescription() const override { 
        return "Modify properties of an existing actor"; 
    }
    const char* getCategory() const override { return "actors"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene containing the actor"}
                }},
                {"actor_name", {
                    {"type", "string"},
                    {"description", "Name of the actor to modify"}
                }},
                {"new_name", {
                    {"type", "string"},
                    {"description", "New name for the actor (optional)"}
                }},
                {"x", {{"type", "number"}, {"description", "New X position (optional)"}}},
                {"y", {{"type", "number"}, {"description", "New Y position (optional)"}}},
                {"visible", {{"type", "boolean"}, {"description", "Set visibility (optional)"}}},
                {"active", {{"type", "boolean"}, {"description", "Set active state (optional)"}}}
            }},
            {"required", {"scene_id", "actor_name"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class DeleteActorTool
 * @brief Delete an actor from a scene
 */
class DeleteActorTool : public IEditorTool {
public:
    const char* getName() const override { return "delete_actor"; }
    const char* getDescription() const override { 
        return "Delete an actor from a scene"; 
    }
    const char* getCategory() const override { return "actors"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene containing the actor"}
                }},
                {"actor_name", {
                    {"type", "string"},
                    {"description", "Name of the actor to delete"}
                }}
            }},
            {"required", {"scene_id", "actor_name"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class AddComponentTool
 * @brief Add a component to an actor
 */
class AddComponentTool : public IEditorTool {
public:
    const char* getName() const override { return "add_component"; }
    const char* getDescription() const override { 
        return "Add a component to an existing actor (sprite, animation, collision, etc.)"; 
    }
    const char* getCategory() const override { return "actors"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene containing the actor"}
                }},
                {"actor_name", {
                    {"type", "string"},
                    {"description", "Name of the actor to add component to"}
                }},
                {"component_type", {
                    {"type", "string"},
                    {"enum", {"sprite", "animation", "collision", "movement", "dialog", "inventory"}},
                    {"description", "Type of component to add"}
                }},
                {"properties", {
                    {"type", "object"},
                    {"description", "Component-specific properties"}
                }}
            }},
            {"required", {"scene_id", "actor_name", "component_type"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
