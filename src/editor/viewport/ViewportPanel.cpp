/**
 * @file ViewportPanel.cpp
 * @brief Implementation of main viewport panel with inheritance hierarchy
 */
#include "ViewportPanel.h"
#include "ui/ViewportToolbar.h"
#include "ui/ViewportBreadcrumbs.h"
#include "renderers/3d/World3DRenderer.h"
#include "renderers/3d/Level3DRenderer.h"
#include "renderers/3d/Scene3DRenderer.h"
#include "renderers/2d/World2DRenderer.h"
#include "renderers/2d/Level2DRenderer.h"
#include "renderers/2d/Scene2DRenderer.h"
#include "editor/core/EditorContext.h"
#include "editor/core/SelectionManager.h"
#include "editor/tools/ToolManager.h"
#include "editor/core/EditorPlayMode.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

ViewportPanel::ViewportPanel(EditorContext& context)
    : m_context(context)
{
    m_selectionManager = new SelectionManager(context);
    
    // Create UI components
    m_toolbar = std::make_unique<ViewportToolbar>();
    m_breadcrumbs = std::make_unique<ViewportBreadcrumbs>();
    
    // Create renderers using inheritance hierarchy
    // Start with Scene renderers (most specific) which inherit from Level and World
    m_3dRenderer = std::make_unique<Scene3DRenderer>();  // 3D PRIMARY
    m_2dRenderer = std::make_unique<Scene2DRenderer>();  // 2D SECONDARY (inherits Level2D→World2D)
    
    // Initialize renderers
    m_3dRenderer->initialize(&m_context, m_selectionManager);
    m_2dRenderer->initialize(&m_context, m_selectionManager);
    
    // Initialize render context
    m_renderContext.zoom = 1.0f;
    m_renderContext.panX = 0.0f;
    m_renderContext.panY = 0.0f;
    m_renderContext.renderMode = RenderMode::Mode3D;  // 3D is PRIMARY
}

ViewportPanel::~ViewportPanel() {
    delete m_selectionManager;
}

void ViewportPanel::update(float deltaTime) {
    if (m_3dRenderer) m_3dRenderer->update(deltaTime);
    if (m_2dRenderer) m_2dRenderer->update(deltaTime);
}

void ViewportPanel::render() {
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Breadcrumbs
        if (m_breadcrumbs) {
            m_breadcrumbs->render(m_world, m_activeLevel, m_activeScene);
            handleNavigation();
        }
        ImGui::Separator();
        
        // Toolbar
        if (m_toolbar) {
            m_toolbar->render(m_renderContext.renderMode, m_renderContext.zoom, 
                            m_renderContext.panX, m_renderContext.panY, m_playMode);
            
            // Update render context from toolbar
            m_renderContext.showGrid = m_toolbar->showGrid();
            m_renderContext.showHotspots = m_toolbar->showHotspots();
            m_renderContext.showWalkArea = m_toolbar->showWalkArea();
            m_renderContext.showPhysicsDebug = m_toolbar->showPhysicsDebug();
        }
        
        // Content area
        renderContent();
    }
    ImGui::End();
}

