/**
 * @file CharacterController2D.h
 * @brief Platformer character controller with walk, run, jump
 * 
 * Provides high-level character movement for 2D platformers.
 * Works with RigidBody2DComponent for physics-based movement.
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/core/Vec2.h"
#include <glm/glm.hpp>

namespace engine {

class RigidBody2DComponent;

/**
 * @brief Platformer character controller
 * 
 * Usage:
 *   auto* controller = actor->addComponent<CharacterController2D>();
 *   controller->setWalkSpeed(200.0f);
 *   controller->setJumpForce(400.0f);
 *   
 *   // In update:
 *   controller->move(inputX);  // -1 to 1
 *   if (jumpPressed) controller->jump();
 */
class CharacterController2D : public ActorComponent {
public:
    CharacterController2D(const std::string& name = "CharacterController2D");
    virtual ~CharacterController2D() = default;
    
    // ========================================================================
    // ACTORCOMPONENT LIFECYCLE
    // ========================================================================
    
    void initialize() override;
    void update(float deltaTime) override;
    
    // ========================================================================
    // MOVEMENT SETTINGS
    // ========================================================================
    
    void setWalkSpeed(float speed) { m_walkSpeed = speed; }
    float getWalkSpeed() const { return m_walkSpeed; }
    
    void setRunSpeed(float speed) { m_runSpeed = speed; }
    float getRunSpeed() const { return m_runSpeed; }
    
    void setAcceleration(float accel) { m_acceleration = accel; }
    float getAcceleration() const { return m_acceleration; }
    
    void setDeceleration(float decel) { m_deceleration = decel; }
    float getDeceleration() const { return m_deceleration; }
    
    // ========================================================================
    // JUMP SETTINGS
    // ========================================================================
    
    void setJumpForce(float force) { m_jumpForce = force; }
    float getJumpForce() const { return m_jumpForce; }
    
    void setMaxAirJumps(int jumps) { m_maxAirJumps = jumps; }
    int getMaxAirJumps() const { return m_maxAirJumps; }
    
    void setCoyoteTime(float time) { m_coyoteTime = time; }
    float getCoyoteTime() const { return m_coyoteTime; }
    
    void setJumpBufferTime(float time) { m_jumpBufferTime = time; }
    float getJumpBufferTime() const { return m_jumpBufferTime; }
    
    // ========================================================================
    // MOVEMENT ACTIONS
    // ========================================================================
    
    /**
     * @brief Move character horizontally
     * @param direction -1 (left) to 1 (right), 0 = stop
     */
    void move(float direction);
    
    /**
     * @brief Set running state
     * @param running true to use run speed, false for walk speed
     */
    void setRunning(bool running) { m_isRunning = running; }
    bool isRunning() const { return m_isRunning; }
    
    /**
     * @brief Attempt to jump
     * Uses coyote time and jump buffering
     */
    void jump();
    
    /**
     * @brief Release jump (for variable jump height)
     */
    void releaseJump();
    
    // ========================================================================
    // STATE QUERIES
    // ========================================================================
    
    bool isGrounded() const { return m_isGrounded; }
    bool isJumping() const { return m_isJumping; }
    bool isFalling() const;
    bool isMoving() const;
    
    float getFacingDirection() const { return m_facingDirection; }
    glm::vec2 getVelocity() const;
    
    // ========================================================================
    // GROUND DETECTION
    // ========================================================================
    
    void setGroundCheckDistance(float distance) { m_groundCheckDistance = distance; }
    float getGroundCheckDistance() const { return m_groundCheckDistance; }
    
    /**
     * @brief Manually set grounded state (for custom ground detection)
     */
    void setGrounded(bool grounded);
    
private:
    void updateGroundedState();
    void updateMovement(float deltaTime);
    void updateJump(float deltaTime);
    void applyGravityModifiers();
    
    RigidBody2DComponent* m_rigidBody = nullptr;
    
    // Movement settings
    float m_walkSpeed = 200.0f;
    float m_runSpeed = 350.0f;
    float m_acceleration = 1500.0f;
    float m_deceleration = 2000.0f;
    
    // Jump settings
    float m_jumpForce = 400.0f;
    int m_maxAirJumps = 0;
    float m_coyoteTime = 0.1f;
    float m_jumpBufferTime = 0.1f;
    float m_jumpCutMultiplier = 0.5f;  // Velocity multiplier when releasing jump
    
    // Ground detection
    float m_groundCheckDistance = 2.0f;
    
    // State
    float m_moveInput = 0.0f;
    bool m_isRunning = false;
    bool m_isGrounded = false;
    bool m_isJumping = false;
    bool m_jumpReleased = true;
    int m_airJumpsRemaining = 0;
    float m_coyoteTimer = 0.0f;
    float m_jumpBufferTimer = 0.0f;
    float m_facingDirection = 1.0f;  // 1 = right, -1 = left
    
    // Previous state for edge detection
    bool m_wasGrounded = false;
};

} // namespace engine
