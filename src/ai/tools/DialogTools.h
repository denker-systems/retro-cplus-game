/**
 * @file DialogTools.h
 * @brief AI tools for dialog manipulation
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ListDialogsTool
 * @brief List all dialogs in the game
 */
class ListDialogsTool : public IEditorTool {
public:
    const char* getName() const override { return "list_dialogs"; }
    const char* getDescription() const override { 
        return "List all dialogs in the game"; 
    }
    const char* getCategory() const override { return "dialogs"; }
    
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
 * @class GetDialogTool
 * @brief Get details of a specific dialog
 */
class GetDialogTool : public IEditorTool {
public:
    const char* getName() const override { return "get_dialog"; }
    const char* getDescription() const override { 
        return "Get detailed information about a specific dialog including all nodes"; 
    }
    const char* getCategory() const override { return "dialogs"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"dialog_id", {
                    {"type", "string"},
                    {"description", "ID of the dialog to get"}
                }}
            }},
            {"required", {"dialog_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class CreateDialogTool
 * @brief Create a new dialog
 */
class CreateDialogTool : public IEditorTool {
public:
    const char* getName() const override { return "create_dialog"; }
    const char* getDescription() const override { 
        return "Create a new dialog for an NPC"; 
    }
    const char* getCategory() const override { return "dialogs"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"id", {
                    {"type", "string"},
                    {"description", "Unique dialog ID (e.g., 'bartender_intro')"}
                }},
                {"npc_name", {
                    {"type", "string"},
                    {"description", "Name of the NPC speaking"}
                }},
                {"initial_text", {
                    {"type", "string"},
                    {"description", "The first line of dialog"}
                }}
            }},
            {"required", {"id", "npc_name", "initial_text"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class AddDialogNodeTool
 * @brief Add a node to an existing dialog
 */
class AddDialogNodeTool : public IEditorTool {
public:
    const char* getName() const override { return "add_dialog_node"; }
    const char* getDescription() const override { 
        return "Add a new node to an existing dialog"; 
    }
    const char* getCategory() const override { return "dialogs"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"dialog_id", {
                    {"type", "string"},
                    {"description", "ID of the dialog to add node to"}
                }},
                {"speaker", {
                    {"type", "string"},
                    {"description", "Who is speaking (NPC name or 'Player')"}
                }},
                {"text", {
                    {"type", "string"},
                    {"description", "The dialog text"}
                }},
                {"next_node_id", {
                    {"type", "integer"},
                    {"description", "ID of the next node (-1 to end dialog)"}
                }},
                {"choices", {
                    {"type", "array"},
                    {"description", "Player response choices"},
                    {"items", {
                        {"type", "object"},
                        {"properties", {
                            {"text", {{"type", "string"}}},
                            {"next_node_id", {{"type", "integer"}}},
                            {"tone", {{"type", "string"}}}
                        }}
                    }}
                }}
            }},
            {"required", {"dialog_id", "speaker", "text"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
