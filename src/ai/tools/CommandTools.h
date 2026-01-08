/**
 * @file CommandTools.h
 * @brief AI tool for executing editor commands
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ExecuteCommandTool
 * @brief Execute a command in the editor command panel
 */
class ExecuteCommandTool : public IEditorTool {
public:
    const char* getName() const override { return "execute_command"; }
    const char* getDescription() const override { 
        return "Execute a command in the editor command panel. Use this to run any registered tool by name."; 
    }
    const char* getCategory() const override { return "system"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"command", {
                    {"type", "string"},
                    {"description", "The command to execute (e.g., 'list_scenes' or 'create_hotspot scene_id=tavern id=door')"}
                }}
            }},
            {"required", {"command"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class ListCommandsTool
 * @brief List all available commands
 */
class ListCommandsTool : public IEditorTool {
public:
    const char* getName() const override { return "list_commands"; }
    const char* getDescription() const override { 
        return "List all available editor commands"; 
    }
    const char* getCategory() const override { return "system"; }
    
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
