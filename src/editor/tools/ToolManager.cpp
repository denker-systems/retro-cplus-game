/**
 * @file ToolManager.cpp
 * @brief Implementation of ToolManager
 */
#include "ToolManager.h"
#include "SelectTool.h"
#include "ScaleTool.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ToolManager::ToolManager() {
    // Initialize tools array with nullptr
    m_tools.resize(static_cast<size_t>(ToolType::Count));
    
    // Register default tools
    registerTool(ToolType::Select, std::make_unique<SelectTool>());
    registerTool(ToolType::Scale, std::make_unique<ScaleTool>());
    
    // Set default active tool
    setActiveTool(ToolType::Select);
}

void ToolManager::registerTool(ToolType type, std::unique_ptr<IEditorTool> tool) {
    size_t idx = static_cast<size_t>(type);
    if (idx < m_tools.size()) {
        m_tools[idx] = std::move(tool);
    }
}

void ToolManager::setActiveTool(ToolType type) {
    // Deactivate current tool
    if (auto* current = getActiveTool()) {
        current->deactivate();
        current->setActive(false);
    }
    
    m_activeToolType = type;
    
    // Activate new tool
    if (auto* newTool = getActiveTool()) {
        newTool->activate();
        newTool->setActive(true);
    }
}

IEditorTool* ToolManager::getActiveTool() const {
    return getTool(m_activeToolType);
}

IEditorTool* ToolManager::getTool(ToolType type) const {
    size_t idx = static_cast<size_t>(type);
    if (idx < m_tools.size()) {
        return m_tools[idx].get();
    }
    return nullptr;
}

bool ToolManager::onMouseDown(float roomX, float roomY, ToolContext& ctx) {
    if (auto* tool = getActiveTool()) {
        return tool->onMouseDown(roomX, roomY, ctx);
    }
    return false;
}

bool ToolManager::onMouseUp(float roomX, float roomY, ToolContext& ctx) {
    if (auto* tool = getActiveTool()) {
        return tool->onMouseUp(roomX, roomY, ctx);
    }
    return false;
}

bool ToolManager::onMouseDrag(float roomX, float roomY, float deltaX, float deltaY, ToolContext& ctx) {
    if (auto* tool = getActiveTool()) {
        return tool->onMouseDrag(roomX, roomY, deltaX, deltaY, ctx);
    }
    return false;
}

bool ToolManager::onMouseMove(float roomX, float roomY, ToolContext& ctx) {
    if (auto* tool = getActiveTool()) {
        return tool->onMouseMove(roomX, roomY, ctx);
    }
    return false;
}

bool ToolManager::onKeyDown(int key, ToolContext& ctx) {
    // Handle tool shortcuts
#ifdef HAS_IMGUI
    if (key == ImGuiKey_S) {
        setActiveTool(ToolType::Select);
        return true;
    }
    if (key == ImGuiKey_R) {
        setActiveTool(ToolType::Scale);
        return true;
    }
#endif
    
    if (auto* tool = getActiveTool()) {
        return tool->onKeyDown(key, ctx);
    }
    return false;
}

void ToolManager::renderOverlay(ImDrawList* drawList, const ToolContext& ctx) {
    if (auto* tool = getActiveTool()) {
        tool->renderOverlay(drawList, ctx);
    }
}

void ToolManager::renderToolbar() {
#ifdef HAS_IMGUI
    ImGui::BeginGroup();
    
    for (size_t i = 0; i < m_tools.size(); i++) {
        if (!m_tools[i]) continue;
        
        bool selected = (static_cast<size_t>(m_activeToolType) == i);
        
        if (selected) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
        }
        
        if (ImGui::Button(m_tools[i]->getIcon(), ImVec2(28, 28))) {
            setActiveTool(static_cast<ToolType>(i));
        }
        
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s", m_tools[i]->getTooltip());
        }
        
        if (selected) {
            ImGui::PopStyleColor();
        }
        
        ImGui::SameLine();
    }
    
    ImGui::EndGroup();
#endif
}
