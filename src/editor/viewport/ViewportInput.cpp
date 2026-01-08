/**
 * @file ViewportInput.cpp
 * @brief Unified input handling implementation
 */
#include "ViewportInput.h"
#include "ViewportRenderer.h"
#include "EditorCamera3D.h"

#ifdef HAS_IMGUI

#include <imgui.h>

namespace editor {

ViewportInput::ViewportInput() = default;

void ViewportInput::processInput(ViewportRenderer& renderer,
                                  const glm::vec2& viewportPos,
                                  const glm::vec2& viewportSize,
                                  bool isHovered,
                                  bool isFocused) {
    ImGuiIO& io = ImGui::GetIO();
    m_mousePos = glm::vec2(io.MousePos.x, io.MousePos.y);
    m_mouseDelta = m_mousePos - m_lastMousePos;
    
    m_clicked = false;
    m_doubleClicked = false;
    
    if (isHovered) {
        // Picking - update hovered object
        renderer.pickObjectAt(m_mousePos, viewportPos, viewportSize);
        
        // Click detection
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            int hovered = renderer.getHoveredIndex();
            if (hovered >= 0) {
                renderer.setSelectedIndex(hovered);
                m_clicked = true;
            }
        }
        
        // Double-click detection
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
            if (renderer.getSelectedIndex() >= 0) {
                m_doubleClicked = true;
            }
        }
        
        // Process camera input based on mode
        if (renderer.getRenderMode() == RenderMode::Mode2D) {
            process2DInput(renderer, viewportPos, viewportSize, isHovered);
        } else {
            process3DInput(renderer, viewportPos, viewportSize, isHovered);
        }
    } else {
        m_isPanning = false;
        m_isOrbiting = false;
    }
    
    m_lastMousePos = m_mousePos;
}

void ViewportInput::process2DInput(ViewportRenderer& renderer, const glm::vec2& viewportPos,
                                    const glm::vec2& viewportSize, bool isHovered) {
    ImGuiIO& io = ImGui::GetIO();
    
    // Middle mouse - pan
    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        glm::vec2 pan = renderer.getPan();
        pan += m_mouseDelta;
        renderer.setPan(pan);
    }
    
    // Scroll - zoom
    if (io.MouseWheel != 0.0f) {
        float zoom = renderer.getZoom();
        zoom *= (1.0f + io.MouseWheel * 0.1f);
        zoom = glm::clamp(zoom, 0.1f, 10.0f);
        renderer.setZoom(zoom);
    }
}

void ViewportInput::process3DInput(ViewportRenderer& renderer, const glm::vec2& viewportPos,
                                    const glm::vec2& viewportSize, bool isHovered) {
    ImGuiIO& io = ImGui::GetIO();
    EditorCamera3D* camera = renderer.getCamera3D();
    if (!camera) return;
    
    // Right mouse - orbit
    if (ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
        if (!m_isOrbiting) {
            m_isOrbiting = true;
        } else {
            camera->onMouseMove(m_mouseDelta.x, m_mouseDelta.y, true);
        }
    } else {
        m_isOrbiting = false;
    }
    
    // Middle mouse - pan
    if (ImGui::IsMouseDown(ImGuiMouseButton_Middle)) {
        if (!m_isPanning) {
            m_isPanning = true;
        } else {
            camera->onPan(m_mouseDelta.x, m_mouseDelta.y);
        }
    } else {
        m_isPanning = false;
    }
    
    // Scroll - zoom
    if (io.MouseWheel != 0.0f) {
        camera->onMouseScroll(io.MouseWheel);
    }
}

} // namespace editor

#endif // HAS_IMGUI
