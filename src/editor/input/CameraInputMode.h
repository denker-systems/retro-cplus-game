/**
 * @file CameraInputMode.h
 * @brief Camera input mode with UE-style navigation
 */
#pragma once

#include "IInputMode.h"

#ifdef HAS_IMGUI

namespace editor {

class IViewportRenderer;
class EditorCamera3D;

/**
 * @brief Camera control input mode
 * 
 * Implements Unreal Engine-style viewport navigation:
 * - RMB + WASD/QE: Fly mode
 * - RMB + Mouse: Look around
 * - Alt + LMB: Orbit
 * - Alt + RMB: Dolly
 * - Alt + MMB / MMB: Pan
 * - Scroll: Zoom
 * - RMB + Scroll: Adjust speed
 * - F: Focus on selection
 */
class CameraInputMode : public IInputMode {
public:
    CameraInputMode() = default;
    ~CameraInputMode() override = default;
    
    const char* getName() const override { return "Camera"; }
    
    void onEnter() override;
    void onExit() override;
    
    bool processInput(const InputContext& ctx) override;
    
    bool wantCaptureMouse() const override { return m_isNavigating; }
    bool wantCaptureKeyboard() const override { return m_isFlyMode; }
    
    // Speed settings
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    float getMoveSpeed() const { return m_moveSpeed; }
    
    void setRotateSpeed(float speed) { m_rotateSpeed = speed; }
    float getRotateSpeed() const { return m_rotateSpeed; }

private:
    bool process2DCamera(const InputContext& ctx);
    bool process3DCamera(const InputContext& ctx);
    
    void handleFlyMode(const InputContext& ctx, EditorCamera3D* camera);
    void handleOrbit(const InputContext& ctx, EditorCamera3D* camera);
    void handlePan(const InputContext& ctx, EditorCamera3D* camera);
    void handleDolly(const InputContext& ctx, EditorCamera3D* camera);
    void handleScroll(const InputContext& ctx, EditorCamera3D* camera);
    
    // State
    bool m_isNavigating = false;
    bool m_isFlyMode = false;
    bool m_isOrbiting = false;
    bool m_isPanning = false;
    bool m_isDollying = false;
    
    // Settings
    float m_moveSpeed = 500.0f;
    float m_rotateSpeed = 0.3f;
    float m_panSpeed = 1.0f;
    float m_zoomSpeed = 0.1f;
};

} // namespace editor

#endif // HAS_IMGUI
