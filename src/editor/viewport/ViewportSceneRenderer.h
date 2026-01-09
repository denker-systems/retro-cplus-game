/**
 * @file ViewportSceneRenderer.h
 * @brief Renderer for Scene-level viewport (shows Actors)
 */
#pragma once

#include "IViewportRenderer.h"
#include "editor/tools/ToolManager.h"
#include <SDL.h>

namespace engine {
    class Scene;
    class ActorObjectExtended;
}

namespace editor {

/**
 * @brief Renders Scene view showing all Actors
 */
class ViewportSceneRenderer : public IViewportRenderer {
public:
    ViewportSceneRenderer() = default;
    ~ViewportSceneRenderer() override = default;
    
    void initialize(EditorContext* context, SelectionManager* selectionManager) override;
    void render(ImDrawList* drawList, const ViewportRenderContext& ctx) override;
    void update(float deltaTime) override;
    
    bool onMouseClick(float x, float y, const ViewportRenderContext& ctx) override;
    bool onMouseDrag(float x, float y, float deltaX, float deltaY, const ViewportRenderContext& ctx) override;
    void onMouseRelease(float x, float y, const ViewportRenderContext& ctx) override;
    bool onDoubleClick(float x, float y, const ViewportRenderContext& ctx) override;
    void onScroll(float delta, const ViewportRenderContext& ctx) override;
    
    void* getSelectedItem() const override { return m_selectedActor; }
    void clearSelection() override;
    const char* getName() const override { return "SceneRenderer"; }
    
    // Scene management
    void setScene(engine::Scene* scene) { m_scene = scene; }
    engine::Scene* getScene() const { return m_scene; }
    
    // SDL renderer for texture loading
    void setSDLRenderer(SDL_Renderer* renderer) { m_sdlRenderer = renderer; }
    
    // Actor operations
    void deleteSelectedActor();
    void duplicateSelectedActor();
    
    // Grid settings
    bool isGridVisible() const { return m_showGrid; }
    void setGridVisible(bool visible) { m_showGrid = visible; }
    int getGridSize() const { return m_gridSize; }
    void setGridSize(int size) { m_gridSize = size; }
    bool isSnapToGrid() const { return m_snapToGrid; }
    void setSnapToGrid(bool snap) { m_snapToGrid = snap; }
    
    // Debug visualization
    bool isPhysicsDebugVisible() const { return m_showPhysicsDebug; }
    void setPhysicsDebugVisible(bool visible) { m_showPhysicsDebug = visible; }

private:
    void renderBackground(ImDrawList* drawList, const ViewportRenderContext& ctx);
    void renderGrid(ImDrawList* drawList, const ViewportRenderContext& ctx);
    void renderActors(ImDrawList* drawList, const ViewportRenderContext& ctx);
    void renderActorSprite(ImDrawList* drawList, engine::ActorObjectExtended* actor, 
                           float worldX, float worldY, float zoom);
    void renderActorPlaceholder(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                float worldX, float worldY, float zoom);
    void renderActorSelection(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                              float worldX, float worldY, float zoom);
    void renderPhysicsDebug(ImDrawList* drawList, const ViewportRenderContext& ctx);
    void renderInfoOverlay(ImDrawList* drawList, const ViewportRenderContext& ctx);
    
    engine::ActorObjectExtended* findActorAt(float roomX, float roomY);
    
    EditorContext* m_context = nullptr;
    SelectionManager* m_selectionManager = nullptr;
    SDL_Renderer* m_sdlRenderer = nullptr;
    engine::Scene* m_scene = nullptr;
    
    // Selection/drag state
    engine::ActorObjectExtended* m_selectedActor = nullptr;
    engine::ActorObjectExtended* m_draggedActor = nullptr;
    float m_dragOffsetX = 0.0f;
    float m_dragOffsetY = 0.0f;
    
    // Grid settings
    bool m_showGrid = true;
    int m_gridSize = 32;
    bool m_snapToGrid = true;
    
    // Debug
    bool m_showPhysicsDebug = true;
    
    // Tools
    ToolManager m_toolManager;
    ToolContext m_toolContext;
};

} // namespace editor
