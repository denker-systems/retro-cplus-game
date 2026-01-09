/**
 * @file ViewportLevelRenderer.h
 * @brief Renderer for Level-level viewport (shows Scenes)
 */
#pragma once

#include "IViewportRenderer.h"

namespace engine {
    class Level;
    class Scene;
}

namespace editor {

/**
 * @brief Renders Level view showing all Scenes
 */
class ViewportLevelRenderer : public IViewportRenderer {
public:
    ViewportLevelRenderer() = default;
    ~ViewportLevelRenderer() override = default;
    
    void initialize(EditorContext* context, SelectionManager* selectionManager) override;
    void render(ImDrawList* drawList, const ViewportRenderContext& ctx) override;
    void update(float deltaTime) override;
    
    bool onMouseClick(float x, float y, const ViewportRenderContext& ctx) override;
    bool onMouseDrag(float x, float y, float deltaX, float deltaY, const ViewportRenderContext& ctx) override;
    void onMouseRelease(float x, float y, const ViewportRenderContext& ctx) override;
    bool onDoubleClick(float x, float y, const ViewportRenderContext& ctx) override;
    void onScroll(float delta, const ViewportRenderContext& ctx) override;
    
    void* getSelectedItem() const override { return m_selectedScene; }
    void clearSelection() override;
    const char* getName() const override { return "LevelRenderer"; }
    
    // Level management
    void setLevel(engine::Level* level) { m_level = level; }
    engine::Level* getLevel() const { return m_level; }
    
    // View mode: 0=Spatial, 1=Grid, 2=List
    int getViewMode() const { return m_viewMode; }
    void setViewMode(int mode) { m_viewMode = mode; }

private:
    void renderSpatialView(ImDrawList* drawList, const ViewportRenderContext& ctx);
    void renderGridView(ImDrawList* drawList, const ViewportRenderContext& ctx);
    
    engine::Scene* findSceneAt(float x, float y, const ViewportRenderContext& ctx);
    
    EditorContext* m_context = nullptr;
    SelectionManager* m_selectionManager = nullptr;
    engine::Level* m_level = nullptr;
    
    // View mode
    int m_viewMode = 0;  // 0=Spatial, 1=Grid, 2=List
    
    // Selection/drag state
    engine::Scene* m_selectedScene = nullptr;
    engine::Scene* m_draggedScene = nullptr;
    float m_dragStartX = 0.0f;
    float m_dragStartY = 0.0f;
    int m_dragStartGridX = 0;
    int m_dragStartGridY = 0;
    bool m_snapToGrid = true;
};

} // namespace editor
