/**
 * @file ViewportPanel.h
 * @brief DEPRECATED: OOP renderer architecture (not in use)
 * 
 * MIGRATION NOTE: This is an alternative OOP design that was not adopted.
 * The active ViewportPanel is in src/editor/panels/viewport/ViewportPanel.h
 * 
 * This file is kept for reference but should be removed once migration is complete.
 */
#pragma once

#ifdef HAS_IMGUI

#include "editor/core/IEditorPanel.h"
#include "editor/core/SelectionManager.h"
#include "editor/viewport/IViewportRenderer.h"
#include "editor/viewport/ViewportWorldRenderer.h"
#include "editor/viewport/ViewportLevelRenderer.h"
#include "editor/viewport/ViewportSceneRenderer.h"
#include "editor/viewport/Viewport3DPanel.h"
#include "editor/input/EditorInputController.h"
#include <memory>
#include <string>
#include <SDL.h>

class EditorContext;

namespace engine {
    class World;
    class Level;
    class Scene;
}

namespace editor {

/**
 * @brief Current hierarchy level being viewed
 */
enum class HierarchyLevel {
    World,
    Level,
    Scene
};

/**
 * @brief Render mode
 */
enum class RenderMode {
    Mode2D,
    Mode3D
};

/**
 * @class ViewportPanel
 * @brief Main viewport panel implementing IEditorPanel
 */
class ViewportPanel : public IEditorPanel {
public:
    explicit ViewportPanel(EditorContext& context);
    ~ViewportPanel() override = default;
    
    // IEditorPanel interface
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    void render() override;
    void update(float deltaTime) override;
    
    // Navigation (legacy compatibility)
    void setWorld(engine::World* world);
    void setLevel(engine::Level* level);
    void setScene(engine::Scene* scene);
    void navigateToLevel(engine::Level* level);
    void navigateToScene(engine::Scene* scene);
    void navigateUp();
    
    // Sync with SelectionManager
    void syncFromSelectionManager();
    
    // State accessors
    engine::World* getWorld() const { return m_world; }
    engine::Level* getActiveLevel() const { return m_activeLevel; }
    engine::Scene* getActiveScene() const { return m_activeScene; }
    engine::Scene* getScene() const { return m_activeScene; }
    HierarchyLevel getHierarchyLevel() const { return m_hierarchyLevel; }
    
    // SDL Renderer (legacy compatibility)
    void setRenderer(SDL_Renderer* renderer) { m_sdlRenderer = renderer; }
    void setSDLRenderer(SDL_Renderer* renderer) { m_sdlRenderer = renderer; }
    
    // Selection Manager
    SelectionManager* getSelectionManager() { return &m_selectionManager; }
    
    // Actor operations (for EditorInputHandler)
    void deleteSelectedActor();
    void duplicateSelectedActor();
    
    // Legacy compatibility
    void loadRoom(const std::string& roomId);

private:
    void renderToolbar();
    void renderBreadcrumbs();
    void renderViewportContent();
    void handleInput();
    void handleNavigation();
    
    IViewportRenderer* getCurrentRenderer();
    
    EditorContext& m_context;
    SelectionManager m_selectionManager;
    SDL_Renderer* m_sdlRenderer = nullptr;
    
    // Panel ID
    std::string m_id = "viewport";
    std::string m_title = "Viewport";
    
    // Hierarchy-specific renderers (2D)
    std::unique_ptr<ViewportWorldRenderer> m_worldRenderer;
    std::unique_ptr<ViewportLevelRenderer> m_levelRenderer;
    std::unique_ptr<ViewportSceneRenderer> m_sceneRenderer;
    
    // 3D viewport
    std::unique_ptr<Viewport3DPanel> m_viewport3D;
    
    // Navigation state
    engine::World* m_world = nullptr;
    engine::Level* m_activeLevel = nullptr;
    engine::Scene* m_activeScene = nullptr;
    HierarchyLevel m_hierarchyLevel = HierarchyLevel::World;
    
    // Render mode
    RenderMode m_renderMode = RenderMode::Mode2D;
    
    // Viewport state
    glm::vec2 m_viewportPos{0.0f, 0.0f};
    glm::vec2 m_viewportSize{0.0f, 0.0f};
    float m_zoom = 1.0f;
    float m_panX = 0.0f;
    float m_panY = 0.0f;
    bool m_viewportHovered = false;
    bool m_viewportFocused = false;
};

} // namespace editor

#endif // HAS_IMGUI
