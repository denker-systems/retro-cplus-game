/**
 * @file PlayerConfigActor.h
 * @brief Player configuration actor with global settings and camera (2D gameplay)
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include <glm/glm.hpp>
#include <memory>

namespace engine {

class CameraComponent;

/**
 * @brief Player configuration actor for 2D gameplay
 * 
 * This is a placeable actor in the scene that defines global player settings.
 * Contains CameraComponent for camera configuration.
 * 
 * Usage:
 * - Place one PlayerConfigActor per scene
 * - Configure camera settings (zoom, follow smoothing)
 * - Configure player settings (move speed, etc.)
 * - PlayerStart defines spawn position
 */
class PlayerConfigActor : public ActorObjectExtended {
public:
    PlayerConfigActor(const std::string& name = "PlayerConfig");
    virtual ~PlayerConfigActor() = default;
    
    // ========================================================================
    // PLAYER SETTINGS
    // ========================================================================
    
    /** @brief Set player move speed */
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    float getMoveSpeed() const { return m_moveSpeed; }
    
    /** @brief Set player run speed multiplier */
    void setRunSpeedMultiplier(float multiplier) { m_runSpeedMultiplier = multiplier; }
    float getRunSpeedMultiplier() const { return m_runSpeedMultiplier; }
    
    // ========================================================================
    // CAMERA ACCESS
    // ========================================================================
    
    /** @brief Get camera component */
    CameraComponent* getCamera() const { return m_camera; }
    
    // ========================================================================
    // CAMERA OFFSET (for play mode)
    // ========================================================================
    
    /** @brief Set camera offset from player (X, Y, Z) */
    void setCameraOffset(const glm::vec3& offset) { m_cameraOffset = offset; }
    glm::vec3 getCameraOffset() const { return m_cameraOffset; }
    
    /** @brief Set camera follow speed */
    void setCameraFollowSpeed(float speed) { m_cameraFollowSpeed = speed; }
    float getCameraFollowSpeed() const { return m_cameraFollowSpeed; }
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    void update(float deltaTime) override;
    
private:
    CameraComponent* m_camera = nullptr;
    
    // Player settings
    float m_moveSpeed = 200.0f;  // pixels/second
    float m_runSpeedMultiplier = 1.5f;
    
    // Camera settings (for play mode)
    glm::vec3 m_cameraOffset{0.0f, 3.0f, 5.0f};  // Default: behind and above player
    float m_cameraFollowSpeed = 5.0f;
};

} // namespace engine
