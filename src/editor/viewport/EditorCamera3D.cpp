/**
 * @file EditorCamera3D.cpp
 * @brief 3D orbit camera implementation
 */
#include "EditorCamera3D.h"
#include <algorithm>

namespace editor {

EditorCamera3D::EditorCamera3D(float fov, float aspectRatio)
    : m_fov(fov)
    , m_aspectRatio(aspectRatio) {
    updateCameraVectors();
}

void EditorCamera3D::update(float /*deltaTime*/) {
    // Camera updates are event-driven, no continuous update needed
}

void EditorCamera3D::onMouseMove(float xOffset, float yOffset, bool orbiting) {
    if (orbiting) {
        // Orbit around target
        m_yaw += xOffset * m_mouseSensitivity;
        m_pitch -= yOffset * m_mouseSensitivity;
        
        // Clamp pitch to avoid gimbal lock
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
        
        updateCameraVectors();
    }
}

void EditorCamera3D::onMouseScroll(float yOffset) {
    m_distance -= yOffset * m_zoomSpeed;
    m_distance = std::clamp(m_distance, m_minDistance, m_maxDistance);
    updateCameraVectors();
}

void EditorCamera3D::onPan(float xOffset, float yOffset) {
    // Pan in screen space
    glm::vec3 panOffset = m_right * (-xOffset * m_panSpeed * m_distance) +
                          m_up * (yOffset * m_panSpeed * m_distance);
    m_target += panOffset;
    updateCameraVectors();
}

glm::mat4 EditorCamera3D::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 EditorCamera3D::getProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
}

glm::mat4 EditorCamera3D::getViewProjection() const {
    return getProjectionMatrix() * getViewMatrix();
}

void EditorCamera3D::focusOn(const glm::vec3& target, float distance) {
    m_target = target;
    m_distance = distance;
    updateCameraVectors();
}

void EditorCamera3D::reset() {
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_distance = 10.0f;
    m_yaw = 0.0f;
    m_pitch = 30.0f;
    updateCameraVectors();
}

void EditorCamera3D::updateCameraVectors() {
    // Calculate position from spherical coordinates
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    
    // Position on sphere around target
    m_position.x = m_target.x + m_distance * cos(pitchRad) * sin(yawRad);
    m_position.y = m_target.y + m_distance * sin(pitchRad);
    m_position.z = m_target.z + m_distance * cos(pitchRad) * cos(yawRad);
    
    // Calculate camera vectors
    m_forward = glm::normalize(m_target - m_position);
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

} // namespace editor
