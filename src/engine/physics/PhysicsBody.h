/**
 * @file PhysicsBody.h
 * @brief Physics body with velocity and collision
 */
#pragma once

#include "core/Node2D.h"
#include "CollisionShape.h"
#include <vector>
#include <memory>
#include <functional>

namespace engine {

/**
 * @brief Physics body with velocity and collision
 */
class PhysicsBody : public Node2D {
public:
    PhysicsBody();
    explicit PhysicsBody(const std::string& name);
    virtual ~PhysicsBody() = default;
    
    void update(float deltaTime) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // PHYSICS PROPERTIES
    // ═══════════════════════════════════════════════════════════════════
    
    const Vec2& getVelocity() const { return m_velocity; }
    void setVelocity(const Vec2& vel) { m_velocity = vel; }
    void setVelocity(float x, float y) { m_velocity = Vec2(x, y); }
    
    const Vec2& getAcceleration() const { return m_acceleration; }
    void setAcceleration(const Vec2& acc) { m_acceleration = acc; }
    void setAcceleration(float x, float y) { m_acceleration = Vec2(x, y); }
    
    float getFriction() const { return m_friction; }
    void setFriction(float friction) { m_friction = friction; }
    
    float getMass() const { return m_mass; }
    void setMass(float mass) { m_mass = mass; }
    
    bool isStatic() const { return m_isStatic; }
    void setStatic(bool isStatic) { m_isStatic = isStatic; }
    
    // ═══════════════════════════════════════════════════════════════════
    // COLLISION SHAPES
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Add collision shape */
    void addShape(std::unique_ptr<CollisionShape> shape);
    
    /** @brief Get all shapes */
    const std::vector<std::unique_ptr<CollisionShape>>& getShapes() const { return m_shapes; }
    
    /** @brief Check collision with another body */
    bool collidesWith(const PhysicsBody* other) const;
    
    // ═══════════════════════════════════════════════════════════════════
    // COLLISION CALLBACKS
    // ═══════════════════════════════════════════════════════════════════
    
    std::function<void(PhysicsBody*)> onCollisionEnter;
    std::function<void(PhysicsBody*)> onCollisionStay;
    std::function<void(PhysicsBody*)> onCollisionExit;
    
protected:
    virtual void applyPhysics(float deltaTime);
    
    Vec2 m_velocity{0, 0};
    Vec2 m_acceleration{0, 0};
    float m_friction = 0.9f;
    float m_mass = 1.0f;
    bool m_isStatic = false;
    
    std::vector<std::unique_ptr<CollisionShape>> m_shapes;
};

} // namespace engine
