/**
 * @file CharacterController2D.cpp
 * @brief Platformer character controller implementation
 */
#include "CharacterController2D.h"
#include "RigidBody2DComponent.h"
#include "engine/core/ActorObjectExtended.h"
#include <cmath>
#include <algorithm>

namespace engine {

// ============================================================================
// CONSTRUCTOR
// ============================================================================

CharacterController2D::CharacterController2D(const std::string& name)
    : ActorComponent(name)
{
}

// ============================================================================
// ACTORCOMPONENT LIFECYCLE
// ============================================================================

void CharacterController2D::initialize() {
    // Find sibling RigidBody2DComponent
    if (m_owner) {
        auto* extendedActor = dynamic_cast<ActorObjectExtended*>(m_owner);
        if (extendedActor) {
            m_rigidBody = extendedActor->getComponent<RigidBody2DComponent>();
        }
    }
}

void CharacterController2D::update(float deltaTime) {
    if (!m_rigidBody || !m_rigidBody->isInitialized()) return;
    
    // Store previous grounded state
    m_wasGrounded = m_isGrounded;
    
    // Update states
    updateGroundedState();
    updateMovement(deltaTime);
    updateJump(deltaTime);
    
    // Reset air jumps when landing
    if (m_isGrounded && !m_wasGrounded) {
        m_airJumpsRemaining = m_maxAirJumps;
        m_isJumping = false;
    }
}

// ============================================================================
// MOVEMENT ACTIONS
// ============================================================================

void CharacterController2D::move(float direction) {
    m_moveInput = std::clamp(direction, -1.0f, 1.0f);
    
    // Update facing direction
    if (std::abs(m_moveInput) > 0.1f) {
        m_facingDirection = m_moveInput > 0 ? 1.0f : -1.0f;
    }
}

void CharacterController2D::jump() {
    // Buffer jump input
    m_jumpBufferTimer = m_jumpBufferTime;
    m_jumpReleased = false;
}

void CharacterController2D::releaseJump() {
    m_jumpReleased = true;
    
    // Cut jump short if still going up
    if (m_isJumping && !m_isGrounded) {
        glm::vec2 vel = m_rigidBody->getVelocity();
        if (vel.y < 0) { // Moving up (negative Y in screen coords)
            vel.y *= m_jumpCutMultiplier;
            m_rigidBody->setVelocity(vel);
        }
    }
}

// ============================================================================
// STATE QUERIES
// ============================================================================

bool CharacterController2D::isFalling() const {
    if (!m_rigidBody) return false;
    glm::vec2 vel = m_rigidBody->getVelocity();
    return vel.y > 0 && !m_isGrounded; // Positive Y = falling (screen coords)
}

bool CharacterController2D::isMoving() const {
    if (!m_rigidBody) return false;
    glm::vec2 vel = m_rigidBody->getVelocity();
    return std::abs(vel.x) > 10.0f;
}

glm::vec2 CharacterController2D::getVelocity() const {
    if (!m_rigidBody) return {0, 0};
    return m_rigidBody->getVelocity();
}

void CharacterController2D::setGrounded(bool grounded) {
    m_isGrounded = grounded;
}

// ============================================================================
// INTERNAL UPDATE METHODS
// ============================================================================

void CharacterController2D::updateGroundedState() {
    // For now, use a simple velocity-based check
    // In a full implementation, this would use raycasting
    glm::vec2 vel = m_rigidBody->getVelocity();
    
    // Consider grounded if vertical velocity is very small
    // This is a simplified approach - proper ground detection uses raycasts
    if (std::abs(vel.y) < 5.0f && m_wasGrounded) {
        m_isGrounded = true;
    } else if (vel.y > 50.0f) { // Falling fast = not grounded
        m_isGrounded = false;
    }
    
    // Update coyote timer
    if (m_isGrounded) {
        m_coyoteTimer = m_coyoteTime;
    } else {
        m_coyoteTimer -= 1.0f / 60.0f; // Approximate, should use deltaTime
    }
}

void CharacterController2D::updateMovement(float deltaTime) {
    glm::vec2 vel = m_rigidBody->getVelocity();
    
    float targetSpeed = m_isRunning ? m_runSpeed : m_walkSpeed;
    float targetVelX = m_moveInput * targetSpeed;
    
    // Accelerate or decelerate towards target
    float speedDiff = targetVelX - vel.x;
    float accelRate = (std::abs(targetVelX) > 0.01f) ? m_acceleration : m_deceleration;
    
    // Apply acceleration
    float movement = speedDiff * std::min(1.0f, accelRate * deltaTime / std::max(std::abs(speedDiff), 1.0f));
    vel.x += movement;
    
    // Set velocity
    m_rigidBody->setVelocityX(vel.x);
}

void CharacterController2D::updateJump(float deltaTime) {
    // Decrease jump buffer
    if (m_jumpBufferTimer > 0) {
        m_jumpBufferTimer -= deltaTime;
    }
    
    // Check if we can jump
    bool canJump = false;
    
    if (m_jumpBufferTimer > 0) {
        // Grounded jump (with coyote time)
        if (m_isGrounded || m_coyoteTimer > 0) {
            canJump = true;
        }
        // Air jump
        else if (m_airJumpsRemaining > 0) {
            canJump = true;
            m_airJumpsRemaining--;
        }
    }
    
    // Execute jump
    if (canJump && !m_jumpReleased) {
        glm::vec2 vel = m_rigidBody->getVelocity();
        vel.y = -m_jumpForce; // Negative Y = up (screen coords)
        m_rigidBody->setVelocity(vel);
        
        m_isJumping = true;
        m_jumpBufferTimer = 0;
        m_coyoteTimer = 0;
        m_jumpReleased = true; // Require release before next jump
    }
}

void CharacterController2D::applyGravityModifiers() {
    // Could add things like:
    // - Higher gravity when falling
    // - Lower gravity at jump apex
    // For now, Box2D handles gravity
}

} // namespace engine
