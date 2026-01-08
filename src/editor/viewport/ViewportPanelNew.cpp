/**
 * @file ViewportPanelNew.cpp
 * @brief Unified viewport panel implementation
 */
#include "ViewportPanelNew.h"
#include "editor/core/EditorContext.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"

#ifdef HAS_IMGUI

#include <imgui.h>

namespace editor {

ViewportPanelNew::ViewportPanelNew(EditorContext& context)
    : m_context(context)
    , m_renderer(std::make_unique<ViewportRenderer>())
    , m_input(std::make_unique<ViewportInput>())
{
}

void ViewportPanelNew::update(float deltaTime) {
    if (m_renderer) {
        m_renderer->update(deltaTime);
    }
}

void ViewportPanelNew::render() {
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Breadcrumbs
        renderBreadcrumbs();
        ImGui::Separator();
        
        // Toolbar
        renderToolbar();
        ImGui::Separator();
        
        // Get viewport region
        ImVec2 viewportPos = ImGui::GetCursorScreenPos();
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        
        m_viewportPos = glm::vec2(viewportPos.x, viewportPos.y);
        m_viewportSize = glm::vec2(viewportSize.x, viewportSize.y);
        
        // Invisible button to capture input
        ImGui::InvisibleButton("viewport_area", viewportSize, 
            ImGuiButtonFlags_MouseButtonLeft | 
            ImGuiButtonFlags_MouseButtonRight | 
            ImGuiButtonFlags_MouseButtonMiddle);
        
        m_viewportHovered = ImGui::IsItemHovered();
        m_viewportFocused = ImGui::IsItemFocused();
        
        // Configure renderer
        m_renderer->setHierarchyLevel(m_hierarchyLevel);
        m_renderer->setRenderMode(m_renderMode);
        m_renderer->setWorld(m_world);
        m_renderer->setActiveLevel(m_activeLevel);
        m_renderer->setActiveScene(m_activeScene);
        
        // Render content
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        m_renderer->render(drawList, m_viewportPos, m_viewportSize);
        
        // Process input
        m_input->processInput(*m_renderer, m_viewportPos, m_viewportSize, 
                              m_viewportHovered, m_viewportFocused);
        
        // Handle navigation on double-click
        handleNavigation();
    }
    ImGui::End();
}

void ViewportPanelNew::renderToolbar() {
    // 2D/3D toggle
    ImGui::PushStyleColor(ImGuiCol_Button, m_renderMode == RenderMode::Mode2D ? 
        ImVec4(0.2f, 0.5f, 0.8f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("2D")) {
        m_renderMode = RenderMode::Mode2D;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    ImGui::PushStyleColor(ImGuiCol_Button, m_renderMode == RenderMode::Mode3D ? 
        ImVec4(0.2f, 0.5f, 0.8f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("3D")) {
        m_renderMode = RenderMode::Mode3D;
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    
    // Zoom control (2D only)
    if (m_renderMode == RenderMode::Mode2D) {
        float zoom = m_renderer->getZoom() * 100.0f;
        ImGui::Text("Zoom: %.0f%%", zoom);
        ImGui::SameLine();
        if (ImGui::Button("-")) {
            m_renderer->setZoom(m_renderer->getZoom() * 0.9f);
        }
        ImGui::SameLine();
        if (ImGui::Button("+")) {
            m_renderer->setZoom(m_renderer->getZoom() * 1.1f);
        }
        ImGui::SameLine();
        if (ImGui::Button("100%")) {
            m_renderer->setZoom(1.0f);
            m_renderer->setPan(glm::vec2(0.0f));
        }
    }
}

void ViewportPanelNew::renderBreadcrumbs() {
    // World
    if (m_world) {
        if (ImGui::SmallButton(("World: " + m_world->getName()).c_str())) {
            m_activeLevel = nullptr;
            m_activeScene = nullptr;
            m_hierarchyLevel = HierarchyLevel::World;
            m_renderer->clearSelection();
        }
    }
    
    // Level
    if (m_activeLevel) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        if (ImGui::SmallButton(m_activeLevel->getName().c_str())) {
            m_activeScene = nullptr;
            m_hierarchyLevel = HierarchyLevel::Level;
            m_renderer->clearSelection();
        }
    }
    
    // Scene
    if (m_activeScene) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        ImGui::Text("%s", m_activeScene->getName().c_str());
    }
}

void ViewportPanelNew::handleNavigation() {
    if (!m_input->wasDoubleClicked()) return;
    
    int selectedIdx = m_renderer->getSelectedIndex();
    if (selectedIdx < 0) return;
    
    const auto& pickables = m_renderer->getPickableObjects();
    if (selectedIdx >= (int)pickables.size()) return;
    
    void* userData = pickables[selectedIdx].userData;
    
    switch (m_hierarchyLevel) {
        case HierarchyLevel::World:
            // Navigate to level
            if (m_world) {
                const auto& levels = m_world->getLevels();
                if (selectedIdx < (int)levels.size()) {
                    navigateToLevel(levels[selectedIdx].get());
                }
            }
            break;
            
        case HierarchyLevel::Level:
            // Navigate to scene
            if (m_activeLevel) {
                const auto& scenes = m_activeLevel->getScenes();
                if (selectedIdx < (int)scenes.size()) {
                    navigateToScene(scenes[selectedIdx].get());
                }
            }
            break;
            
        case HierarchyLevel::Scene:
            // Select actor (already selected, no navigation)
            break;
    }
    
    m_input->clearClickState();
}

void ViewportPanelNew::setWorld(engine::World* world) {
    m_world = world;
    m_activeLevel = nullptr;
    m_activeScene = nullptr;
    m_hierarchyLevel = HierarchyLevel::World;
    m_renderer->clearSelection();
}

void ViewportPanelNew::navigateToLevel(engine::Level* level) {
    m_activeLevel = level;
    m_activeScene = nullptr;
    m_hierarchyLevel = HierarchyLevel::Level;
    m_renderer->clearSelection();
}

void ViewportPanelNew::navigateToScene(engine::Scene* scene) {
    m_activeScene = scene;
    m_hierarchyLevel = HierarchyLevel::Scene;
    m_renderer->clearSelection();
}

void ViewportPanelNew::navigateUp() {
    switch (m_hierarchyLevel) {
        case HierarchyLevel::Scene:
            m_activeScene = nullptr;
            m_hierarchyLevel = HierarchyLevel::Level;
            break;
        case HierarchyLevel::Level:
            m_activeLevel = nullptr;
            m_hierarchyLevel = HierarchyLevel::World;
            break;
        case HierarchyLevel::World:
            // Already at top
            break;
    }
    m_renderer->clearSelection();
}

} // namespace editor

#endif // HAS_IMGUI
