/**
 * @file CameraActor.h
 * @brief 3D Camera Actor for scene viewing
 * 
 * Similar to ACameraActor in Unreal Engine.
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace engine {

/**
 * @class CameraActor
 * @brief Actor with 3D camera functionality
 * 
 * Provides view and projection matrices for 3D rendering.
 * Can be used as game camera or cutscene camera.
 */
class CameraActor : public ActorObjectExtended {
public:
    CameraActor(const std::string& name = "CameraActor");
    virtual ~CameraActor() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // ========================================================================
    // CAMERA PROPERTIES
    // ========================================================================
    
    void setFOV(float fov) { m_fov = fov; }
    float getFOV() const { return m_fov; }
    
    void setAspectRatio(float ratio) { m_aspectRatio = ratio; }
    float getAspectRatio() const { return m_aspectRatio; }
    
    void setNearClip(float near) { m_nearClip = near; }
    float getNearClip() const { return m_nearClip; }
    
    void setFarClip(float far) { m_farClip = far; }
    float getFarClip() const { return m_farClip; }
    
    // ========================================================================
    // 3D POSITION AND ORIENTATION
    // ========================================================================
    
    void setPosition3D(const glm::vec3& pos);
    glm::vec3 getPosition3D() const { return m_position3D; }
    
    void setTarget(const glm::vec3& target);
    glm::vec3 getTarget() const { return m_target; }
    
    void lookAt(const glm::vec3& target);
    
    void setYaw(float yaw) { m_yaw = yaw; updateVectors(); }
    float getYaw() const { return m_yaw; }
    
    void setPitch(float pitch) { m_pitch = pitch; updateVectors(); }
    float getPitch() const { return m_pitch; }
    
    // ========================================================================
    // MATRICES
    // ========================================================================
    
    glm::mat4 getViewMatrix() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewProjection() const;
    
    // ========================================================================
    // CAMERA VECTORS
    // ========================================================================
    
    glm::vec3 getForward() const { return m_forward; }
    glm::vec3 getRight() const { return m_right; }
    glm::vec3 getUp() const { return m_up; }
    
    // ========================================================================
    // FOLLOW TARGET
    // ========================================================================
    
    void setFollowTarget(ActorObjectExtended* target) { m_followTarget = target; }
    ActorObjectExtended* getFollowTarget() const { return m_followTarget; }
    
    void setFollowOffset(const glm::vec3& offset) { m_followOffset = offset; }
    glm::vec3 getFollowOffset() const { return m_followOffset; }
    
    void setFollowSmoothing(float smoothing) { m_followSmoothing = smoothing; }
    float getFollowSmoothing() const { return m_followSmoothing; }

private:
    void updateVectors();
    
    // Camera properties
    float m_fov = 45.0f;
    float m_aspectRatio = 16.0f / 9.0f;
    float m_nearClip = 0.1f;
    float m_farClip = 1000.0f;
    
    // 3D position and orientation
    glm::vec3 m_position3D{0.0f, 5.0f, 10.0f};
    glm::vec3 m_target{0.0f, 0.0f, 0.0f};
    float m_yaw = 0.0f;
    float m_pitch = 0.0f;
    
    // Camera vectors
    glm::vec3 m_forward{0.0f, 0.0f, -1.0f};
    glm::vec3 m_right{1.0f, 0.0f, 0.0f};
    glm::vec3 m_up{0.0f, 1.0f, 0.0f};
    
    // Follow target
    ActorObjectExtended* m_followTarget = nullptr;
    glm::vec3 m_followOffset{0.0f, 5.0f, 10.0f};
    float m_followSmoothing = 0.1f;
};

} // namespace engine
