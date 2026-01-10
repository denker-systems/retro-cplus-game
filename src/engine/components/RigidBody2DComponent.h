/**
 * @file RigidBody2DComponent.h
 * @brief DEPRECATED: Legacy 2D physics body component
 * 
 * USE INSTEAD: RigidBody3DComponent (PhysX-based 3D physics)
 * 
 * This component uses Box2D for 2D physics.
 * All new code should use RigidBody3DComponent with PhysX.
 * 
 * @deprecated Use RigidBody3DComponent for all new code
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/physics/box2d/PhysicsWorld2D.h"
#include "engine/core/Vec2.h"
#include <glm/glm.hpp>

namespace engine {

namespace physics { class PhysicsWorld2D; }

/**
 * @brief Rigid body component using Box2D
 * 
 * Usage:
 *   auto* rb = actor->addComponent<RigidBody2DComponent>();
 *   rb->setBodyType(RigidBody2DComponent::BodyType::Dynamic);
 *   rb->setFixedRotation(true);
 */
class RigidBody2DComponent : public ActorComponent {
public:
    enum class BodyType {
        Static,     // Doesn't move (ground, walls)
        Dynamic,    // Full physics (player, enemies)
        Kinematic   // Controlled movement (moving platforms)
    };
    
    RigidBody2DComponent(const std::string& name = "RigidBody2DComponent");
    virtual ~RigidBody2DComponent();
    
    // ========================================================================
    // ACTORCOMPONENT LIFECYCLE
    // ========================================================================
    
    void initialize() override;
    void shutdown() override;
    void update(float deltaTime) override;
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    void initializeBody(physics::PhysicsWorld2D* world);
    bool isInitialized() const { return m_bodyInitialized; }
    
    // ========================================================================
    // BODY TYPE
    // ========================================================================
    
    void setBodyType(BodyType type);
    BodyType getBodyType() const { return m_bodyType; }
    
    // ========================================================================
    // PHYSICS PROPERTIES
    // ========================================================================
    
    void setGravityScale(float scale);
    float getGravityScale() const { return m_gravityScale; }
    
    void setFixedRotation(bool fixed);
    bool isFixedRotation() const { return m_fixedRotation; }
    
    void setLinearDamping(float damping);
    float getLinearDamping() const { return m_linearDamping; }
    
    // ========================================================================
    // VELOCITY & FORCES
    // ========================================================================
    
    void setVelocity(glm::vec2 velocity);
    glm::vec2 getVelocity() const;
    
    void setVelocityX(float vx);
    void setVelocityY(float vy);
    
    void addForce(glm::vec2 force);
    void addImpulse(glm::vec2 impulse);
    
    // ========================================================================
    // POSITION (synced with Actor)
    // ========================================================================
    
    glm::vec2 getPosition() const;
    void setPosition(glm::vec2 position);
    
    // ========================================================================
    // BOX2D ACCESS (for advanced usage)
    // ========================================================================
    
    b2BodyId getBodyId() const { return m_bodyId; }
    physics::PhysicsWorld2D* getWorld() const { return m_world; }
    
private:
    void createBody();
    void destroyBody();
    void syncActorFromBody();
    void syncBodyFromActor();
    
    physics::PhysicsWorld2D* m_world = nullptr;
    b2BodyId m_bodyId;
    bool m_bodyInitialized = false;
    
    // Cached properties (applied when body is created)
    BodyType m_bodyType = BodyType::Dynamic;
    float m_gravityScale = 1.0f;
    bool m_fixedRotation = true;
    float m_linearDamping = 0.0f;
    glm::vec2 m_initialVelocity{0, 0};
};

} // namespace engine
