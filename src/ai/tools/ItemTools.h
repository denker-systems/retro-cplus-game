/**
 * @file ItemTools.h
 * @brief AI tools for item manipulation
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ListItemsTool
 * @brief List all items in the game
 */
class ListItemsTool : public IEditorTool {
public:
    const char* getName() const override { return "list_items"; }
    const char* getDescription() const override { 
        return "List all inventory items in the game"; 
    }
    const char* getCategory() const override { return "items"; }
    
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
 * @class GetItemTool
 * @brief Get details of a specific item
 */
class GetItemTool : public IEditorTool {
public:
    const char* getName() const override { return "get_item"; }
    const char* getDescription() const override { 
        return "Get detailed information about a specific item"; 
    }
    const char* getCategory() const override { return "items"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"item_id", {
                    {"type", "string"},
                    {"description", "ID of the item to get"}
                }}
            }},
            {"required", {"item_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class CreateItemTool
 * @brief Create a new inventory item
 */
class CreateItemTool : public IEditorTool {
public:
    const char* getName() const override { return "create_item"; }
    const char* getDescription() const override { 
        return "Create a new inventory item"; 
    }
    const char* getCategory() const override { return "items"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"id", {
                    {"type", "string"},
                    {"description", "Unique item ID (e.g., 'rusty_key')"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "Display name of the item"}
                }},
                {"description", {
                    {"type", "string"},
                    {"description", "Item description shown in inventory"}
                }},
                {"icon", {
                    {"type", "string"},
                    {"description", "Icon sprite path (e.g., 'items/key.png')"}
                }},
                {"combinable", {
                    {"type", "boolean"},
                    {"description", "Whether item can be combined with others"}
                }},
                {"combines_with", {
                    {"type", "string"},
                    {"description", "Item ID this can combine with"}
                }},
                {"combine_result", {
                    {"type", "string"},
                    {"description", "Result item ID after combination"}
                }}
            }},
            {"required", {"id", "name", "description"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class ModifyItemTool
 * @brief Modify an existing item
 */
class ModifyItemTool : public IEditorTool {
public:
    const char* getName() const override { return "modify_item"; }
    const char* getDescription() const override { 
        return "Modify properties of an existing item"; 
    }
    const char* getCategory() const override { return "items"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"item_id", {
                    {"type", "string"},
                    {"description", "ID of the item to modify"}
                }},
                {"name", {
                    {"type", "string"},
                    {"description", "New display name (optional)"}
                }},
                {"description", {
                    {"type", "string"},
                    {"description", "New description (optional)"}
                }},
                {"icon", {
                    {"type", "string"},
                    {"description", "New icon path (optional)"}
                }},
                {"combinable", {
                    {"type", "boolean"},
                    {"description", "Update combinable flag"}
                }},
                {"combines_with", {
                    {"type", "string"},
                    {"description", "New combine target"}
                }},
                {"combine_result", {
                    {"type", "string"},
                    {"description", "New combine result"}
                }}
            }},
            {"required", {"item_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
