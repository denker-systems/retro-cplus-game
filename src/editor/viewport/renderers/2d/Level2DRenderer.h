/**
 * @file Level2DRenderer.h
 * @brief 2D renderer for Level view - inherits from World2DRenderer
 */
#pragma once

#include "World2DRenderer.h"

namespace engine {
    class Level;
    class Scene;
}

namespace editor {
namespace viewport {

/**
 * @class Level2DRenderer
 * @brief Renders level in 2D (scenes as cards) - Inherits World functionality
 * 
 * Inheritance chain: World2DRenderer → Level2DRenderer → Scene2DRenderer
 */
class Level2DRenderer : public World2DRenderer {
public:
    Level2DRenderer() = default;
    ~Level2DRenderer() override = default;
    
    void setLevel(engine::Level* level) { m_level = level; }
    
    engine::Scene* getSelectedScene() const { return m_selectedScene; }

protected:
    void render2DContent(ImDrawList* drawList, const RenderContext& ctx) override;
    virtual void renderLevelContent(ImDrawList* drawList, const RenderContext& ctx);
    void handle2DInput(const RenderContext& ctx, bool hovered) override;
    
    engine::Level* m_level = nullptr;
    engine::Scene* m_selectedScene = nullptr;
    engine::Scene* m_draggedScene = nullptr;
    
    float m_sceneDragStartX = 0.0f;
    float m_sceneDragStartY = 0.0f;
    int m_sceneDragStartGridX = 0;
    int m_sceneDragStartGridY = 0;
};

} // namespace viewport
} // namespace editor
