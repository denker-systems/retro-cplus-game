/**
 * @file Character3DActor.cpp
 * @brief Implementation of Character3DActor
 */
#include "Character3DActor.h"
#include "engine/components/CharacterController3DComponent.h"
#include "engine/utils/Logger.h"
#include <SDL.h>
#include <iostream>

namespace engine {

// ============================================================================
// Character3DActor
// ============================================================================

Character3DActor::Character3DActor(const std::string& name)
    : ActorObjectExtended(name) {
    // Add character controller component
    m_controller = addComponent<CharacterController3DComponent>();
}

bool Character3DActor::initializeController(physics::PhysicsWorld3D* world) {
    if (!m_controller) {
        LOG_ERROR("[Character3DActor] No controller component");
        return false;
    }
    
    glm::vec3 pos = getPosition3D();
    return m_controller->initializePhysics(world, pos);
}

void Character3DActor::shutdownController() {
    if (m_controller) {
        m_controller->shutdown();
    }
}

bool Character3DActor::isControllerInitialized() const {
    return m_controller && m_controller->isInitialized();
}

void Character3DActor::setMoveInput(const glm::vec3& input) {
    if (m_controller) {
        m_controller->setMoveInput(input);
    }
}

glm::vec3 Character3DActor::getVelocity() const {
    if (m_controller) {
        return m_controller->getVelocity();
    }
    return glm::vec3(0);
}

void Character3DActor::jump() {
    if (m_controller) {
        m_controller->jump();
    }
}

bool Character3DActor::isGrounded() const {
    if (m_controller) {
        return m_controller->isGrounded();
    }
    return false;
}

void Character3DActor::setMoveSpeed(float speed) {
    if (m_controller) {
        m_controller->setMoveSpeed(speed);
    }
}

float Character3DActor::getMoveSpeed() const {
    if (m_controller) {
        return m_controller->getMoveSpeed();
    }
    return 5.0f;
}

void Character3DActor::setJumpVelocity(float velocity) {
    if (m_controller) {
        m_controller->setJumpVelocity(velocity);
    }
}

float Character3DActor::getJumpVelocity() const {
    if (m_controller) {
        return m_controller->getJumpVelocity();
    }
    return 8.0f;
}

glm::vec3 Character3DActor::getPosition3D() const {
    if (m_controller && m_controller->isInitialized()) {
        return m_controller->getPosition();
    }
    // Convert from 2D storage
    auto pos2D = getPosition();
    float z = getZ();
    return glm::vec3(pos2D.x / 100.0f, z / 100.0f + 0.9f, pos2D.y / 100.0f);
}

void Character3DActor::setPosition3D(const glm::vec3& position) {
    if (m_controller && m_controller->isInitialized()) {
        m_controller->setPosition(position);
    }
    // Also update 2D storage
    setPosition(position.x * 100.0f, position.z * 100.0f);
    setZ((position.y - 0.9f) * 100.0f);
}

glm::vec3 Character3DActor::getForwardDirection() const {
    float yawRad = glm::radians(m_yaw);
    return glm::vec3(sin(yawRad), 0.0f, cos(yawRad));
}

glm::vec3 Character3DActor::getRightDirection() const {
    float yawRad = glm::radians(m_yaw);
    return glm::vec3(cos(yawRad), 0.0f, -sin(yawRad));
}

void Character3DActor::update(float deltaTime) {
    ActorObjectExtended::update(deltaTime);
    
    // Sync position from controller to actor
    if (m_controller && m_controller->isInitialized()) {
        glm::vec3 pos = m_controller->getPosition();
        setPosition(pos.x * 100.0f, pos.z * 100.0f);
        setZ((pos.y - 0.9f) * 100.0f);
    }
}

// ============================================================================
// Player3DActor
// ============================================================================

Player3DActor::Player3DActor()
    : Character3DActor("Player") {
    setMoveSpeed(6.0f);
    setJumpVelocity(10.0f);
}

void Player3DActor::update(float deltaTime) {
    if (m_inputEnabled) {
        handleInput(deltaTime);
    }
    
    Character3DActor::update(deltaTime);
}

void Player3DActor::handleInput(float deltaTime) {
    if (!isControllerInitialized()) return;
    
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    
    // WASD movement relative to player rotation
    float forward = 0.0f;
    float right = 0.0f;
    
    if (keys[SDL_SCANCODE_W]) forward += 1.0f;   // Forward
    if (keys[SDL_SCANCODE_S]) forward -= 1.0f;   // Backward
    if (keys[SDL_SCANCODE_D]) right += 1.0f;     // Right
    if (keys[SDL_SCANCODE_A]) right -= 1.0f;     // Left
    
    // Convert to world space based on player yaw
    glm::vec3 moveDir(0);
    if (forward != 0.0f || right != 0.0f) {
        glm::vec3 forwardDir = getForwardDirection();
        glm::vec3 rightDir = getRightDirection();
        moveDir = forwardDir * forward + rightDir * right;
        
        // Debug logging
        static int moveLogCounter = 0;
        if (moveLogCounter++ % 30 == 0) {
            std::cout << "[Player3D] Movement - yaw=" << getYaw()
                      << " forward=(" << forwardDir.x << "," << forwardDir.y << "," << forwardDir.z << ")"
                      << " right=(" << rightDir.x << "," << rightDir.y << "," << rightDir.z << ")"
                      << " moveDir=(" << moveDir.x << "," << moveDir.y << "," << moveDir.z << ")" << std::endl;
        }
        
        if (glm::length(moveDir) > 0.01f) {
            moveDir = glm::normalize(moveDir);
        }
    }
    
    setMoveInput(moveDir);
    
    // Jump with Space
    if (keys[SDL_SCANCODE_SPACE]) {
        jump();
    }
}

void Player3DActor::handleMouseLook(float deltaX, float deltaY) {
    // Rotate player with mouse (yaw only, no pitch for character)
    float oldYaw = m_yaw;
    addYaw(deltaX * m_mouseSensitivity);
    std::cout << "[Player3D] handleMouseLook - deltaX=" << deltaX 
              << " sensitivity=" << m_mouseSensitivity
              << " yaw: " << oldYaw << " -> " << m_yaw << std::endl;
}

} // namespace engine
