/**
 * @file MovementComponent.cpp
 * @brief Movement Component Implementation
 */
#include "MovementComponent.h"
#include <cmath>

namespace engine {

MovementComponent::MovementComponent(const std::string& name)
    : ActorComponent(name)
{
}

void MovementComponent::addForce(Vec2 force) {
    m_acceleration = m_acceleration + force;
}

void MovementComponent::stop() {
    m_velocity = Vec2(0, 0);
    m_acceleration = Vec2(0, 0);
}

void MovementComponent::setWalkArea(float minX, float maxX, float minY, float maxY) {
    m_hasWalkArea = true;
    m_walkMinX = minX;
    m_walkMaxX = maxX;
    m_walkMinY = minY;
    m_walkMaxY = maxY;
}

void MovementComponent::clearWalkArea() {
    m_hasWalkArea = false;
}

void MovementComponent::update(float deltaTime) {
    if (!m_owner) return;
    
    // Apply acceleration
    m_velocity = m_velocity + m_acceleration * deltaTime;
    
    // Apply friction
    m_velocity.x *= m_friction;
    m_velocity.y *= m_friction;
    
    // Clamp to max speed
    float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
    if (speed > m_maxSpeed) {
        m_velocity.x = (m_velocity.x / speed) * m_maxSpeed;
        m_velocity.y = (m_velocity.y / speed) * m_maxSpeed;
    }
    
    // Update position
    Vec2 currentPos = m_owner->getPosition();
    Vec2 newPos = currentPos + m_velocity * deltaTime;
    
    // Apply walk area constraints
    if (m_hasWalkArea) {
        if (newPos.x < m_walkMinX) newPos.x = m_walkMinX;
        if (newPos.x > m_walkMaxX) newPos.x = m_walkMaxX;
        if (newPos.y < m_walkMinY) newPos.y = m_walkMinY;
        if (newPos.y > m_walkMaxY) newPos.y = m_walkMaxY;
    }
    
    m_owner->setPosition(newPos);
    
    // Reset acceleration
    m_acceleration = Vec2(0, 0);
}

} // namespace engine
