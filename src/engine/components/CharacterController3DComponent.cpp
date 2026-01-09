/**
 * @file CharacterController3DComponent.cpp
 * @brief Implementation of CharacterController3DComponent
 */
#include "CharacterController3DComponent.h"
#include "engine/physics/physx/PhysicsWorld3D.h"
#include "engine/utils/Logger.h"
#include <iostream>
#include <PxPhysicsAPI.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <characterkinematic/PxCapsuleController.h>

using namespace physx;

namespace engine {

CharacterController3DComponent::CharacterController3DComponent()
    : ActorComponent("CharacterController3D") {
}

CharacterController3DComponent::~CharacterController3DComponent() {
    shutdown();
}

bool CharacterController3DComponent::initializePhysics(physics::PhysicsWorld3D* world, const glm::vec3& position) {
    if (!world || !world->isInitialized()) {
        LOG_ERROR("[CharacterController3D] Cannot initialize - world not ready");
        return false;
    }
    
    m_world = world;
    
    // Get or create controller manager
    PxControllerManager* manager = world->getControllerManager();
    if (!manager) {
        LOG_ERROR("[CharacterController3D] Cannot get controller manager");
        return false;
    }
    
    // Create capsule controller
    PxCapsuleControllerDesc desc;
    desc.height = m_capsuleHeight - 2.0f * m_capsuleRadius;  // Height is body only, not including caps
    desc.radius = m_capsuleRadius;
    desc.position = PxExtendedVec3(position.x, position.y, position.z);
    desc.material = world->getDefaultMaterial();
    desc.stepOffset = m_stepOffset;
    desc.slopeLimit = cosf(PxDegToRad(45.0f));  // 45 degree max slope
    desc.contactOffset = 0.1f;
    desc.upDirection = PxVec3(0, 1, 0);
    desc.reportCallback = nullptr;
    desc.behaviorCallback = nullptr;
    
    m_controller = manager->createController(desc);
    
    if (!m_controller) {
        LOG_ERROR("[CharacterController3D] Failed to create controller");
        return false;
    }
    
    LOG_INFO("[CharacterController3D] Initialized at (" + 
             std::to_string(position.x) + ", " + 
             std::to_string(position.y) + ", " + 
             std::to_string(position.z) + ")");
    
    return true;
}

void CharacterController3DComponent::shutdown() {
    if (m_controller) {
        m_controller->release();
        m_controller = nullptr;
    }
    m_world = nullptr;
}

void CharacterController3DComponent::move(const glm::vec3& displacement, float deltaTime) {
    if (!m_controller) return;
    
    // Always apply a tiny downward force to detect ground contact
    PxVec3 disp(displacement.x, displacement.y - 0.01f, displacement.z);
    
    PxControllerFilters filters;
    PxControllerCollisionFlags flags = m_controller->move(disp, 0.0001f, deltaTime, filters);
    
    // Update grounded state from collision flags
    if (flags & PxControllerCollisionFlag::eCOLLISION_DOWN) {
        m_isGrounded = true;
    } else {
        m_isGrounded = false;
    }
}

void CharacterController3DComponent::jump() {
    if (m_isGrounded) {
        m_jumpRequested = true;
    }
}

glm::vec3 CharacterController3DComponent::getPosition() const {
    if (!m_controller) return glm::vec3(0);
    
    PxExtendedVec3 pos = m_controller->getPosition();
    return glm::vec3(static_cast<float>(pos.x), 
                     static_cast<float>(pos.y), 
                     static_cast<float>(pos.z));
}

void CharacterController3DComponent::setPosition(const glm::vec3& position) {
    if (!m_controller) return;
    
    m_controller->setPosition(PxExtendedVec3(position.x, position.y, position.z));
}

void CharacterController3DComponent::setCapsuleSize(float radius, float height) {
    m_capsuleRadius = radius;
    m_capsuleHeight = height;
    
    // If already initialized, update the controller
    if (m_controller) {
        PxCapsuleController* capsule = static_cast<PxCapsuleController*>(m_controller);
        capsule->setRadius(radius);
        capsule->setHeight(height - 2.0f * radius);
    }
}

void CharacterController3DComponent::update(float deltaTime) {
    if (!m_controller) return;
    
    // Apply gravity
    applyGravity(deltaTime);
    
    // Handle jump
    if (m_jumpRequested && m_isGrounded) {
        m_velocity.y = m_jumpVelocity;
        m_jumpRequested = false;
        m_isGrounded = false;
        std::cout << "[CharacterController3D] JUMP! velocity.y=" << m_velocity.y << std::endl;
    }
    
    // Calculate movement
    glm::vec3 moveDir = m_moveInput;
    if (glm::length(moveDir) > 0.01f) {
        moveDir = glm::normalize(moveDir);
    }
    
    // Horizontal movement
    glm::vec3 displacement = moveDir * m_moveSpeed * deltaTime;
    
    // Vertical movement (gravity/jump)
    displacement.y += m_velocity.y * deltaTime;
    
    // Debug logging
    static int updateLogCounter = 0;
    if (glm::length(moveDir) > 0.01f || !m_isGrounded) {
        if (updateLogCounter++ % 60 == 0) {
            glm::vec3 pos = getPosition();
            std::cout << "[CharacterController3D] pos=(" << pos.x << "," << pos.y << "," << pos.z << ")"
                      << " vel.y=" << m_velocity.y
                      << " grounded=" << (m_isGrounded ? "YES" : "NO")
                      << " moveDir=(" << moveDir.x << "," << moveDir.z << ")"
                      << std::endl;
        }
    }
    
    // Apply movement
    move(displacement, deltaTime);
    
    // Reset move input
    m_moveInput = glm::vec3(0);
}

void CharacterController3DComponent::applyGravity(float deltaTime) {
    if (!m_isGrounded) {
        m_velocity.y -= m_gravity * deltaTime;
        
        // Terminal velocity
        if (m_velocity.y < -50.0f) {
            m_velocity.y = -50.0f;
        }
    } else {
        // Reset vertical velocity when grounded
        if (m_velocity.y < 0) {
            m_velocity.y = 0;
        }
    }
}

void CharacterController3DComponent::updateGroundedState() {
    // Ground state is updated in move() via collision flags
}

} // namespace engine
