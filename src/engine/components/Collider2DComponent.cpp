/**
 * @file Collider2DComponent.cpp
 * @brief Box2D collider component implementation
 */
#include "Collider2DComponent.h"
#include "RigidBody2DComponent.h"
#include "engine/core/ActorObjectExtended.h"
#include <iostream>

namespace engine {

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

Collider2DComponent::Collider2DComponent(const std::string& name)
    : ActorComponent(name)
    , m_shapeId(b2_nullShapeId)
{
}

Collider2DComponent::~Collider2DComponent() {
    destroyShape();
}

// ============================================================================
// ACTORCOMPONENT LIFECYCLE
// ============================================================================

void Collider2DComponent::initialize() {
    // Shape will be created when initializeShape() is called
    // This is typically done after RigidBody2DComponent is initialized
}

void Collider2DComponent::shutdown() {
    destroyShape();
}

void Collider2DComponent::update(float deltaTime) {
    // Colliders don't need per-frame updates
    // The RigidBody handles position sync
}

// ============================================================================
// SHAPE CONFIGURATION
// ============================================================================

void Collider2DComponent::setBoxShape(float width, float height) {
    m_shapeType = ShapeType::Box;
    m_size = {width, height};
    
    if (m_shapeInitialized) {
        destroyShape();
        createShape();
    }
}

void Collider2DComponent::setCircleShape(float radius) {
    m_shapeType = ShapeType::Circle;
    m_size = {radius * 2.0f, radius * 2.0f};
    
    if (m_shapeInitialized) {
        destroyShape();
        createShape();
    }
}

void Collider2DComponent::setCapsuleShape(float width, float height) {
    m_shapeType = ShapeType::Capsule;
    m_size = {width, height};
    
    if (m_shapeInitialized) {
        destroyShape();
        createShape();
    }
}

void Collider2DComponent::setOffset(glm::vec2 offset) {
    m_offset = offset;
    
    if (m_shapeInitialized) {
        destroyShape();
        createShape();
    }
}

// ============================================================================
// PHYSICS MATERIAL
// ============================================================================

void Collider2DComponent::setDensity(float density) {
    m_density = density;
    // Note: Can't change density on existing shape in Box2D v3
    // Would need to recreate shape
}

void Collider2DComponent::setFriction(float friction) {
    m_friction = friction;
    if (m_shapeInitialized && b2Shape_IsValid(m_shapeId)) {
        b2Shape_SetFriction(m_shapeId, friction);
    }
}

void Collider2DComponent::setRestitution(float restitution) {
    m_restitution = restitution;
    if (m_shapeInitialized && b2Shape_IsValid(m_shapeId)) {
        b2Shape_SetRestitution(m_shapeId, restitution);
    }
}

// ============================================================================
// COLLISION SETTINGS
// ============================================================================

void Collider2DComponent::setTrigger(bool isTrigger) {
    m_isTrigger = isTrigger;
    // Note: Would need to recreate shape to change sensor status
}

void Collider2DComponent::setCollisionLayer(physics::CollisionCategory layer) {
    m_layer = layer;
    if (m_shapeInitialized && b2Shape_IsValid(m_shapeId)) {
        b2Filter filter = b2Shape_GetFilter(m_shapeId);
        filter.categoryBits = static_cast<uint64_t>(layer);
        b2Shape_SetFilter(m_shapeId, filter);
    }
}

void Collider2DComponent::setCollisionMask(physics::CollisionCategory mask) {
    m_mask = mask;
    if (m_shapeInitialized && b2Shape_IsValid(m_shapeId)) {
        b2Filter filter = b2Shape_GetFilter(m_shapeId);
        filter.maskBits = static_cast<uint64_t>(mask);
        b2Shape_SetFilter(m_shapeId, filter);
    }
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void Collider2DComponent::initializeShape() {
    if (m_shapeInitialized) return;
    
    m_rigidBody = findRigidBody();
    if (!m_rigidBody || !m_rigidBody->isInitialized()) {
        std::cerr << "[Collider2DComponent] Cannot initialize - no valid RigidBody2DComponent" << std::endl;
        return;
    }
    
    createShape();
}

RigidBody2DComponent* Collider2DComponent::findRigidBody() {
    if (!m_owner) return nullptr;
    
    // Try to cast owner to ActorObjectExtended to access components
    auto* extendedActor = dynamic_cast<ActorObjectExtended*>(m_owner);
    if (!extendedActor) return nullptr;
    
    return extendedActor->getComponent<RigidBody2DComponent>();
}

void Collider2DComponent::createShape() {
    if (!m_rigidBody || !m_rigidBody->getWorld()) return;
    
    physics::ShapeDef2D def;
    def.density = m_density;
    def.friction = m_friction;
    def.restitution = m_restitution;
    def.isSensor = m_isTrigger;
    def.category = m_layer;
    def.mask = m_mask;
    def.offset = m_offset;
    def.size = m_size;
    
    switch (m_shapeType) {
        case ShapeType::Box:
            def.type = physics::ShapeType2D::Box;
            break;
        case ShapeType::Circle:
            def.type = physics::ShapeType2D::Circle;
            break;
        case ShapeType::Capsule:
            def.type = physics::ShapeType2D::Capsule;
            break;
    }
    
    m_shapeId = m_rigidBody->getWorld()->addShape(m_rigidBody->getBodyId(), def);
    m_shapeInitialized = b2Shape_IsValid(m_shapeId);
    
    if (m_shapeInitialized) {
        std::cout << "[Collider2DComponent] Shape created: " 
                  << m_size.x << "x" << m_size.y << std::endl;
    }
}

void Collider2DComponent::destroyShape() {
    if (m_shapeInitialized && m_rigidBody && m_rigidBody->getWorld()) {
        m_rigidBody->getWorld()->removeShape(m_shapeId);
        m_shapeId = b2_nullShapeId;
        m_shapeInitialized = false;
    }
}

} // namespace engine
