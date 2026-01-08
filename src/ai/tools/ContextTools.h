/**
 * @file ContextTools.h
 * @brief AI tools for getting editor context
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class GetEditorContextTool
 * @brief Get current editor context (selected scene, actor, etc.)
 */
class GetEditorContextTool : public IEditorTool {
public:
    const char* getName() const override { return "get_editor_context"; }
    const char* getDescription() const override { 
        return "Get current editor context: selected scene, actor, and other state"; 
    }
    const char* getCategory() const override { return "context"; }
    
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
 * @class SelectSceneTool
 * @brief Select a scene in the editor
 */
class SelectSceneTool : public IEditorTool {
public:
    const char* getName() const override { return "select_scene"; }
    const char* getDescription() const override { 
        return "Select a scene in the editor viewport"; 
    }
    const char* getCategory() const override { return "context"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"scene_id", {
                    {"type", "string"},
                    {"description", "ID of the scene to select"}
                }}
            }},
            {"required", {"scene_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class SelectActorTool
 * @brief Select an actor in the editor
 */
class SelectActorTool : public IEditorTool {
public:
    const char* getName() const override { return "select_actor"; }
    const char* getDescription() const override { 
        return "Select an actor in the current scene"; 
    }
    const char* getCategory() const override { return "context"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"actor_name", {
                    {"type", "string"},
                    {"description", "Name of the actor to select"}
                }}
            }},
            {"required", {"actor_name"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
