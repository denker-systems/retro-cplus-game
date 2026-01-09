/**
 * @file CameraInputMode.cpp
 * @brief Camera input mode implementation
 */
#include "CameraInputMode.h"
#include "EditorInputController.h"
#include "editor/viewport/EditorCamera3D.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#include <algorithm>

namespace editor {

void CameraInputMode::onEnter() {
    m_isNavigating = false;
    m_isFlyMode = false;
    m_isOrbiting = false;
    m_isPanning = false;
    m_isDollying = false;
}

void CameraInputMode::onExit() {
    m_isNavigating = false;
    m_isFlyMode = false;
}

bool CameraInputMode::processInput(const InputContext& ctx) {
    if (!ctx.isHovered && !m_isNavigating) {
        return false;
    }
    
    // 2D camera is handled by ViewportPanel directly
    // This mode only handles 3D camera navigation
    return process3DCamera(ctx);
}

bool CameraInputMode::process2DCamera(const InputContext& ctx) {
    // 2D pan/zoom is handled directly by ViewportPanel
    return false;
}

bool CameraInputMode::process3DCamera(const InputContext& ctx) {
    // 3D camera support - currently disabled until camera is properly integrated
    // TODO: Re-enable when ViewportPanel exposes EditorCamera3D
    EditorCamera3D* camera = nullptr;
    if (!camera) return false;
    
    bool consumed = false;
    bool rightDown = ctx.mouseButtons[1];   // Right
    bool middleDown = ctx.mouseButtons[2];  // Middle
    bool leftDown = ctx.mouseButtons[0];    // Left
    
    // Track navigation state
    bool wasNavigating = m_isNavigating;
    m_isNavigating = false;
    
    // UE-style controls:
    
    // 1. RMB + WASD/QE = Fly mode
    if (rightDown && !ctx.alt) {
        m_isFlyMode = true;
        m_isNavigating = true;
        handleFlyMode(ctx, camera);
        consumed = true;
    } else {
        m_isFlyMode = false;
    }
    
    // 2. Alt + LMB = Orbit
    if (ctx.alt && leftDown) {
        if (!m_isOrbiting) {
            m_isOrbiting = true;
        }
        m_isNavigating = true;
        handleOrbit(ctx, camera);
        consumed = true;
    } else {
        m_isOrbiting = false;
    }
    
    // 3. Alt + RMB = Dolly
    if (ctx.alt && rightDown) {
        if (!m_isDollying) {
            m_isDollying = true;
        }
        m_isNavigating = true;
        handleDolly(ctx, camera);
        consumed = true;
    } else {
        m_isDollying = false;
    }
    
    // 4. Alt + MMB or just MMB = Pan
    if (middleDown) {
        if (!m_isPanning) {
            m_isPanning = true;
        }
        m_isNavigating = true;
        handlePan(ctx, camera);
        consumed = true;
    } else {
        m_isPanning = false;
    }
    
    // 5. Scroll = Zoom (when hovered)
    if (ctx.mouseWheel != 0.0f && ctx.isHovered) {
        handleScroll(ctx, camera);
        consumed = true;
    }
    
    // 6. F = Focus on selection
    if (ImGui::IsKeyPressed(ImGuiKey_F) && ctx.isFocused) {
        camera->focusOn(glm::vec3(0.0f));  // TODO: Focus on actual selection
        consumed = true;
    }
    
    return consumed;
}

void CameraInputMode::handleFlyMode(const InputContext& ctx, EditorCamera3D* camera) {
    // Mouse look
    if (ctx.mouseDelta.x != 0.0f || ctx.mouseDelta.y != 0.0f) {
        camera->onMouseMove(ctx.mouseDelta.x, ctx.mouseDelta.y, true);
    }
    
    // WASD movement
    float forward = 0.0f, right = 0.0f, up = 0.0f;
    if (ImGui::IsKeyDown(ImGuiKey_W)) forward += 1.0f;  // Forward
    if (ImGui::IsKeyDown(ImGuiKey_S)) forward -= 1.0f;  // Back
    if (ImGui::IsKeyDown(ImGuiKey_A)) right -= 1.0f;    // Left
    if (ImGui::IsKeyDown(ImGuiKey_D)) right += 1.0f;    // Right
    if (ImGui::IsKeyDown(ImGuiKey_E)) up += 1.0f;       // Up
    if (ImGui::IsKeyDown(ImGuiKey_Q)) up -= 1.0f;       // Down
    
    if (forward != 0.0f || right != 0.0f || up != 0.0f) {
        camera->onKeyboardMove(forward, right, up, ctx.deltaTime);
    }
    
    // RMB + Scroll = Adjust speed
    if (ctx.mouseWheel != 0.0f) {
        m_moveSpeed *= (1.0f + ctx.mouseWheel * 0.1f);
        m_moveSpeed = glm::clamp(m_moveSpeed, 50.0f, 5000.0f);
    }
}

void CameraInputMode::handleOrbit(const InputContext& ctx, EditorCamera3D* camera) {
    if (ctx.mouseDelta.x != 0.0f || ctx.mouseDelta.y != 0.0f) {
        camera->onMouseMove(ctx.mouseDelta.x, ctx.mouseDelta.y, true);
    }
}

void CameraInputMode::handlePan(const InputContext& ctx, EditorCamera3D* camera) {
    if (ctx.mouseDelta.x != 0.0f || ctx.mouseDelta.y != 0.0f) {
        camera->onPan(ctx.mouseDelta.x * m_panSpeed, ctx.mouseDelta.y * m_panSpeed);
    }
}

void CameraInputMode::handleDolly(const InputContext& ctx, EditorCamera3D* camera) {
    // Vertical mouse movement = dolly in/out
    if (ctx.mouseDelta.y != 0.0f) {
        camera->onMouseScroll(-ctx.mouseDelta.y * 0.05f);
    }
}

void CameraInputMode::handleScroll(const InputContext& ctx, EditorCamera3D* camera) {
    camera->onMouseScroll(ctx.mouseWheel);
}

} // namespace editor

#endif // HAS_IMGUI
