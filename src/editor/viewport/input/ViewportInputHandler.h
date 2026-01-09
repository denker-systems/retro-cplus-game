/**
 * @file ViewportInputHandler.h
 * @brief Handles input for viewport interactions
 */
#pragma once

#include "../ViewportTypes.h"

namespace editor {
namespace viewport {

/**
 * @class ViewportInputHandler
 * @brief Processes mouse and keyboard input for viewport
 */
class ViewportInputHandler {
public:
    ViewportInputHandler() = default;
    ~ViewportInputHandler() = default;
    
    /**
     * @brief Process input events
     * @param ctx Render context
     * @param hovered Whether viewport is hovered
     */
    void processInput(RenderContext& ctx, bool hovered);
    
    /**
     * @brief Check if double-click occurred
     */
    bool wasDoubleClicked() const { return m_doubleClicked; }
    
    /**
     * @brief Clear double-click flag
     */
    void clearDoubleClick() { m_doubleClicked = false; }

private:
    void handleZoom(RenderContext& ctx);
    void handlePan(RenderContext& ctx);
    void handleDoubleClick();
    
    bool m_doubleClicked = false;
};

} // namespace viewport
} // namespace editor
