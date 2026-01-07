/**
 * @file Collider2DComponent.h
 * @brief Box2D collider component for actors
 * 
 * Works together with RigidBody2DComponent.
 * Adds collision shapes to the rigid body.
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/physics/box2d/PhysicsWorld2D.h"
#include <glm/glm.hpp>

namespace engine {

class RigidBody2DComponent;

/**
 * @brief Collider component using Box2D shapes
 * 
 * Usage:
 *   auto* col = actor->addComponent<Collider2DComponent>();
 *   col->setBoxShape(32, 64);
 *   col->setCollisionLayer(CollisionLayer::Player);
 */
class Collider2DComponent : public ActorComponent {
public:
    enum class ShapeType {
        Box,
        Circle,
        Capsule
    };
    
    Collider2DComponent(const std::string& name = "Collider2DComponent");
    virtual ~Collider2DComponent();
    
    // ========================================================================
    // ACTORCOMPONENT LIFECYCLE
    // ========================================================================
    
    void initialize() override;
    void shutdown() override;
    void update(float deltaTime) override;
    
    // ========================================================================
    // SHAPE CONFIGURATION
    // ========================================================================
    
    void setBoxShape(float width, float height);
    void setCircleShape(float radius);
    void setCapsuleShape(float width, float height);
    
    void setOffset(glm::vec2 offset);
    glm::vec2 getOffset() const { return m_offset; }
    
    glm::vec2 getSize() const { return m_size; }
    ShapeType getShapeType() const { return m_shapeType; }
    
    // ========================================================================
    // PHYSICS MATERIAL
    // ========================================================================
    
    void setDensity(float density);
    float getDensity() const { return m_density; }
    
    void setFriction(float friction);
    float getFriction() const { return m_friction; }
    
    void setRestitution(float restitution);
    float getRestitution() const { return m_restitution; }
    
    // ========================================================================
    // COLLISION SETTINGS
    // ========================================================================
    
    void setTrigger(bool isTrigger);
    bool isTrigger() const { return m_isTrigger; }
    
    void setCollisionLayer(physics::CollisionCategory layer);
    physics::CollisionCategory getCollisionLayer() const { return m_layer; }
    
    void setCollisionMask(physics::CollisionCategory mask);
    physics::CollisionCategory getCollisionMask() const { return m_mask; }
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    void initializeShape();
    bool isInitialized() const { return m_shapeInitialized; }
    
private:
    void createShape();
    void destroyShape();
    RigidBody2DComponent* findRigidBody();
    
    RigidBody2DComponent* m_rigidBody = nullptr;
    b2ShapeId m_shapeId;
    bool m_shapeInitialized = false;
    
    // Shape properties
    ShapeType m_shapeType = ShapeType::Box;
    glm::vec2 m_size{32, 32};
    glm::vec2 m_offset{0, 0};
    
    // Physics material
    float m_density = 1.0f;
    float m_friction = 0.3f;
    float m_restitution = 0.0f;
    
    // Collision settings
    bool m_isTrigger = false;
    physics::CollisionCategory m_layer = physics::CollisionCategory::All;
    physics::CollisionCategory m_mask = physics::CollisionCategory::All;
};

} // namespace engine
