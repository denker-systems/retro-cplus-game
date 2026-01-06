/**
 * @file KinematicBody.cpp
 * @brief Implementation of KinematicBody
 */
#include "KinematicBody.h"
#include <cmath>

namespace engine {

KinematicBody::KinematicBody() : PhysicsBody("KinematicBody") {}

KinematicBody::KinematicBody(const std::string& name) : PhysicsBody(name) {}

Vec2 KinematicBody::moveAndSlide(const Vec2& velocity, const std::vector<CollisionShape*>& obstacles) {
    Vec2 motion = velocity;
    Vec2 remainingMotion = motion;
    m_isOnFloor = false;
    
    for (int i = 0; i < m_maxSlides && remainingMotion.lengthSquared() > m_stopThreshold; i++) {
        // Try to move
        Vec2 oldPos = getPosition();
        setPosition(oldPos.x + remainingMotion.x, oldPos.y + remainingMotion.y);
        
        // Check for collisions
        bool collided = false;
        for (CollisionShape* obstacle : obstacles) {
            if (!obstacle) continue;
            
            // Check if any of our shapes collide with obstacle
            for (const auto& shape : m_shapes) {
                if (shape && shape->overlaps(*obstacle)) {
                    collided = true;
                    
                    // Simple collision response: move back
                    setPosition(oldPos);
                    
                    // Calculate slide direction (perpendicular to collision normal)
                    // For now, just stop motion in that direction
                    // TODO: Implement proper collision normal calculation
                    Vec2 normal(0, -1);  // Assume floor for now
                    
                    // Check if this is a floor
                    float angle = std::acos(normal.dot(Vec2(0, -1)));
                    if (angle <= m_floorMaxAngle) {
                        m_isOnFloor = true;
                        m_floorNormal = normal;
                    }
                    
                    // Slide along surface
                    remainingMotion = slideOnSlope(remainingMotion, normal);
                    break;
                }
            }
            
            if (collided) break;
        }
        
        if (!collided) {
            // No collision, motion complete
            break;
        }
    }
    
    return getVelocity();
}

bool KinematicBody::moveAndCollide(const Vec2& motion, const std::vector<CollisionShape*>& obstacles) {
    Vec2 oldPos = getPosition();
    setPosition(oldPos.x + motion.x, oldPos.y + motion.y);
    
    // Check for collisions
    for (CollisionShape* obstacle : obstacles) {
        if (!obstacle) continue;
        
        for (const auto& shape : m_shapes) {
            if (shape && shape->overlaps(*obstacle)) {
                // Collision detected, move back
                setPosition(oldPos);
                return true;
            }
        }
    }
    
    return false;
}

Vec2 KinematicBody::slideOnSlope(const Vec2& velocity, const Vec2& normal) {
    // Project velocity onto surface (remove normal component)
    float dot = velocity.dot(normal);
    return velocity - normal * dot;
}

} // namespace engine
