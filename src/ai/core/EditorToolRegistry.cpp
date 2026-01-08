/**
 * @file EditorToolRegistry.cpp
 * @brief Implementation of EditorToolRegistry
 */
#include "EditorToolRegistry.h"
#include "engine/utils/Logger.h"

namespace ai {

EditorToolRegistry& EditorToolRegistry::instance() {
    static EditorToolRegistry instance;
    return instance;
}

void EditorToolRegistry::registerTool(std::unique_ptr<IEditorTool> tool) {
    if (!tool) {
        LOG_WARNING("[AI] Attempted to register null tool");
        return;
    }
    
    const std::string name = tool->getName();
    if (m_toolMap.find(name) != m_toolMap.end()) {
        LOG_WARNING("[AI] Tool already registered: " + name);
        return;
    }
    
    LOG_DEBUG("[AI] Registered tool: " + name + " (" + std::string(tool->getCategory()) + ")");
    m_toolMap[name] = tool.get();
    m_tools.push_back(std::move(tool));
}

IEditorTool* EditorToolRegistry::getTool(const std::string& name) const {
    auto it = m_toolMap.find(name);
    if (it != m_toolMap.end()) {
        return it->second;
    }
    return nullptr;
}

const std::vector<std::unique_ptr<IEditorTool>>& EditorToolRegistry::getAllTools() const {
    return m_tools;
}

std::vector<ToolDefinition> EditorToolRegistry::getToolDefinitions() const {
    std::vector<ToolDefinition> definitions;
    definitions.reserve(m_tools.size());
    
    for (const auto& tool : m_tools) {
        ToolDefinition def;
        def.name = tool->getName();
        def.description = tool->getDescription();
        def.parameters = tool->getParameterSchema();
        definitions.push_back(std::move(def));
    }
    
    return definitions;
}

std::vector<IEditorTool*> EditorToolRegistry::getToolsByCategory(const std::string& category) const {
    std::vector<IEditorTool*> result;
    
    for (const auto& tool : m_tools) {
        if (tool->getCategory() == category) {
            result.push_back(tool.get());
        }
    }
    
    return result;
}

bool EditorToolRegistry::hasTool(const std::string& name) const {
    return m_toolMap.find(name) != m_toolMap.end();
}

void EditorToolRegistry::clear() {
    LOG_DEBUG("[AI] Clearing tool registry (" + std::to_string(m_tools.size()) + " tools)");
    m_toolMap.clear();
    m_tools.clear();
}

} // namespace ai
