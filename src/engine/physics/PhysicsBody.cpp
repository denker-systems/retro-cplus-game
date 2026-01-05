/**
 * @file PhysicsBody.cpp
 * @brief Implementation of PhysicsBody
 */
#include "PhysicsBody.h"

namespace engine {

PhysicsBody::PhysicsBody() : Node2D("PhysicsBody") {}

PhysicsBody::PhysicsBody(const std::string& name) : Node2D(name) {}

void PhysicsBody::update(float deltaTime) {
    if (!m_isStatic) {
        applyPhysics(deltaTime);
    }
    
    Node2D::update(deltaTime);
}

void PhysicsBody::applyPhysics(float deltaTime) {
    // Apply acceleration
    m_velocity += m_acceleration * deltaTime;
    
    // Apply friction
    m_velocity *= m_friction;
    
    // Apply velocity
    translate(m_velocity * deltaTime);
}

void PhysicsBody::addShape(std::unique_ptr<CollisionShape> shape) {
    if (shape) {
        m_shapes.push_back(std::move(shape));
    }
}

bool PhysicsBody::collidesWith(const PhysicsBody* other) const {
    if (!other) return false;
    
    // Check all shape combinations
    for (const auto& myShape : m_shapes) {
        for (const auto& otherShape : other->m_shapes) {
            if (myShape && otherShape && myShape->overlaps(*otherShape)) {
                return true;
            }
        }
    }
    
    return false;
}

} // namespace engine
