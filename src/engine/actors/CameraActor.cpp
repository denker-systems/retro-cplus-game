/**
 * @file CameraActor.cpp
 * @brief 3D Camera Actor implementation
 */

#include "CameraActor.h"
#include <cmath>

namespace engine {

CameraActor::CameraActor(const std::string& name)
    : ActorObjectExtended(name)
{
    updateVectors();
}

void CameraActor::update(float deltaTime) {
    ActorObjectExtended::update(deltaTime);
    
    if (m_followTarget) {
        glm::vec3 targetPos = glm::vec3(
            m_followTarget->getX() / 100.0f,
            m_followTarget->getZ() / 100.0f,
            m_followTarget->getY() / 100.0f
        );
        
        glm::vec3 desiredPos = targetPos + m_followOffset;
        
        m_position3D = glm::mix(m_position3D, desiredPos, m_followSmoothing);
        m_target = targetPos;
        
        updateVectors();
    }
}

void CameraActor::render(SDL_Renderer* renderer) {
    ActorObjectExtended::render(renderer);
}

void CameraActor::setPosition3D(const glm::vec3& pos) {
    m_position3D = pos;
    setPosition(pos.x * 100.0f, pos.z * 100.0f);
    setZ(pos.y * 100.0f);
    updateVectors();
}

void CameraActor::setTarget(const glm::vec3& target) {
    m_target = target;
    updateVectors();
}

void CameraActor::lookAt(const glm::vec3& target) {
    m_target = target;
    
    glm::vec3 direction = glm::normalize(target - m_position3D);
    m_pitch = glm::degrees(asin(direction.y));
    m_yaw = glm::degrees(atan2(direction.x, -direction.z));
    
    updateVectors();
}

glm::mat4 CameraActor::getViewMatrix() const {
    return glm::lookAt(m_position3D, m_target, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 CameraActor::getProjectionMatrix() const {
    return glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
}

glm::mat4 CameraActor::getViewProjection() const {
    return getProjectionMatrix() * getViewMatrix();
}

void CameraActor::updateVectors() {
    glm::vec3 direction = glm::normalize(m_target - m_position3D);
    m_forward = direction;
    m_right = glm::normalize(glm::cross(m_forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    m_up = glm::normalize(glm::cross(m_right, m_forward));
}

} // namespace engine
