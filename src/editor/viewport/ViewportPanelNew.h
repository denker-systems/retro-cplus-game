/**
 * @file ViewportPanelNew.h
 * @brief Unified viewport panel using new architecture
 * 
 * Replaces the old split-file ViewportPanel with a clean OOP design.
 */
#pragma once

#ifdef HAS_IMGUI

#include "ViewportRenderer.h"
#include "ViewportInput.h"
#include <memory>
#include <string>

// Forward declarations
class EditorContext;

namespace engine {
    class World;
    class Level;
    class Scene;
}

namespace editor {

/**
 * @class ViewportPanelNew
 * @brief Main viewport panel with unified 2D/3D rendering at all levels
 */
class ViewportPanelNew {
public:
    ViewportPanelNew(EditorContext& context);
    ~ViewportPanelNew() = default;
    
    /**
     * @brief Render the viewport panel
     */
    void render();
    
    /**
     * @brief Update per-frame logic
     */
    void update(float deltaTime);
    
    // Navigation
    void setWorld(engine::World* world);
    void navigateToLevel(engine::Level* level);
    void navigateToScene(engine::Scene* scene);
    void navigateUp();  // Go to parent level
    
    // State
    engine::World* getWorld() const { return m_world; }
    engine::Level* getActiveLevel() const { return m_activeLevel; }
    engine::Scene* getActiveScene() const { return m_activeScene; }
    HierarchyLevel getHierarchyLevel() const { return m_hierarchyLevel; }
    
    // Visibility
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }

private:
    void renderToolbar();
    void renderBreadcrumbs();
    void handleNavigation();
    
    EditorContext& m_context;
    
    // Rendering
    std::unique_ptr<ViewportRenderer> m_renderer;
    std::unique_ptr<ViewportInput> m_input;
    
    // Navigation state
    engine::World* m_world = nullptr;
    engine::Level* m_activeLevel = nullptr;
    engine::Scene* m_activeScene = nullptr;
    HierarchyLevel m_hierarchyLevel = HierarchyLevel::World;
    
    // UI state
    bool m_visible = true;
    std::string m_title = "Viewport";
    RenderMode m_renderMode = RenderMode::Mode2D;
    
    // Viewport state
    glm::vec2 m_viewportPos{0.0f, 0.0f};
    glm::vec2 m_viewportSize{0.0f, 0.0f};
    bool m_viewportHovered = false;
    bool m_viewportFocused = false;
};

} // namespace editor

#endif // HAS_IMGUI
