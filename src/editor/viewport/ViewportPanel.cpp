/**
 * @file ViewportPanel.cpp
 * @brief Unified viewport panel implementation using OOP renderers
 */
#include "editor/viewport/ViewportPanel.h"
#include "editor/core/EditorContext.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include <iostream>

#ifdef HAS_IMGUI

#include <imgui.h>

namespace editor {

ViewportPanel::ViewportPanel(EditorContext& context)
    : m_context(context)
    , m_selectionManager(context)
    , m_worldRenderer(std::make_unique<ViewportWorldRenderer>())
    , m_levelRenderer(std::make_unique<ViewportLevelRenderer>())
    , m_sceneRenderer(std::make_unique<ViewportSceneRenderer>())
    , m_viewport3D(std::make_unique<Viewport3DPanel>())
{
    EditorInputController::instance().initialize();
    
    // Initialize 2D renderers with selection manager
    m_worldRenderer->initialize(&m_context, &m_selectionManager);
    m_levelRenderer->initialize(&m_context, &m_selectionManager);
    m_sceneRenderer->initialize(&m_context, &m_selectionManager);
    
    // Initialize 3D viewport
    m_viewport3D->initialize();
    m_viewport3D->setSelectionManager(&m_selectionManager);
}

void ViewportPanel::update(float deltaTime) {
    if (m_renderMode == RenderMode::Mode3D) {
        if (m_viewport3D) {
            m_viewport3D->update(deltaTime);
        }
    } else {
        if (auto* renderer = getCurrentRenderer()) {
            renderer->update(deltaTime);
        }
    }
}

void ViewportPanel::render() {
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        renderBreadcrumbs();
        ImGui::Separator();
        renderToolbar();
        ImGui::Separator();
        
        // Get viewport region
        ImVec2 viewportPos = ImGui::GetCursorScreenPos();
        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        
        m_viewportPos = glm::vec2(viewportPos.x, viewportPos.y);
        m_viewportSize = glm::vec2(viewportSize.x, viewportSize.y);
        
        // Invisible button for input capture
        ImGui::InvisibleButton("viewport_area", viewportSize, 
            ImGuiButtonFlags_MouseButtonLeft | 
            ImGuiButtonFlags_MouseButtonRight | 
            ImGuiButtonFlags_MouseButtonMiddle);
        
        m_viewportHovered = ImGui::IsItemHovered();
        m_viewportFocused = ImGui::IsItemFocused();
        
        // Render content via current renderer
        renderViewportContent();
        
        // Handle input
        handleInput();
        handleNavigation();
    }
    ImGui::End();
}

void ViewportPanel::renderToolbar() {
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
        ImGui::Text("Zoom: %.0f%%", m_zoom * 100.0f);
        ImGui::SameLine();
        if (ImGui::Button("-")) { m_zoom = std::max(0.25f, m_zoom * 0.9f); }
        ImGui::SameLine();
        if (ImGui::Button("+")) { m_zoom = std::min(4.0f, m_zoom * 1.1f); }
        ImGui::SameLine();
        if (ImGui::Button("100%")) { m_zoom = 1.0f; m_panX = 0; m_panY = 0; }
    }
}

void ViewportPanel::renderBreadcrumbs() {
    if (m_world) {
        if (ImGui::SmallButton(("World: " + m_world->getName()).c_str())) {
            m_activeLevel = nullptr;
            m_activeScene = nullptr;
            m_hierarchyLevel = HierarchyLevel::World;
            if (auto* r = getCurrentRenderer()) r->clearSelection();
        }
    }
    
    if (m_activeLevel) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        if (ImGui::SmallButton(m_activeLevel->getName().c_str())) {
            m_activeScene = nullptr;
            m_hierarchyLevel = HierarchyLevel::Level;
            if (auto* r = getCurrentRenderer()) r->clearSelection();
        }
    }
    
    if (m_activeScene) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        ImGui::Text("%s", m_activeScene->getName().c_str());
    }
}

