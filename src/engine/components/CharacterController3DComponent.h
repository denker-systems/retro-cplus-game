/**
 * @file CharacterController3DComponent.h
 * @brief PhysX-based character controller for 3D movement
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <glm/glm.hpp>

// Forward declarations
namespace physx {
    class PxController;
    class PxControllerManager;
}

namespace engine {

namespace physics {
    class PhysicsWorld3D;
}

/**
 * @brief Character controller using PhysX CCT (Character Controller Toolkit)
 * 
 * Provides smooth character movement with:
 * - Capsule collision shape
 * - Ground detection
 * - Slope handling
 * - Step climbing
 * - Gravity
 * - Jumping
 * 
 * Similar to UCharacterMovementComponent in Unreal Engine.
 */
class CharacterController3DComponent : public ActorComponent {
public:
    CharacterController3DComponent();
    virtual ~CharacterController3DComponent();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /** @brief Base initialize (does nothing - use initializePhysics instead) */
    void initialize() override {}
    
    /** @brief Initialize the controller with PhysX */
    bool initializePhysics(physics::PhysicsWorld3D* world, const glm::vec3& position);
    
    /** @brief Cleanup PhysX resources */
    void shutdown();
    
    /** @brief Check if initialized */
    bool isInitialized() const { return m_controller != nullptr; }
    
    // ========================================================================
    // MOVEMENT
    // ========================================================================
    
    /** @brief Move the character by displacement vector */
    void move(const glm::vec3& displacement, float deltaTime);
    
    /** @brief Apply jump impulse */
    void jump();
    
    /** @brief Set movement input direction (normalized) */
    void setMoveInput(const glm::vec3& input) { m_moveInput = input; }
    
    /** @brief Get current velocity */
    glm::vec3 getVelocity() const { return m_velocity; }
    
    // ========================================================================
    // GROUND DETECTION
    // ========================================================================
    
    /** @brief Check if character is on ground */
    bool isGrounded() const { return m_isGrounded; }
    
    /** @brief Check if character is falling */
    bool isFalling() const { return !m_isGrounded && m_velocity.y < 0; }
    
    // ========================================================================
    // CONFIGURATION
    // ========================================================================
    
    /** @brief Set movement speed (units/second) */
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    float getMoveSpeed() const { return m_moveSpeed; }
    
    /** @brief Set jump velocity */
    void setJumpVelocity(float velocity) { m_jumpVelocity = velocity; }
    float getJumpVelocity() const { return m_jumpVelocity; }
    
    /** @brief Set gravity strength */
    void setGravity(float gravity) { m_gravity = gravity; }
    float getGravity() const { return m_gravity; }
    
    /** @brief Set capsule dimensions */
    void setCapsuleSize(float radius, float height);
    float getCapsuleRadius() const { return m_capsuleRadius; }
    float getCapsuleHeight() const { return m_capsuleHeight; }
    
    /** @brief Set step offset (max step height to climb) */
    void setStepOffset(float offset) { m_stepOffset = offset; }
    float getStepOffset() const { return m_stepOffset; }
    
    // ========================================================================
    // POSITION
    // ========================================================================
    
    /** @brief Get current position */
    glm::vec3 getPosition() const;
    
    /** @brief Teleport to position */
    void setPosition(const glm::vec3& position);
    
    // ========================================================================
    // UPDATE
    // ========================================================================
    
    /** @brief Update the controller (call every frame) */
    void update(float deltaTime) override;
    
private:
    void applyGravity(float deltaTime);
    void updateGroundedState();
    
    // PhysX
    physx::PxController* m_controller = nullptr;
    physics::PhysicsWorld3D* m_world = nullptr;
    
    // State
    glm::vec3 m_velocity{0, 0, 0};
    glm::vec3 m_moveInput{0, 0, 0};
    bool m_isGrounded = false;
    bool m_jumpRequested = false;
    
    // Configuration
    float m_moveSpeed = 5.0f;       // Units per second
    float m_jumpVelocity = 8.0f;    // Initial jump velocity
    float m_gravity = 20.0f;        // Gravity strength
    float m_capsuleRadius = 0.4f;   // Capsule radius
    float m_capsuleHeight = 1.8f;   // Total height
    float m_stepOffset = 0.3f;      // Max step to climb
};

} // namespace engine
