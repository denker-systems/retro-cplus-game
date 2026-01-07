/**
 * @file PhysicsLoader.cpp
 * @brief Implementation of PhysicsLoader
 */
#include "PhysicsLoader.h"
#include "PhysicsWorld2D.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/RigidBody2DComponent.h"
#include "engine/components/Collider2DComponent.h"
#include "engine/components/TriggerComponent.h"

namespace engine {

bool PhysicsLoader::applyPhysics(ActorObjectExtended* actor, const PhysicsData& data, physics::PhysicsWorld2D* world) {
    if (!actor || !data.enabled || !world) {
        return false;
    }
    
    // Create or get RigidBody2DComponent
    auto* rb = actor->getComponent<RigidBody2DComponent>();
    if (!rb) {
        rb = actor->addComponent<RigidBody2DComponent>();
    }
    
    // Set body type
    if (data.bodyType == "dynamic") {
        rb->setBodyType(RigidBody2DComponent::BodyType::Dynamic);
    } else if (data.bodyType == "kinematic") {
        rb->setBodyType(RigidBody2DComponent::BodyType::Kinematic);
    } else {
        rb->setBodyType(RigidBody2DComponent::BodyType::Static);
    }
    
    rb->setFixedRotation(data.fixedRotation);
    rb->setGravityScale(data.gravityScale);
    
    // Initialize body in physics world
    rb->initializeBody(world);
    
    // Create or get Collider2DComponent
    auto* col = actor->getComponent<Collider2DComponent>();
    if (!col) {
        col = actor->addComponent<Collider2DComponent>();
    }
    
    applyColliderData(col, data.collider);
    col->initializeShape();
    
    // If trigger, add TriggerComponent
    if (data.collider.isTrigger) {
        auto* trigger = actor->getComponent<TriggerComponent>();
        if (!trigger) {
            actor->addComponent<TriggerComponent>();
        }
    }
    
    return true;
}

void PhysicsLoader::removePhysics(ActorObjectExtended* actor) {
    if (!actor) return;
    
    actor->removeComponent<TriggerComponent>();
    actor->removeComponent<Collider2DComponent>();
    actor->removeComponent<RigidBody2DComponent>();
}

bool PhysicsLoader::hasPhysics(ActorObjectExtended* actor) {
    if (!actor) return false;
    return actor->getComponent<RigidBody2DComponent>() != nullptr;
}

void PhysicsLoader::applyColliderData(Collider2DComponent* collider, const ColliderData& data) {
    if (!collider) return;
    
    // Set shape
    if (data.shape == "circle") {
        collider->setCircleShape(data.width / 2.0f);
    } else if (data.shape == "capsule") {
        collider->setCapsuleShape(data.width, data.height);
    } else {
        collider->setBoxShape(data.width, data.height);
    }
    
    // Set offset
    collider->setOffset(glm::vec2(data.offsetX, data.offsetY));
    
    // Set physics material
    collider->setDensity(data.density);
    collider->setFriction(data.friction);
    collider->setRestitution(data.restitution);
    
    // Set trigger mode
    collider->setTrigger(data.isTrigger);
}

} // namespace engine
