/**
 * @file IEditorTool.h
 * @brief Interface for AI-callable editor tools
 * 
 * @details
 * Tools are actions that the AI agent can execute in the editor.
 * Each tool:
 * - Has a name and description
 * - Defines input parameters via JSON Schema
 * - Executes actions through IEditorCommand for undo/redo
 */
#pragma once

#include <string>
#include <memory>
#include <nlohmann/json.hpp>
#include "editor/core/IEditorCommand.h"

namespace ai {

/**
 * @brief Result of tool execution
 */
struct ToolResult {
    bool success = true;
    std::string message;
    nlohmann::json data;
    std::unique_ptr<IEditorCommand> command;  // For undo/redo
    
    static ToolResult ok(const std::string& msg = "Success") {
        return {true, msg, {}, nullptr};
    }
    
    static ToolResult ok(const std::string& msg, const nlohmann::json& data) {
        return {true, msg, data, nullptr};
    }
    
    static ToolResult error(const std::string& msg) {
        return {false, msg, {}, nullptr};
    }
};

/**
 * @class IEditorTool
 * @brief Abstract interface for AI-callable editor tools
 * 
 * @details
 * Implementations should:
 * - Validate input parameters
 * - Execute editor actions
 * - Return IEditorCommand for undo support
 * - Handle errors gracefully
 * 
 * @par Example Implementation
 * @code
 * class CreateRoomTool : public IEditorTool {
 * public:
 *     const char* getName() const override { return "create_room"; }
 *     const char* getDescription() const override { 
 *         return "Create a new room in the game"; 
 *     }
 *     nlohmann::json getParameterSchema() const override {
 *         return {
 *             {"type", "object"},
 *             {"properties", {
 *                 {"name", {{"type", "string"}, {"description", "Room name"}}},
 *                 {"background", {{"type", "string"}, {"description", "Background image path"}}}
 *             }},
 *             {"required", {"name"}}
 *         };
 *     }
 *     ToolResult execute(const nlohmann::json& params) override {
 *         std::string name = params["name"];
 *         // Create room...
 *         return ToolResult::ok("Created room: " + name);
 *     }
 * };
 * @endcode
 */
class IEditorTool {
public:
    virtual ~IEditorTool() = default;
    
    /**
     * @brief Get tool name (used in API calls)
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Get human-readable description
     */
    virtual const char* getDescription() const = 0;
    
    /**
     * @brief Get JSON Schema for input parameters
     */
    virtual nlohmann::json getParameterSchema() const = 0;
    
    /**
     * @brief Execute the tool with given parameters
     * @param params JSON object matching the parameter schema
     * @return Result with success/error and optional command for undo
     */
    virtual ToolResult execute(const nlohmann::json& params) = 0;
    
    /**
     * @brief Check if tool requires user confirmation
     * @details Destructive operations should return true
     */
    virtual bool requiresConfirmation() const { return false; }
    
    /**
     * @brief Get category for UI organization
     */
    virtual const char* getCategory() const { return "general"; }
};

} // namespace ai
