/**
 * @file SceneTools.h
 * @brief AI tools for scene manipulation
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ListScenesTool
 * @brief List all scenes in the game
 */
class ListScenesTool : public IEditorTool {
public:
    const char* getName() const override { return "list_scenes"; }
    const char* getDescription() const override { 
        return "List all scenes in the game with their properties"; 
    }
    const char* getCategory() const override { return "scenes"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", nlohmann::json::object()},
            {"required", nlohmann::json::array()}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class GetSceneTool
 * @brief Get details of a specific scene
 */
class GetSceneTool : public IEditorTool {
public:
    const char* getName() const override { return "get_scene"; }
    const char* getDescription() const override { 
        return "Get detailed information about a specific scene"; 
    }
    const char* getCategory() const override { return "scenes"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "The ID of the scene to get"}
                }}
            }},
            {"required", {"scene_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class CreateSceneTool
 * @brief Create a new scene
 */
class CreateSceneTool : public IEditorTool {
public:
    const char* getName() const override { return "create_scene"; }
    const char* getDescription() const override { 
        return "Create a new scene in the game"; 
    }
    const char* getCategory() const override { return "scenes"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"id", {
                    {"type", "string"},
                    {"description", "Unique scene ID (lowercase, no spaces)"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "Display name for the scene"}
                }},
                {"background", {
                    {"type", "string"},
                    {"description", "Background image path (e.g., 'backgrounds/tavern.png')"}
                }},
                {"walk_area", {
                    {"type", "object"},
                    {"description", "Walkable area bounds"},
                    {"properties", {
                        {"min_x", {{"type", "integer"}}},
                        {"max_x", {{"type", "integer"}}},
                        {"min_y", {{"type", "integer"}}},
                        {"max_y", {{"type", "integer"}}}
                    }}
                }}
            }},
            {"required", {"id", "name"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class ModifySceneTool
 * @brief Modify an existing scene
 */
class ModifySceneTool : public IEditorTool {
public:
    const char* getName() const override { return "modify_scene"; }
    const char* getDescription() const override { 
        return "Modify properties of an existing scene"; 
    }
    const char* getCategory() const override { return "scenes"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of scene to modify"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "New display name (optional)"}
                }},
                {"background", {
                    {"type", "string"},
                    {"description", "New background image (optional)"}
                }},
                {"walk_area", {
                    {"type", "object"},
                    {"description", "New walk area bounds (optional)"}
                }}
            }},
            {"required", {"scene_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
