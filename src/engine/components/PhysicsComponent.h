/**
 * @file PhysicsComponent.h
 * @brief Physics Component (UE5-style)
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/core/Vec2.h"

namespace engine {

/**
 * @brief Component for physics simulation
 * 
 * Like UPrimitiveComponent with physics in UE5.
 * Handles velocity, acceleration, friction.
 */
class PhysicsComponent : public ActorComponent {
public:
    PhysicsComponent(const std::string& name = "PhysicsComponent");
    virtual ~PhysicsComponent() = default;
    
    void update(float deltaTime) override;
    
    // ========================================================================
    // PHYSICS PROPERTIES
    // ========================================================================
    
    const Vec2& getVelocity() const { return m_velocity; }
    void setVelocity(Vec2 vel) { m_velocity = vel; }
    
    const Vec2& getAcceleration() const { return m_acceleration; }
    void setAcceleration(Vec2 acc) { m_acceleration = acc; }
    
    float getFriction() const { return m_friction; }
    void setFriction(float friction) { m_friction = friction; }
    
    float getGravity() const { return m_gravity; }
    void setGravity(float gravity) { m_gravity = gravity; }
    
    float getMass() const { return m_mass; }
    void setMass(float mass) { m_mass = mass; }
    
    // ========================================================================
    // FORCES
    // ========================================================================
    
    void addForce(Vec2 force);
    void addImpulse(Vec2 impulse);
    void stop();
    
    // ========================================================================
    // STATE
    // ========================================================================
    
    bool isOnGround() const { return m_onGround; }
    void setOnGround(bool onGround) { m_onGround = onGround; }
    
private:
    Vec2 m_velocity{0, 0};
    Vec2 m_acceleration{0, 0};
    Vec2 m_accumulatedForces{0, 0};
    
    float m_friction = 0.9f;
    float m_gravity = 980.0f;
    float m_mass = 1.0f;
    
    bool m_onGround = false;
};

} // namespace engine
