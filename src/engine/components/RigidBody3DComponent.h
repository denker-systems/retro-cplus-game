/**
 * @file RigidBody3DComponent.h
 * @brief PhysX rigid body component for 3D actors
 * 
 * Follows same pattern as RigidBody2DComponent.
 * Provides 3D physics simulation using NVIDIA PhysX.
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <glm/glm.hpp>

namespace physx {
    class PxRigidDynamic;
    class PxRigidStatic;
    class PxRigidActor;
}

namespace engine {
namespace physics { class PhysicsWorld3D; }

/**
 * @brief 3D Rigid body component using PhysX
 * 
 * Usage:
 *   auto* rb = actor->addComponent<RigidBody3DComponent>();
 *   rb->setBodyType(RigidBody3DComponent::BodyType::Dynamic);
 *   rb->initializeBody(physicsWorld3D);
 */
class RigidBody3DComponent : public ActorComponent {
public:
    enum class BodyType {
        Static,     // Doesn't move (ground, walls)
        Dynamic,    // Full physics (player, enemies)
        Kinematic   // Controlled movement (moving platforms)
    };
    
    enum class ShapeType {
        Box,
        Sphere,
        Capsule
    };
    
    RigidBody3DComponent(const std::string& name = "RigidBody3DComponent");
    virtual ~RigidBody3DComponent();
    
    // ========================================================================
    // ACTORCOMPONENT LIFECYCLE
    // ========================================================================
    
    void initialize() override;
    void shutdown() override;
    void update(float deltaTime) override;
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    void initializeBody(physics::PhysicsWorld3D* world);
    bool isInitialized() const { return m_bodyInitialized; }
    
    // ========================================================================
    // BODY TYPE
    // ========================================================================
    
    void setBodyType(BodyType type);
    BodyType getBodyType() const { return m_bodyType; }
    
    // ========================================================================
    // SHAPE
    // ========================================================================
    
    void setShapeType(ShapeType type) { m_shapeType = type; }
    ShapeType getShapeType() const { return m_shapeType; }
    
    void setBoxExtents(const glm::vec3& halfExtents) { m_boxHalfExtents = halfExtents; }
    glm::vec3 getBoxExtents() const { return m_boxHalfExtents; }
    
    void setSphereRadius(float radius) { m_sphereRadius = radius; }
    float getSphereRadius() const { return m_sphereRadius; }
    
    void setCapsuleSize(float radius, float halfHeight) { 
        m_capsuleRadius = radius; 
        m_capsuleHalfHeight = halfHeight; 
    }
    
    // ========================================================================
    // PHYSICS PROPERTIES
    // ========================================================================
    
    void setMass(float mass);
    float getMass() const { return m_mass; }
    
    void setUseGravity(bool use);
    bool usesGravity() const { return m_useGravity; }
    
    void setLinearDamping(float damping);
    float getLinearDamping() const { return m_linearDamping; }
    
    void setAngularDamping(float damping);
    float getAngularDamping() const { return m_angularDamping; }
    
    // ========================================================================
    // VELOCITY & FORCES
    // ========================================================================
    
    void setVelocity(const glm::vec3& velocity);
    glm::vec3 getVelocity() const;
    
    void setAngularVelocity(const glm::vec3& velocity);
    glm::vec3 getAngularVelocity() const;
    
    void addForce(const glm::vec3& force);
    void addImpulse(const glm::vec3& impulse);
    void addTorque(const glm::vec3& torque);
    
    // ========================================================================
    // POSITION & ROTATION (synced with Actor)
    // ========================================================================
    
    glm::vec3 getPosition() const;
    void setPosition(const glm::vec3& position);
    
    glm::vec3 getRotation() const;  // Euler angles
    void setRotation(const glm::vec3& rotation);
    
    // ========================================================================
    // PHYSX ACCESS
    // ========================================================================
    
    physx::PxRigidActor* getActor() const { return m_actor; }
    physx::PxRigidDynamic* getDynamicActor() const;
    physics::PhysicsWorld3D* getWorld() const { return m_world; }
    
private:
    void createBody();
    void destroyBody();
    void syncActorFromBody();
    void syncBodyFromActor();
    void attachShape();
    
    physics::PhysicsWorld3D* m_world = nullptr;
    physx::PxRigidActor* m_actor = nullptr;
    bool m_bodyInitialized = false;
    
    // Body settings
    BodyType m_bodyType = BodyType::Dynamic;
    float m_mass = 1.0f;
    bool m_useGravity = true;
    float m_linearDamping = 0.0f;
    float m_angularDamping = 0.05f;
    
    // Shape settings
    ShapeType m_shapeType = ShapeType::Box;
    glm::vec3 m_boxHalfExtents{0.5f, 0.5f, 0.5f};
    float m_sphereRadius = 0.5f;
    float m_capsuleRadius = 0.5f;
    float m_capsuleHalfHeight = 0.5f;
    
    // Cached initial values
    glm::vec3 m_initialVelocity{0, 0, 0};
};

} // namespace engine
