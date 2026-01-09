/**
 * @file ViewportPanel.h
 * @brief Main viewport panel orchestrator
 */
#pragma once

#include "ViewportTypes.h"
#include "editor/core/IEditorPanel.h"
#include <memory>
#include <string>

// Forward declarations
class EditorContext;
class SelectionManager;
class ToolManager;
class EditorPlayMode;

struct ImDrawList;
struct ImVec2;
struct SceneData;

namespace engine {
    class World;
    class Level;
    class Scene;
    class ActorObjectExtended;
}

namespace editor {

class Viewport3DPanel;
class EditorPlayMode;

namespace viewport {

class ViewportToolbar;
class ViewportBreadcrumbs;
class BaseViewRenderer;
class World2DRenderer;
class World3DRenderer;

/**
 * @class ViewportPanel
 * @brief Main viewport panel that orchestrates all viewport rendering
 * 
 * This is a lightweight orchestrator that delegates to specialized renderers.
 * Max 275 lines to maintain clean architecture.
 */
class ViewportPanel : public IEditorPanel {
public:
    ViewportPanel(EditorContext& context);
    ~ViewportPanel() override;
    
    // IEditorPanel interface
    void update(float deltaTime) override;
    void render() override;
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    // Navigation
    void setWorld(engine::World* world);
    void setLevel(engine::Level* level);
    void setScene(engine::Scene* scene);
    void syncFromSelectionManager();
    
    // Actor operations
    void deleteSelectedActor();
    void duplicateSelectedActor();
    
    // Selection manager access
    SelectionManager* getSelectionManager() const { return m_selectionManager; }
    
    // Play mode
    void setPlayMode(EditorPlayMode* playMode) { m_playMode = playMode; }

private:
    void renderContent();
    void handleNavigation();
    
    EditorContext& m_context;
    SelectionManager* m_selectionManager = nullptr;
    EditorPlayMode* m_playMode = nullptr;
    
    std::string m_id = "viewport";
    std::string m_title = "Viewport";
    
    // UI components
    std::unique_ptr<ViewportToolbar> m_toolbar;
    std::unique_ptr<ViewportBreadcrumbs> m_breadcrumbs;
    
    // Renderers using inheritance hierarchy
    // 3D is PRIMARY (default), 2D is SECONDARY (fallback)
    std::unique_ptr<BaseViewRenderer> m_3dRenderer;  // World3D/Level3D/Scene3D
    std::unique_ptr<BaseViewRenderer> m_2dRenderer;  // World2D→Level2D→Scene2D
    
    // Navigation state
    engine::World* m_world = nullptr;
    engine::Level* m_activeLevel = nullptr;
    engine::Scene* m_activeScene = nullptr;
    HierarchyLevel m_hierarchyLevel = HierarchyLevel::World;
    
    // Render state
    RenderContext m_renderContext;
    RenderMode m_renderMode = RenderMode::Mode3D;  // 3D is PRIMARY (default)
};

} // namespace viewport
} // namespace editor
