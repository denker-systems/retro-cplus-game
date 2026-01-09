/**
 * @file Base2DRenderer.h
 * @brief Base class for 2D viewport rendering (SECONDARY)
 */
#pragma once

#include "BaseViewRenderer.h"
#include <glm/glm.hpp>

namespace editor {
namespace viewport {

/**
 * @class Base2DRenderer
 * @brief Base for 2D rendering (SECONDARY rendering mode)
 * 
 * 2D is the fallback/secondary rendering mode.
 * Provides common 2D rendering utilities (zoom, pan, grid).
 */
class Base2DRenderer : public BaseViewRenderer {
public:
    Base2DRenderer() = default;
    virtual ~Base2DRenderer() = default;
    
    void render(ImDrawList* drawList, const RenderContext& ctx) override;

protected:
    /**
     * @brief Render 2D content (implemented by derived classes)
     */
    virtual void render2DContent(ImDrawList* drawList, const RenderContext& ctx) = 0;
    
    /**
     * @brief Handle 2D input (zoom, pan, selection)
     */
    virtual void handle2DInput(const RenderContext& ctx, bool hovered);
    
    /**
     * @brief Draw 2D grid
     */
    void drawGrid(const glm::vec2& canvasPos, const glm::vec2& canvasSize, 
                  float gridSize, float zoom, float panX, float panY);
    
    // Shared 2D state
    float m_zoom = 1.0f;
    float m_panX = 0.0f;
    float m_panY = 0.0f;
};

} // namespace viewport
} // namespace editor