void ViewportPanel::renderContent() {
    // Get viewport region
    ImVec2 viewportPos = ImGui::GetCursorScreenPos();
    ImVec2 viewportSize = ImGui::GetContentRegionAvail();
    
    m_renderContext.viewportPos = glm::vec2(viewportPos.x, viewportPos.y);
    m_renderContext.viewportSize = glm::vec2(viewportSize.x, viewportSize.y);
    m_renderContext.renderMode = m_renderMode;
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Render based on hierarchy level (like old ViewportPanel)
    if (!m_activeLevel) {
        // World level - show all levels
        if (m_renderMode == RenderMode::Mode3D) {
            if (auto* world3D = dynamic_cast<World3DRenderer*>(m_3dRenderer.get())) {
                world3D->setWorld(m_world);
                m_3dRenderer->render(drawList, m_renderContext);
            }
        } else {
            if (auto* scene2D = dynamic_cast<Scene2DRenderer*>(m_2dRenderer.get())) {
                scene2D->setWorld(m_world);
                scene2D->setLevel(nullptr);
                scene2D->setScene(nullptr);
                m_2dRenderer->render(drawList, m_renderContext);
            }
        }
    } else if (!m_activeScene) {
        // Level level - show all scenes
        if (m_renderMode == RenderMode::Mode3D) {
            if (auto* level3D = dynamic_cast<Level3DRenderer*>(m_3dRenderer.get())) {
                level3D->setLevel(m_activeLevel);
                m_3dRenderer->render(drawList, m_renderContext);
            }
        } else {
            if (auto* scene2D = dynamic_cast<Scene2DRenderer*>(m_2dRenderer.get())) {
                scene2D->setWorld(m_world);
                scene2D->setLevel(m_activeLevel);
                scene2D->setScene(nullptr);
                m_2dRenderer->render(drawList, m_renderContext);
            }
        }
    } else {
        // Scene level - show scene content
        if (m_renderMode == RenderMode::Mode3D) {
            if (auto* scene3D = dynamic_cast<Scene3DRenderer*>(m_3dRenderer.get())) {
                scene3D->setScene(m_activeScene);
                m_3dRenderer->render(drawList, m_renderContext);
            }
        } else {
            if (auto* scene2D = dynamic_cast<Scene2DRenderer*>(m_2dRenderer.get())) {
                scene2D->setWorld(m_world);
                scene2D->setLevel(m_activeLevel);
                scene2D->setScene(m_activeScene);
                m_2dRenderer->render(drawList, m_renderContext);
            }
        }
    }
    
    // Check for navigation (2D only)
    if (m_renderMode == RenderMode::Mode2D && m_2dRenderer) {
        if (m_2dRenderer->wasDoubleClicked()) {
            if (!m_activeLevel) {
                // World level - check for level selection
                if (auto* world2D = dynamic_cast<World2DRenderer*>(m_2dRenderer.get())) {
                    if (auto* level = world2D->getSelectedLevel()) {
                        setLevel(level);
                    }
                }
            }
            else if (!m_activeScene) {
                // Level level - check for scene selection
                if (auto* level2D = dynamic_cast<Level2DRenderer*>(m_2dRenderer.get())) {
                    if (auto* scene = level2D->getSelectedScene()) {
                        setScene(scene);
                    }
                }
            }
            m_2dRenderer->clearDoubleClick();
        }
    }
}

void ViewportPanel::handleNavigation() {
    if (!m_breadcrumbs) return;
    
    if (m_breadcrumbs->shouldNavigateToWorld()) {
        m_activeLevel = nullptr;
        m_activeScene = nullptr;
        m_hierarchyLevel = HierarchyLevel::World;
        m_breadcrumbs->resetNavigation();
    }
    else if (m_breadcrumbs->shouldNavigateToLevel()) {
        m_activeScene = nullptr;
        m_hierarchyLevel = HierarchyLevel::Level;
        m_breadcrumbs->resetNavigation();
    }
}

void ViewportPanel::setWorld(engine::World* world) {
    m_world = world;
    m_activeLevel = nullptr;
    m_activeScene = nullptr;
    m_hierarchyLevel = HierarchyLevel::World;
    
    if (m_selectionManager) {
        m_selectionManager->setWorld(world);
    }
}

void ViewportPanel::setLevel(engine::Level* level) {
    m_activeLevel = level;
    m_activeScene = nullptr;
    m_hierarchyLevel = level ? HierarchyLevel::Level : HierarchyLevel::World;
    
    if (m_selectionManager) {
        m_selectionManager->setActiveLevel(level);
        m_selectionManager->setActiveScene(nullptr);
    }
}

void ViewportPanel::setScene(engine::Scene* scene) {
    m_activeScene = scene;
    m_hierarchyLevel = scene ? HierarchyLevel::Scene : HierarchyLevel::Level;
    
    if (m_selectionManager) {
        m_selectionManager->setActiveScene(scene);
    }
    
    if (m_playMode && scene) {
        m_playMode->setActiveScene(scene);
    }
}

void ViewportPanel::syncFromSelectionManager() {
    if (!m_selectionManager) return;
    
    m_world = m_selectionManager->getWorld();
    m_activeLevel = m_selectionManager->getActiveLevel();
    m_activeScene = m_selectionManager->getActiveScene();
    
    // Update hierarchy level
    if (m_activeScene) {
        m_hierarchyLevel = HierarchyLevel::Scene;
    } else if (m_activeLevel) {
        m_hierarchyLevel = HierarchyLevel::Level;
    } else {
        m_hierarchyLevel = HierarchyLevel::World;
    }
    
    // Update EditorPlayMode with active scene
    if (m_playMode && m_activeScene) {
        m_playMode->setActiveScene(m_activeScene);
    }
}

void ViewportPanel::deleteSelectedActor() {
    if (auto* scene2D = dynamic_cast<Scene2DRenderer*>(m_2dRenderer.get())) {
        scene2D->deleteSelectedActor();
    }
}

void ViewportPanel::duplicateSelectedActor() {
    if (auto* scene2D = dynamic_cast<Scene2DRenderer*>(m_2dRenderer.get())) {
        scene2D->duplicateSelectedActor();
    }
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
