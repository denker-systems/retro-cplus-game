/**
 * @file GizmoInputMode.h
 * @brief Transform gizmo input mode
 */
#pragma once

#include "IInputMode.h"

#ifdef HAS_IMGUI

namespace editor {

/**
 * @brief Gizmo transform type
 */
enum class GizmoType {
    Translate,
    Rotate,
    Scale
};

/**
 * @brief Transform space
 */
enum class TransformSpace {
    Local,
    World
};

/**
 * @brief Gizmo input mode
 * 
 * Handles transform gizmo controls:
 * - W: Translate mode
 * - E: Rotate mode
 * - R: Scale mode
 * - Space: Toggle Local/World space
 * - Ctrl held: Snap enabled
 */
class GizmoInputMode : public IInputMode {
public:
    GizmoInputMode() = default;
    ~GizmoInputMode() override = default;
    
    const char* getName() const override { return "Gizmo"; }
    
    void onEnter() override;
    void onExit() override;
    
    bool processInput(const InputContext& ctx) override;
    
    // Gizmo state
    GizmoType getGizmoType() const { return m_gizmoType; }
    void setGizmoType(GizmoType type) { m_gizmoType = type; }
    
    TransformSpace getTransformSpace() const { return m_transformSpace; }
    void setTransformSpace(TransformSpace space) { m_transformSpace = space; }
    
    bool isSnapping() const { return m_snapping; }
    
    // Snap settings
    float getTranslateSnap() const { return m_translateSnap; }
    void setTranslateSnap(float snap) { m_translateSnap = snap; }
    
    float getRotateSnap() const { return m_rotateSnap; }
    void setRotateSnap(float snap) { m_rotateSnap = snap; }
    
    float getScaleSnap() const { return m_scaleSnap; }
    void setScaleSnap(float snap) { m_scaleSnap = snap; }

private:
    void handleGizmoShortcuts(const InputContext& ctx);
    
    GizmoType m_gizmoType = GizmoType::Translate;
    TransformSpace m_transformSpace = TransformSpace::Local;
    bool m_snapping = false;
    
    // Snap values
    float m_translateSnap = 10.0f;  // pixels
    float m_rotateSnap = 15.0f;     // degrees
    float m_scaleSnap = 0.1f;       // scale units
};

} // namespace editor

#endif // HAS_IMGUI
