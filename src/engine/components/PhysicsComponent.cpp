/**
 * @file PhysicsComponent.cpp
 * @brief Physics Component Implementation
 */
#include "PhysicsComponent.h"
#include "engine/core/CoreRedirects.h"
#include <cmath>

namespace engine {

PhysicsComponent::PhysicsComponent(const std::string& name)
    : ActorComponent(name)
{
}

void PhysicsComponent::update(float deltaTime) {
    if (!m_owner) return;
    
    // Apply gravity
    if (!m_onGround) {
        m_accumulatedForces.y += m_gravity * m_mass;
    }
    
    // F = ma, so a = F/m
    Vec2 acceleration = m_accumulatedForces / m_mass;
    
    // Update velocity
    m_velocity = m_velocity + acceleration * deltaTime;
    
    // Apply friction
    m_velocity.x *= m_friction;
    if (m_onGround) {
        m_velocity.y *= m_friction;
    }
    
    // Update position
    Vec2 currentPos = m_owner->getPosition();
    m_owner->setPosition(currentPos + m_velocity * deltaTime);
    
    // Reset accumulated forces
    m_accumulatedForces = Vec2(0, 0);
}

void PhysicsComponent::addForce(Vec2 force) {
    m_accumulatedForces = m_accumulatedForces + force;
}

void PhysicsComponent::addImpulse(Vec2 impulse) {
    m_velocity = m_velocity + impulse;
}

void PhysicsComponent::stop() {
    m_velocity = Vec2(0, 0);
    m_acceleration = Vec2(0, 0);
    m_accumulatedForces = Vec2(0, 0);
}

} // namespace engine