void ViewportPanel::renderViewportContent() {
    // Debug: Check what we have
    std::cout << "[ViewportPanel] renderViewportContent - Mode: " << (m_renderMode == RenderMode::Mode2D ? "2D" : "3D")
              << ", Level: " << static_cast<int>(m_hierarchyLevel)
              << ", World: " << (m_world ? "YES" : "NULL")
              << ", ActiveLevel: " << (m_activeLevel ? "YES" : "NULL")
              << ", ActiveScene: " << (m_activeScene ? m_activeScene->getName() : "NULL") << std::endl;
    
    // 3D mode - use Viewport3DPanel
    if (m_renderMode == RenderMode::Mode3D) {
        if (m_viewport3D && m_viewport3D->isInitialized()) {
            // Configure 3D viewport based on hierarchy level
            switch (m_hierarchyLevel) {
                case HierarchyLevel::World:
                    m_viewport3D->setViewLevel(View3DLevel::World);
                    m_viewport3D->setWorld(m_world);
                    break;
                case HierarchyLevel::Level:
                    m_viewport3D->setViewLevel(View3DLevel::Level);
                    m_viewport3D->setLevel(m_activeLevel);
                    break;
                case HierarchyLevel::Scene:
                    m_viewport3D->setViewLevel(View3DLevel::Scene);
                    m_viewport3D->setScene(m_activeScene);
                    break;
            }
            m_viewport3D->render();
        } else {
            // Show error message if 3D not initialized
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "3D viewport not initialized");
            ImGui::Text("OpenGL may not be available");
        }
        return;
    }
    
    // 2D mode - use OOP renderers
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    ViewportRenderContext ctx;
    ctx.viewportPos = m_viewportPos;
    ctx.viewportSize = m_viewportSize;
    ctx.zoom = m_zoom;
    ctx.panX = m_panX;
    ctx.panY = m_panY;
    ctx.isHovered = m_viewportHovered;
    ctx.isFocused = m_viewportFocused;
    
    // Configure and render based on hierarchy level
    switch (m_hierarchyLevel) {
        case HierarchyLevel::World:
            m_worldRenderer->setWorld(m_world);
            m_worldRenderer->render(drawList, ctx);
            break;
            
        case HierarchyLevel::Level:
            m_levelRenderer->setLevel(m_activeLevel);
            m_levelRenderer->render(drawList, ctx);
            break;
            
        case HierarchyLevel::Scene:
            m_sceneRenderer->setScene(m_activeScene);
            m_sceneRenderer->setSDLRenderer(m_sdlRenderer);
            m_sceneRenderer->render(drawList, ctx);
            break;
    }
}

void ViewportPanel::handleInput() {
    if (!m_viewportHovered) return;
    
    ViewportRenderContext ctx;
    ctx.viewportPos = m_viewportPos;
    ctx.viewportSize = m_viewportSize;
    ctx.zoom = m_zoom;
    ctx.panX = m_panX;
    ctx.panY = m_panY;
    
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    float mx = mousePos.x;
    float my = mousePos.y;
    
    auto* renderer = getCurrentRenderer();
    if (!renderer) return;
    
    // Mouse click
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        renderer->onMouseClick(mx, my, ctx);
    }
    
    // Mouse drag
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        renderer->onMouseDrag(mx, my, delta.x, delta.y, ctx);
    }
    
    // Mouse release
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        renderer->onMouseRelease(mx, my, ctx);
    }
    
    // Middle mouse pan
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        m_panX += delta.x;
        m_panY += delta.y;
    }
    
    // Scroll zoom
    float wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0.0f) {
        m_zoom = std::clamp(m_zoom + wheel * 0.1f, 0.25f, 4.0f);
    }
}

IViewportRenderer* ViewportPanel::getCurrentRenderer() {
    switch (m_hierarchyLevel) {
        case HierarchyLevel::World:  return m_worldRenderer.get();
        case HierarchyLevel::Level:  return m_levelRenderer.get();
        case HierarchyLevel::Scene:  return m_sceneRenderer.get();
    }
    return nullptr;
}

