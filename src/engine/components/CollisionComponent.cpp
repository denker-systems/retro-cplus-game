/**
 * @file CollisionComponent.cpp
 * @brief Collision Component Implementation
 */
#include "CollisionComponent.h"
#include "engine/core/CoreRedirects.h"
#include <cmath>

namespace engine {

CollisionComponent::CollisionComponent(const std::string& name)
    : ActorComponent(name)
{
}

bool CollisionComponent::checkCollision(CollisionComponent* other) {
    if (!other || !m_owner || !other->getOwner()) return false;
    
    Vec2 posA = m_owner->getPosition() + m_offset;
    Vec2 posB = other->getOwner()->getPosition() + other->getOffset();
    
    if (m_shape == CollisionShape::Box && other->getShape() == CollisionShape::Box) {
        // AABB collision
        return (posA.x < posB.x + other->getSize().x &&
                posA.x + m_size.x > posB.x &&
                posA.y < posB.y + other->getSize().y &&
                posA.y + m_size.y > posB.y);
    }
    else if (m_shape == CollisionShape::Circle && other->getShape() == CollisionShape::Circle) {
        // Circle collision
        float dx = posB.x - posA.x;
        float dy = posB.y - posA.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        return distance < (m_radius + other->getRadius());
    }
    
    return false;
}

} // namespace engine
