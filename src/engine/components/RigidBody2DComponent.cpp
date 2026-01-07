/**
 * @file RigidBody2DComponent.cpp
 * @brief Box2D rigid body component implementation
 */
#include "RigidBody2DComponent.h"
#include "engine/core/ActorObject.h"
#include "engine/physics/box2d/PhysicsConversions.h"
#include <iostream>

namespace engine {

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

RigidBody2DComponent::RigidBody2DComponent(const std::string& name)
    : ActorComponent(name)
    , m_bodyId(b2_nullBodyId)
{
}

RigidBody2DComponent::~RigidBody2DComponent() {
    destroyBody();
}

// ============================================================================
// ACTORCOMPONENT LIFECYCLE
// ============================================================================

void RigidBody2DComponent::initialize() {
    // Body will be created when initializeBody() is called with a PhysicsWorld
    // This allows the Scene to control when physics is set up
}

void RigidBody2DComponent::shutdown() {
    destroyBody();
}

void RigidBody2DComponent::update(float deltaTime) {
    if (!m_bodyInitialized || !m_owner) return;
    
    // Sync Actor position FROM Box2D body
    syncActorFromBody();
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void RigidBody2DComponent::initializeBody(physics::PhysicsWorld2D* world) {
    if (m_bodyInitialized) return;
    if (!world || !world->isInitialized()) {
        std::cerr << "[RigidBody2DComponent] Cannot initialize - world not ready" << std::endl;
        return;
    }
    
    m_world = world;
    createBody();
}

void RigidBody2DComponent::createBody() {
    if (!m_world || !m_owner) return;
    
    physics::BodyDef2D def;
    
    // Convert our BodyType to physics BodyType2D
    switch (m_bodyType) {
        case BodyType::Static:
            def.type = physics::BodyType2D::Static;
            break;
        case BodyType::Dynamic:
            def.type = physics::BodyType2D::Dynamic;
            break;
        case BodyType::Kinematic:
            def.type = physics::BodyType2D::Kinematic;
            break;
    }
    
    // Get initial position from Actor
    Vec2 actorVec = m_owner->getPosition();
    def.position = glm::vec2(actorVec.x, actorVec.y);
    def.gravityScale = m_gravityScale;
    def.fixedRotation = m_fixedRotation;
    def.linearDamping = m_linearDamping;
    def.linearVelocity = m_initialVelocity;
    def.userData = m_owner;
    
    m_bodyId = m_world->createBody(def);
    m_bodyInitialized = b2Body_IsValid(m_bodyId);
    
    if (m_bodyInitialized) {
        std::cout << "[RigidBody2DComponent] Body created at (" 
                  << actorVec.x << ", " << actorVec.y << ")" << std::endl;
    }
}

void RigidBody2DComponent::destroyBody() {
    if (m_bodyInitialized && m_world) {
        m_world->destroyBody(m_bodyId);
        m_bodyId = b2_nullBodyId;
        m_bodyInitialized = false;
    }
}

void RigidBody2DComponent::syncActorFromBody() {
    if (!m_bodyInitialized || !m_owner) return;
    
    glm::vec2 bodyPos = m_world->getBodyPosition(m_bodyId);
    m_owner->setPosition(Vec2(bodyPos.x, bodyPos.y));
}

void RigidBody2DComponent::syncBodyFromActor() {
    if (!m_bodyInitialized || !m_owner) return;
    
    Vec2 pos = m_owner->getPosition();
    m_world->setBodyPosition(m_bodyId, glm::vec2(pos.x, pos.y));
}

// ============================================================================
// BODY TYPE
// ============================================================================

void RigidBody2DComponent::setBodyType(BodyType type) {
    m_bodyType = type;
    
    if (m_bodyInitialized) {
        // Recreate body with new type
        glm::vec2 pos = getPosition();
        glm::vec2 vel = getVelocity();
        destroyBody();
        m_initialVelocity = vel;
        createBody();
        setPosition(pos);
    }
}

// ============================================================================
// PHYSICS PROPERTIES
// ============================================================================

void RigidBody2DComponent::setGravityScale(float scale) {
    m_gravityScale = scale;
    if (m_bodyInitialized) {
        b2Body_SetGravityScale(m_bodyId, scale);
    }
}

void RigidBody2DComponent::setFixedRotation(bool fixed) {
    m_fixedRotation = fixed;
    if (m_bodyInitialized) {
        b2Body_SetFixedRotation(m_bodyId, fixed);
    }
}

void RigidBody2DComponent::setLinearDamping(float damping) {
    m_linearDamping = damping;
    if (m_bodyInitialized) {
        b2Body_SetLinearDamping(m_bodyId, damping);
    }
}

// ============================================================================
// VELOCITY & FORCES
// ============================================================================

void RigidBody2DComponent::setVelocity(glm::vec2 velocity) {
    if (m_bodyInitialized) {
        m_world->setBodyVelocity(m_bodyId, velocity);
    } else {
        m_initialVelocity = velocity;
    }
}

glm::vec2 RigidBody2DComponent::getVelocity() const {
    if (m_bodyInitialized) {
        return m_world->getBodyVelocity(m_bodyId);
    }
    return m_initialVelocity;
}

void RigidBody2DComponent::setVelocityX(float vx) {
    glm::vec2 vel = getVelocity();
    vel.x = vx;
    setVelocity(vel);
}

void RigidBody2DComponent::setVelocityY(float vy) {
    glm::vec2 vel = getVelocity();
    vel.y = vy;
    setVelocity(vel);
}

void RigidBody2DComponent::addForce(glm::vec2 force) {
    if (m_bodyInitialized) {
        m_world->applyForce(m_bodyId, force);
    }
}

void RigidBody2DComponent::addImpulse(glm::vec2 impulse) {
    if (m_bodyInitialized) {
        m_world->applyImpulse(m_bodyId, impulse);
    }
}

// ============================================================================
// POSITION
// ============================================================================

glm::vec2 RigidBody2DComponent::getPosition() const {
    if (m_bodyInitialized) {
        return m_world->getBodyPosition(m_bodyId);
    }
    if (m_owner) {
        Vec2 pos = m_owner->getPosition();
        return glm::vec2{pos.x, pos.y};
    }
    return glm::vec2{0, 0};
}

void RigidBody2DComponent::setPosition(glm::vec2 position) {
    if (m_bodyInitialized) {
        m_world->setBodyPosition(m_bodyId, position);
    }
    if (m_owner) {
        m_owner->setPosition(Vec2(position.x, position.y));
    }
}

} // namespace engine
