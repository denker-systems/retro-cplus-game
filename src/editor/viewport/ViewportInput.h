/**
 * @file ViewportInput.h
 * @brief Unified input handling for viewport (2D and 3D)
 */
#pragma once

#ifdef HAS_IMGUI

#include <glm/glm.hpp>

namespace editor {

class ViewportRenderer;
class EditorCamera3D;

/**
 * @class ViewportInput
 * @brief Handles all viewport input (camera control, selection, etc.)
 * 
 * Works identically at all hierarchy levels.
 */
class ViewportInput {
public:
    ViewportInput();
    ~ViewportInput() = default;
    
    /**
     * @brief Process input for the viewport
     * @param renderer The viewport renderer
     * @param viewportPos Top-left of viewport
     * @param viewportSize Size of viewport
     * @param isHovered Is mouse over viewport
     * @param isFocused Is viewport focused
     */
    void processInput(ViewportRenderer& renderer,
                      const glm::vec2& viewportPos,
                      const glm::vec2& viewportSize,
                      bool isHovered,
                      bool isFocused);
    
    // State queries
    bool wasClicked() const { return m_clicked; }
    bool wasDoubleClicked() const { return m_doubleClicked; }
    void clearClickState() { m_clicked = false; m_doubleClicked = false; }
    
    glm::vec2 getMousePos() const { return m_mousePos; }
    glm::vec2 getMouseDelta() const { return m_mouseDelta; }

private:
    // 2D input
    void process2DInput(ViewportRenderer& renderer, const glm::vec2& viewportPos, 
                        const glm::vec2& viewportSize, bool isHovered);
    
    // 3D input
    void process3DInput(ViewportRenderer& renderer, const glm::vec2& viewportPos,
                        const glm::vec2& viewportSize, bool isHovered);
    
    // State
    glm::vec2 m_mousePos{0.0f, 0.0f};
    glm::vec2 m_lastMousePos{0.0f, 0.0f};
    glm::vec2 m_mouseDelta{0.0f, 0.0f};
    
    bool m_isPanning = false;
    bool m_isOrbiting = false;
    bool m_clicked = false;
    bool m_doubleClicked = false;
};

} // namespace editor

#endif // HAS_IMGUI
