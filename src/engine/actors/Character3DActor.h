/**
 * @file Character3DActor.h
 * @brief 3D Character actor with PhysX character controller
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include <glm/glm.hpp>
#include <memory>

namespace engine {

class CharacterController3DComponent;

namespace physics {
    class PhysicsWorld3D;
}

/**
 * @brief Base class for 3D characters (Player, NPCs)
 * 
 * Features:
 * - PhysX character controller for movement
 * - Capsule collision
 * - Gravity and jumping
 * - Ground detection
 * 
 * Similar to ACharacter in Unreal Engine.
 */
class Character3DActor : public ActorObjectExtended {
public:
    Character3DActor(const std::string& name = "Character");
    virtual ~Character3DActor() = default;
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /** @brief Initialize physics controller */
    bool initializeController(physics::PhysicsWorld3D* world);
    
    /** @brief Shutdown physics controller */
    void shutdownController();
    
    /** @brief Check if controller is initialized */
    bool isControllerInitialized() const;
    
    // ========================================================================
    // MOVEMENT
    // ========================================================================
    
    /** @brief Set movement input (normalized direction) */
    void setMoveInput(const glm::vec3& input);
    
    /** @brief Get current velocity */
    glm::vec3 getVelocity() const;
    
    /** @brief Jump */
    void jump();
    
    /** @brief Check if on ground */
    bool isGrounded() const;
    
    // ========================================================================
    // CONFIGURATION
    // ========================================================================
    
    void setMoveSpeed(float speed);
    float getMoveSpeed() const;
    
    void setJumpVelocity(float velocity);
    float getJumpVelocity() const;
    
    // ========================================================================
    // POSITION (3D)
    // ========================================================================
    
    /** @brief Get position in 3D world coordinates */
    glm::vec3 getPosition3D() const;
    
    /** @brief Set position in 3D world coordinates */
    void setPosition3D(const glm::vec3& position);
    
    // ========================================================================
    // ROTATION
    // ========================================================================
    
    /** @brief Get rotation (yaw in degrees) */
    float getYaw() const { return m_yaw; }
    
    /** @brief Set rotation (yaw in degrees) */
    void setYaw(float yaw) { m_yaw = yaw; }
    
    /** @brief Add to rotation (for mouse look) */
    void addYaw(float delta) { m_yaw += delta; }
    
    /** @brief Get forward direction based on yaw */
    glm::vec3 getForwardDirection() const;
    
    /** @brief Get right direction based on yaw */
    glm::vec3 getRightDirection() const;
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    void update(float deltaTime) override;
    
protected:
    CharacterController3DComponent* m_controller = nullptr;
    float m_yaw = 0.0f;  // Rotation in degrees
};

/**
 * @brief Player character for 3D gameplay
 */
class Player3DActor : public Character3DActor {
public:
    Player3DActor();
    virtual ~Player3DActor() = default;
    
    void update(float deltaTime) override;
    
    /** @brief Handle keyboard input for movement */
    void handleInput(float deltaTime);
    
    /** @brief Handle mouse input for rotation */
    void handleMouseLook(float deltaX, float deltaY);
    
    /** @brief Enable/disable input handling */
    void setInputEnabled(bool enabled) { m_inputEnabled = enabled; }
    bool isInputEnabled() const { return m_inputEnabled; }
    
private:
    bool m_inputEnabled = true;
    float m_mouseSensitivity = 0.1f;
};

} // namespace engine
