/**
 * @file EditorCamera3D.h
 * @brief 3D orbit camera for editor viewport
 */
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace editor {

/**
 * @class EditorCamera3D
 * @brief Orbit camera for 3D scene editing
 * 
 * Provides intuitive camera controls:
 * - Left mouse drag: Orbit around target
 * - Middle mouse drag: Pan
 * - Scroll wheel: Zoom
 * - Right mouse drag: Rotate view
 */
class EditorCamera3D {
public:
    EditorCamera3D(float fov = 45.0f, float aspectRatio = 16.0f / 9.0f);
    
    /**
     * @brief Update camera (called each frame)
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Handle mouse movement for camera rotation
     * @param xOffset Mouse X delta
     * @param yOffset Mouse Y delta
     * @param orbiting If true, orbit around target; if false, rotate in place
     */
    void onMouseMove(float xOffset, float yOffset, bool orbiting = true);
    
    /**
     * @brief Handle mouse scroll for zoom
     * @param yOffset Scroll amount
     */
    void onMouseScroll(float yOffset);
    
    /**
     * @brief Handle panning (middle mouse drag)
     * @param xOffset Mouse X delta
     * @param yOffset Mouse Y delta
     */
    void onPan(float xOffset, float yOffset);
    
    /**
     * @brief Get the view matrix
     */
    glm::mat4 getViewMatrix() const;
    
    /**
     * @brief Get the projection matrix
     */
    glm::mat4 getProjectionMatrix() const;
    
    /**
     * @brief Get combined view-projection matrix
     */
    glm::mat4 getViewProjection() const;
    
    /**
     * @brief Get camera position in world space
     */
    glm::vec3 getPosition() const { return m_position; }
    
    /**
     * @brief Set camera position
     */
    void setPosition(const glm::vec3& pos) { m_position = pos; updateCameraVectors(); }
    
    /**
     * @brief Get camera target (look-at point)
     */
    glm::vec3 getTarget() const { return m_target; }
    
    /**
     * @brief Set camera target
     */
    void setTarget(const glm::vec3& target) { m_target = target; updateCameraVectors(); }
    
    /**
     * @brief Set aspect ratio (call when viewport resizes)
     */
    void setAspectRatio(float ratio) { m_aspectRatio = ratio; }
    
    /**
     * @brief Set field of view
     */
    void setFOV(float fov) { m_fov = fov; }
    
    /**
     * @brief Focus on a point with specific distance
     */
    void focusOn(const glm::vec3& target, float distance = 10.0f);
    
    /**
     * @brief Reset camera to default position
     */
    void reset();

    // Camera settings
    float m_mouseSensitivity = 0.3f;
    float m_zoomSpeed = 1.0f;
    float m_panSpeed = 0.01f;
    float m_minDistance = 0.5f;
    float m_maxDistance = 500.0f;

private:
    // Position and orientation
    glm::vec3 m_position{0.0f, 5.0f, 10.0f};
    glm::vec3 m_target{0.0f, 0.0f, 0.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};
    glm::vec3 m_forward{0.0f, 0.0f, -1.0f};
    
    // Orbit parameters
    float m_distance = 10.0f;
    float m_yaw = 0.0f;      // Horizontal angle (degrees)
    float m_pitch = 30.0f;   // Vertical angle (degrees)
    
    // Projection parameters
    float m_fov = 45.0f;
    float m_aspectRatio = 16.0f / 9.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 1000.0f;
    
    /**
     * @brief Recalculate camera vectors from yaw/pitch/distance
     */
    void updateCameraVectors();
};

} // namespace editor
