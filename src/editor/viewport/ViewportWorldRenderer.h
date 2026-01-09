/**
 * @file ViewportWorldRenderer.h
 * @brief Renderer for World-level viewport (shows Levels)
 */
#pragma once

#include "IViewportRenderer.h"
#include <vector>

namespace engine {
    class World;
    class Level;
}

namespace editor {

/**
 * @brief Renders World view showing all Levels
 */
class ViewportWorldRenderer : public IViewportRenderer {
public:
    ViewportWorldRenderer() = default;
    ~ViewportWorldRenderer() override = default;
    
    void initialize(EditorContext* context, SelectionManager* selectionManager) override;
    void render(ImDrawList* drawList, const ViewportRenderContext& ctx) override;
    void update(float deltaTime) override;
    
    bool onMouseClick(float x, float y, const ViewportRenderContext& ctx) override;
    bool onMouseDrag(float x, float y, float deltaX, float deltaY, const ViewportRenderContext& ctx) override;
    void onMouseRelease(float x, float y, const ViewportRenderContext& ctx) override;
    bool onDoubleClick(float x, float y, const ViewportRenderContext& ctx) override;
    void onScroll(float delta, const ViewportRenderContext& ctx) override;
    
    void* getSelectedItem() const override { return m_selectedLevel; }
    void clearSelection() override;
    const char* getName() const override { return "WorldRenderer"; }
    
    // World management
    void setWorld(engine::World* world) { m_world = world; }
    engine::World* getWorld() const { return m_world; }
    
    // View mode: 0=Spatial, 1=Grid
    int getViewMode() const { return m_viewMode; }
    void setViewMode(int mode) { m_viewMode = mode; }

private:
    void renderSpatialView(ImDrawList* drawList, const ViewportRenderContext& ctx);
    void renderGridView(ImDrawList* drawList, const ViewportRenderContext& ctx);
    void renderLevelCard(ImDrawList* drawList, engine::Level* level, float x, float y, float w, float h);
    
    engine::Level* findLevelAt(float x, float y, const ViewportRenderContext& ctx);
    
    EditorContext* m_context = nullptr;
    SelectionManager* m_selectionManager = nullptr;
    engine::World* m_world = nullptr;
    
    // View mode
    int m_viewMode = 0;  // 0=Spatial, 1=Grid
    
    // Selection/drag state
    engine::Level* m_selectedLevel = nullptr;
    engine::Level* m_draggedLevel = nullptr;
    float m_dragStartX = 0.0f;
    float m_dragStartY = 0.0f;
    int m_dragStartGridX = 0;
    int m_dragStartGridY = 0;
};

} // namespace editor
