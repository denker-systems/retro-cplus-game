/**
 * @file TransformGizmo3D.h
 * @brief 3D transform gizmo using ImGuizmo
 */
#pragma once

#include <glm/glm.hpp>

namespace engine {
    class ActorObjectExtended;
}

namespace editor {

class EditorCamera3D;

/**
 * @brief Gizmo operation type
 */
enum class GizmoOperation {
    Translate,
    Rotate,
    Scale
};

/**
 * @brief Gizmo coordinate space
 */
enum class GizmoSpace {
    Local,
    World
};

/**
 * @class TransformGizmo3D
 * @brief 3D transform gizmo for actor manipulation
 * 
 * Uses ImGuizmo for rendering transform gizmos that allow
 * intuitive 3D manipulation of actors.
 */
class TransformGizmo3D {
public:
    TransformGizmo3D() = default;
    
    /**
     * @brief Render and handle gizmo for selected actor
     * @param camera The 3D camera
     * @param actor The actor to manipulate
     * @param viewportPos Top-left position of viewport
     * @param viewportSize Size of viewport
     * @return true if gizmo was manipulated this frame
     */
    bool render(EditorCamera3D* camera, engine::ActorObjectExtended* actor,
                const glm::vec2& viewportPos, const glm::vec2& viewportSize);
    
    /**
     * @brief Set gizmo operation type
     */
    void setOperation(GizmoOperation op) { m_operation = op; }
    GizmoOperation getOperation() const { return m_operation; }
    
    /**
     * @brief Set gizmo coordinate space
     */
    void setSpace(GizmoSpace space) { m_space = space; }
    GizmoSpace getSpace() const { return m_space; }
    
    /**
     * @brief Set snap values for each operation
     */
    void setTranslateSnap(float snap) { m_translateSnap = snap; }
    void setRotateSnap(float snap) { m_rotateSnap = snap; }
    void setScaleSnap(float snap) { m_scaleSnap = snap; }
    
    /**
     * @brief Enable/disable snapping
     */
    void setSnapEnabled(bool enabled) { m_snapEnabled = enabled; }
    bool isSnapEnabled() const { return m_snapEnabled; }
    
    /**
     * @brief Check if gizmo is being used
     */
    bool isUsing() const { return m_isUsing; }
    
    /**
     * @brief Render operation selector UI
     */
    void renderUI();

private:
    GizmoOperation m_operation = GizmoOperation::Translate;
    GizmoSpace m_space = GizmoSpace::World;
    
    float m_translateSnap = 1.0f;
    float m_rotateSnap = 15.0f;
    float m_scaleSnap = 0.1f;
    bool m_snapEnabled = false;
    bool m_isUsing = false;
};

} // namespace editor
