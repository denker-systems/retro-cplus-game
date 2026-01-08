/**
 * @file EditorToolRegistry.h
 * @brief Registry for AI-callable editor tools
 * 
 * @details
 * Central registry that:
 * - Stores all available tools
 * - Provides tool lookup by name
 * - Generates tool definitions for LLM
 */
#pragma once

#include "IEditorTool.h"
#include "ILLMProvider.h"
#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

namespace ai {

/**
 * @class EditorToolRegistry
 * @brief Singleton registry for editor tools
 */
class EditorToolRegistry {
public:
    /**
     * @brief Get singleton instance
     */
    static EditorToolRegistry& instance();
    
    /**
     * @brief Register a tool
     * @param tool Tool to register (takes ownership)
     */
    void registerTool(std::unique_ptr<IEditorTool> tool);
    
    /**
     * @brief Register a tool (convenience template)
     */
    template<typename T, typename... Args>
    void registerTool(Args&&... args) {
        registerTool(std::make_unique<T>(std::forward<Args>(args)...));
    }
    
    /**
     * @brief Get tool by name
     * @param name Tool name
     * @return Tool pointer or nullptr if not found
     */
    IEditorTool* getTool(const std::string& name) const;
    
    /**
     * @brief Get all registered tools
     */
    const std::vector<std::unique_ptr<IEditorTool>>& getAllTools() const;
    
    /**
     * @brief Generate tool definitions for LLM
     * @return Vector of ToolDefinition for API calls
     */
    std::vector<ToolDefinition> getToolDefinitions() const;
    
    /**
     * @brief Get tools by category
     * @param category Category name
     */
    std::vector<IEditorTool*> getToolsByCategory(const std::string& category) const;
    
    /**
     * @brief Check if a tool exists
     */
    bool hasTool(const std::string& name) const;
    
    /**
     * @brief Get tool count
     */
    size_t getToolCount() const { return m_tools.size(); }
    
    /**
     * @brief Clear all tools (for testing)
     */
    void clear();
    
private:
    EditorToolRegistry() = default;
    ~EditorToolRegistry() = default;
    EditorToolRegistry(const EditorToolRegistry&) = delete;
    EditorToolRegistry& operator=(const EditorToolRegistry&) = delete;
    
    std::vector<std::unique_ptr<IEditorTool>> m_tools;
    std::unordered_map<std::string, IEditorTool*> m_toolMap;
};

} // namespace ai
