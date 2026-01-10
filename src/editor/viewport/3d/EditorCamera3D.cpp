/**
 * @file EditorCamera3D.cpp
 * @brief 3D orbit camera implementation
 */
#include "EditorCamera3D.h"
#include <algorithm>
#include <iostream>

namespace editor {

EditorCamera3D::EditorCamera3D(float fov, float aspectRatio)
    : m_fov(fov)
    , m_aspectRatio(aspectRatio) {
    updateCameraVectors();
}

void EditorCamera3D::update(float /*deltaTime*/) {
    // Camera updates are event-driven, no continuous update needed
}

void EditorCamera3D::onMouseMove(float xOffset, float yOffset, int mode) {
    switch (mode) {
        case 0: onOrbit(xOffset, yOffset); break;
        case 1: onLookAround(xOffset, yOffset); break;
        case 2: onDolly(-yOffset * 0.1f); break;
    }
}

void EditorCamera3D::onLookAround(float xOffset, float yOffset) {
    // UE-style: RMB + mouse = look around (free rotation)
    m_yaw += xOffset * m_mouseSensitivity;
    m_pitch -= yOffset * m_mouseSensitivity;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    
    // In fly mode, camera looks in direction based on yaw/pitch
    // Forward direction: where camera is looking (negative Z in camera space)
    float yawRad = glm::radians(m_yaw);
    float pitchRad = glm::radians(m_pitch);
    
    // Calculate forward direction (inverted from orbit calculation)
    // In orbit: position is BEHIND target, so forward = target - position
    // Here we want to look in the direction of yaw/pitch
    m_forward.x = -cos(pitchRad) * sin(yawRad);
    m_forward.y = -sin(pitchRad);
    m_forward.z = -cos(pitchRad) * cos(yawRad);
    m_forward = glm::normalize(m_forward);
    
    // Update target to be in front of camera
    m_target = m_position + m_forward * m_distance;
    
    // Recalculate right and up
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
    
    std::cout << "[Camera] LookAround yaw:" << m_yaw << " pitch:" << m_pitch 
              << " fwd:(" << m_forward.x << "," << m_forward.y << "," << m_forward.z << ")" << std::endl;
}

void EditorCamera3D::onOrbit(float xOffset, float yOffset) {
    // UE-style: Alt + LMB = orbit around target
    m_yaw += xOffset * m_mouseSensitivity;
    m_pitch -= yOffset * m_mouseSensitivity;
    m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);
    updateCameraVectors();
}

void EditorCamera3D::onDolly(float delta) {
    // UE-style: Alt + RMB drag or LMB + RMB = dolly
    m_distance -= delta * m_zoomSpeed * m_distance * 0.5f;
    m_distance = std::clamp(m_distance, m_minDistance, m_maxDistance);
    updateCameraVectors();
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

void EditorCamera3D::onKeyboardMove(float forward, float right, float up, float deltaTime) {
    // UE-style: RMB + WASD/QE = fly mode
    float velocity = getMoveSpeed() * deltaTime;
    
    // Move camera position directly (fly mode)
    glm::vec3 movement(0.0f);
    movement += m_forward * forward * velocity;  // W/S
    movement += m_right * right * velocity;      // A/D
    movement += glm::vec3(0.0f, 1.0f, 0.0f) * up * velocity;  // Q/E (world up)
    
    // Move both camera and target together
    m_position += movement;
    m_target += movement;
    
    std::cout << "[Camera] KeyboardMove speed:" << getMoveSpeed() 
              << " pos:(" << m_position.x << "," << m_position.y << "," << m_position.z << ")" << std::endl;
}

void EditorCamera3D::adjustMoveSpeed(float delta) {
    // UE-style: RMB + scroll = adjust fly speed
    m_moveSpeedMultiplier *= (1.0f + delta * 0.1f);
    m_moveSpeedMultiplier = std::clamp(m_moveSpeedMultiplier, m_minSpeedMultiplier, m_maxSpeedMultiplier);
}

glm::mat4 EditorCamera3D::getViewMatrix() const {
    return glm::lookAt(m_position, m_target, m_up);
}

glm::mat4 EditorCamera3D::getProjectionMatrix() const {
    if (m_projectionMode == ProjectionMode::Orthographic) {
        // Orthographic projection for 2D mode
        float halfHeight = m_orthoSize * 0.5f;
        float halfWidth = halfHeight * m_aspectRatio;
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, m_nearPlane, m_farPlane);
    } else {
        // Perspective projection for 3D mode
        return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearPlane, m_farPlane);
    }
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
