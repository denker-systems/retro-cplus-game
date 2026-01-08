/**
 * @file LevelTools.h
 * @brief AI tools for level and world manipulation
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ListLevelsTool
 * @brief List all levels in the world
 */
class ListLevelsTool : public IEditorTool {
public:
    const char* getName() const override { return "list_levels"; }
    const char* getDescription() const override { 
        return "List all levels in the game world"; 
    }
    const char* getCategory() const override { return "world"; }
    
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
 * @class CreateLevelTool
 * @brief Create a new level in the world
 */
class CreateLevelTool : public IEditorTool {
public:
    const char* getName() const override { return "create_level"; }
    const char* getDescription() const override { 
        return "Create a new level (chapter/area) in the game world. Levels contain multiple scenes."; 
    }
    const char* getCategory() const override { return "world"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"id", {
                    {"type", "string"},
                    {"description", "Unique level ID (e.g., 'chapter_1', 'forest_area')"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "Display name for the level"}
                }},
                {"description", {
                    {"type", "string"},
                    {"description", "Brief description of the level/area"}
                }},
                {"music_track", {
                    {"type", "string"},
                    {"description", "Background music for this level (optional)"}
                }}
            }},
            {"required", {"id", "name"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class AddSceneToLevelTool
 * @brief Add an existing scene to a level
 */
class AddSceneToLevelTool : public IEditorTool {
public:
    const char* getName() const override { return "add_scene_to_level"; }
    const char* getDescription() const override { 
        return "Add an existing scene to a level"; 
    }
    const char* getCategory() const override { return "world"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene to add"}
                }},
                {"level_id", {
                    {"type", "string"},
                    {"description", "ID of the level to add the scene to (optional, uses active level if not specified)"}
                }}
            }},
            {"required", {"scene_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class SetStartSceneTool
 * @brief Set the starting scene for the game
 */
class SetStartSceneTool : public IEditorTool {
public:
    const char* getName() const override { return "set_start_scene"; }
    const char* getDescription() const override { 
        return "Set the starting scene and level for the game"; 
    }
    const char* getCategory() const override { return "world"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the starting scene"}
                }},
                {"level_id", {
                    {"type", "string"},
                    {"description", "ID of the starting level"}
                }}
            }},
            {"required", {"scene_id", "level_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class GetWorldInfoTool
 * @brief Get information about the current world structure
 */
class GetWorldInfoTool : public IEditorTool {
public:
    const char* getName() const override { return "get_world_info"; }
    const char* getDescription() const override { 
        return "Get complete information about the game world structure including all levels and scenes"; 
    }
    const char* getCategory() const override { return "world"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", nlohmann::json::object()},
            {"required", nlohmann::json::array()}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
