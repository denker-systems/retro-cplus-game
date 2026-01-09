/**
 * @file World2DRenderer.h
 * @brief 2D renderer for World view (SECONDARY - inheritable base)
 */
#pragma once

#include "../../core/Base2DRenderer.h"

namespace engine {
    class World;
    class Level;
}

namespace editor {
namespace viewport {

/**
 * @class World2DRenderer
 * @brief Renders world in 2D (levels as cards) - Base for Level2DRenderer
 * 
 * This is the root of the 2D inheritance chain:
 * World2DRenderer → Level2DRenderer → Scene2DRenderer
 */
class World2DRenderer : public Base2DRenderer {
public:
    World2DRenderer() = default;
    virtual ~World2DRenderer() = default;
    
    void setWorld(engine::World* world) { m_world = world; }
    
    engine::Level* getSelectedLevel() const { return m_selectedLevel; }

protected:
    void render2DContent(ImDrawList* drawList, const RenderContext& ctx) override;
    virtual void renderWorldContent(ImDrawList* drawList, const RenderContext& ctx);
    void handle2DInput(const RenderContext& ctx, bool hovered) override;
    
    engine::World* m_world = nullptr;
    engine::Level* m_selectedLevel = nullptr;
    engine::Level* m_draggedLevel = nullptr;
    
    float m_dragStartX = 0.0f;
    float m_dragStartY = 0.0f;
    int m_dragStartGridX = 0;
    int m_dragStartGridY = 0;
};

} // namespace viewport
} // namespace editor