void ViewportPanel::handleNavigation() {
    // Check for double-click navigation
    ViewportRenderContext ctx;
    ctx.viewportPos = m_viewportPos;
    ctx.viewportSize = m_viewportSize;
    ctx.zoom = m_zoom;
    ctx.panX = m_panX;
    ctx.panY = m_panY;
    
    if (!ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) || !m_viewportHovered) return;
    
    ImVec2 mousePos = ImGui::GetIO().MousePos;
    auto* renderer = getCurrentRenderer();
    if (!renderer) return;
    
    bool shouldNavigate = renderer->onDoubleClick(mousePos.x, mousePos.y, ctx);
    if (!shouldNavigate) return;
    
    void* selected = renderer->getSelectedItem();
    if (!selected) return;
    
    switch (m_hierarchyLevel) {
        case HierarchyLevel::World:
            navigateToLevel(static_cast<engine::Level*>(selected));
            break;
            
        case HierarchyLevel::Level:
            navigateToScene(static_cast<engine::Scene*>(selected));
            break;
            
        case HierarchyLevel::Scene:
            // No navigation from scene level
            break;
    }
}

void ViewportPanel::setWorld(engine::World* world) {
    std::cout << "[ViewportPanel] setWorld: " << (world ? world->getName() : "NULL") << std::endl;
    m_world = world;
    m_activeLevel = nullptr;
    m_activeScene = nullptr;
    m_hierarchyLevel = HierarchyLevel::World;
    m_worldRenderer->clearSelection();
    
    // Sync SelectionManager with world
    m_selectionManager.setWorld(world);
}

void ViewportPanel::navigateToLevel(engine::Level* level) {
    m_activeLevel = level;
    m_activeScene = nullptr;
    m_hierarchyLevel = HierarchyLevel::Level;
    m_levelRenderer->clearSelection();
}

void ViewportPanel::navigateToScene(engine::Scene* scene) {
    m_activeScene = scene;
    m_hierarchyLevel = HierarchyLevel::Scene;
    m_sceneRenderer->clearSelection();
}

void ViewportPanel::navigateUp() {
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
            break;
    }
    if (auto* r = getCurrentRenderer()) r->clearSelection();
}

void ViewportPanel::setLevel(engine::Level* level) {
    std::cout << "[ViewportPanel] setLevel: " << (level ? level->getName() : "NULL") << std::endl;
    m_activeLevel = level;
    if (level) {
        m_hierarchyLevel = HierarchyLevel::Level;
    }
    m_levelRenderer->clearSelection();
    m_selectionManager.setActiveLevel(level);
}

void ViewportPanel::setScene(engine::Scene* scene) {
    std::cout << "[ViewportPanel] setScene: " << (scene ? scene->getName() : "NULL") << std::endl;
    m_activeScene = scene;
    if (scene) {
        m_hierarchyLevel = HierarchyLevel::Scene;
    }
    m_sceneRenderer->clearSelection();
    m_selectionManager.setActiveScene(scene);
}

void ViewportPanel::syncFromSelectionManager() {
    // Sync navigation state from SelectionManager
    if (m_selectionManager.getWorld()) {
        m_world = m_selectionManager.getWorld();
    }
    if (m_selectionManager.getActiveLevel()) {
        m_activeLevel = m_selectionManager.getActiveLevel();
        m_hierarchyLevel = HierarchyLevel::Level;
    }
    if (m_selectionManager.getActiveScene()) {
        m_activeScene = m_selectionManager.getActiveScene();
        m_hierarchyLevel = HierarchyLevel::Scene;
    }
}

void ViewportPanel::deleteSelectedActor() {
    if (m_hierarchyLevel != HierarchyLevel::Scene || !m_activeScene) return;
    
    auto* selectedActor = static_cast<engine::ActorObjectExtended*>(
        m_sceneRenderer->getSelectedItem());
    if (!selectedActor) return;
    
    // TODO: Use CommandManager for undo support
    m_activeScene->removeActor(selectedActor);
    m_sceneRenderer->clearSelection();
}

void ViewportPanel::duplicateSelectedActor() {
    if (m_hierarchyLevel != HierarchyLevel::Scene || !m_activeScene) return;
    
    auto* selectedActor = static_cast<engine::ActorObjectExtended*>(
        m_sceneRenderer->getSelectedItem());
    if (!selectedActor) return;
    
    // TODO: Implement actor duplication
}

void ViewportPanel::loadRoom(const std::string& roomId) {
    // Legacy compatibility - find scene by room ID
    if (!m_world) return;
    
    for (const auto& level : m_world->getLevels()) {
        for (const auto& scene : level->getScenes()) {
            if (scene->getName() == roomId) {
                navigateToLevel(level.get());
                navigateToScene(scene.get());
                return;
            }
        }
    }
}

} // namespace editor

#endif // HAS_IMGUI
