/**
 * @file CommandTools.cpp
 * @brief Implementation of AI tools for command execution
 */
#include "CommandTools.h"
#include "ai/core/EditorToolRegistry.h"
#include "engine/utils/Logger.h"

namespace ai {

ToolResult ExecuteCommandTool::execute(const nlohmann::json& params) {
    std::string command = params.value("command", "");
    
    if (command.empty()) {
        return ToolResult::error("command is required");
    }
    
    LOG_INFO("[AI] ExecuteCommandTool: " + command);
    
    // Parse command string into tool name and parameters
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    
    for (char c : command) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    if (tokens.empty()) {
        return ToolResult::error("Empty command");
    }
    
    std::string toolName = tokens[0];
    
    // Find the tool
    auto& registry = EditorToolRegistry::instance();
    IEditorTool* tool = registry.getTool(toolName);
    
    if (!tool) {
        return ToolResult::error("Unknown command: " + toolName);
    }
    
    // Build parameters from remaining tokens (key=value format)
    nlohmann::json toolParams = nlohmann::json::object();
    
    for (size_t i = 1; i < tokens.size(); i++) {
        const std::string& token = tokens[i];
        size_t eqPos = token.find('=');
        if (eqPos != std::string::npos) {
            std::string key = token.substr(0, eqPos);
            std::string value = token.substr(eqPos + 1);
            
            // Try to parse as number
            try {
                if (value.find('.') != std::string::npos) {
                    toolParams[key] = std::stod(value);
                } else {
                    toolParams[key] = std::stoi(value);
                }
            } catch (...) {
                toolParams[key] = value;
            }
        }
    }
    
    // Execute the tool
    auto result = tool->execute(toolParams);
    
    if (result.success) {
        std::string output = result.message;
        if (!result.data.empty()) {
            output += "\n" + result.data.dump(2);
        }
        return ToolResult::ok(output, result.data);
    } else {
        return ToolResult::error(result.message);
    }
}

ToolResult ListCommandsTool::execute(const nlohmann::json& params) {
    auto& registry = EditorToolRegistry::instance();
    
    nlohmann::json commands = nlohmann::json::array();
    
    for (const auto& tool : registry.getAllTools()) {
        commands.push_back({
            {"name", tool->getName()},
            {"description", tool->getDescription()},
            {"category", tool->getCategory()}
        });
    }
    
    nlohmann::json result = {
        {"command_count", commands.size()},
        {"commands", commands}
    };
    
    return ToolResult::ok("Found " + std::to_string(commands.size()) + " commands", result);
}

} // namespace ai
