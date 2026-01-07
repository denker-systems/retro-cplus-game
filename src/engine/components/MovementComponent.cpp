/**
 * @file MovementComponent.cpp
 * @brief Movement Component Implementation
 */
#include "MovementComponent.h"
#include "engine/core/Vec2.h"
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
    m_hasTarget = false;
}

void MovementComponent::setTarget(const Vec2& target) {
    m_target = target;
    m_hasTarget = true;
    
    if (onMovementStart) {
        onMovementStart(target);
    }
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
    
    Vec2 currentPos = m_owner->getPosition();
    Vec2 newPos = currentPos;
    
    // Point-and-click movement
    if (m_hasTarget) {
        Vec2 toTarget = m_target - currentPos;
        float distance = toTarget.length();
        
        if (distance > 2.0f) {
            // Move towards target
            Vec2 direction = toTarget.normalized();
            Vec2 moveVector = direction * m_maxSpeed * deltaTime;
            
            // Don't overshoot target
            if (moveVector.length() > distance) {
                moveVector = toTarget;
            }
            
            newPos = currentPos + moveVector;
            m_velocity = moveVector / deltaTime; // Update velocity for animations
        } else {
            // Reached target
            newPos = m_target;
            m_hasTarget = false;
            m_velocity = Vec2(0, 0);
            
            if (onMovementComplete) {
                onMovementComplete();
            }
        }
    } else {
        // Physics-based movement
        // Apply acceleration
        m_velocity = m_velocity + m_acceleration * deltaTime;
        
        // Apply friction
        m_velocity.x *= m_friction;
        m_velocity.y *= m_friction;
        
        // Clamp to max speed
        float speed = m_velocity.length();
        if (speed > m_maxSpeed) {
            m_velocity = (m_velocity / speed) * m_maxSpeed;
        }
        
        newPos = currentPos + m_velocity * deltaTime;
    }
    
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
