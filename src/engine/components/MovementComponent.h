/**
 * @file MovementComponent.h
 * @brief Movement Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/core/Vec2.h"
#include <functional>

namespace engine {

/**
 * @brief Component for movement logic
 */
class MovementComponent : public ActorComponent {
public:
    MovementComponent(const std::string& name = "MovementComponent");
    virtual ~MovementComponent() = default;
    
    void setVelocity(Vec2 vel) { m_velocity = vel; }
    Vec2 getVelocity() const { return m_velocity; }
    
    void setMaxSpeed(float speed) { m_maxSpeed = speed; }
    float getMaxSpeed() const { return m_maxSpeed; }
    
    void setFriction(float friction) { m_friction = friction; }
    float getFriction() const { return m_friction; }
    
    // Point-and-click movement
    void setTarget(const Vec2& target);
    const Vec2& getTarget() const { return m_target; }
    bool hasTarget() const { return m_hasTarget; }
    bool isMoving() const { return m_hasTarget || m_velocity.length() > 1.0f; }
    
    // Walk area constraints
    void setWalkArea(float minX, float maxX, float minY, float maxY);
    void clearWalkArea();
    
    void addForce(Vec2 force);
    void stop();
    
    void update(float deltaTime) override;
    
    // Events
    std::function<void(const Vec2& target)> onMovementStart;
    std::function<void()> onMovementComplete;
    
private:
    Vec2 m_velocity{0, 0};
    Vec2 m_acceleration{0, 0};
    float m_maxSpeed = 200.0f;
    float m_friction = 0.9f;
    
    // Point-and-click movement
    Vec2 m_target{0, 0};
    bool m_hasTarget = false;
    
    // Walk area constraints
    bool m_hasWalkArea = false;
    float m_walkMinX = 0, m_walkMaxX = 640;
    float m_walkMinY = 0, m_walkMaxY = 400;
};

} // namespace engine
