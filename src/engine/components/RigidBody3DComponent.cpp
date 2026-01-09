/**
 * @file RigidBody3DComponent.cpp
 * @brief PhysX rigid body component implementation
 */
#include "RigidBody3DComponent.h"
#include "engine/physics/physx/PhysicsWorld3D.h"
#include "engine/core/ActorObjectExtended.h"

#include <PxPhysicsAPI.h>
#include <iostream>

using namespace physx;

namespace engine {

RigidBody3DComponent::RigidBody3DComponent(const std::string& name)
    : ActorComponent(name)
{
}

RigidBody3DComponent::~RigidBody3DComponent() {
    destroyBody();
}

void RigidBody3DComponent::initialize() {
    // Body created when initializeBody() is called with world
}

void RigidBody3DComponent::shutdown() {
    destroyBody();
}

void RigidBody3DComponent::update(float deltaTime) {
    if (m_bodyInitialized && m_bodyType == BodyType::Dynamic) {
        syncActorFromBody();
    }
}

void RigidBody3DComponent::initializeBody(physics::PhysicsWorld3D* world) {
    if (m_bodyInitialized || !world) return;
    
    m_world = world;
    createBody();
}

void RigidBody3DComponent::createBody() {
    if (!m_world || !m_world->isInitialized()) return;
    
    auto* owner = getOwner();
    // Convert from 2D actor coords (pixels) to 3D physics coords (meters)
    // 2D X → physics X, actor Z (height) → physics Y, 2D Y → physics Z
    glm::vec3 position = owner ? glm::vec3(
        owner->getX() / 100.0f,    // 2D X → physics X
        owner->getZ() / 100.0f,    // actor Z (height) → physics Y  
        owner->getY() / 100.0f     // 2D Y → physics Z
    ) : glm::vec3(0);
    
    if (m_bodyType == BodyType::Static) {
        m_actor = m_world->createStaticBody(position, owner);
    } else {
        physics::PhysicsWorld3D::BodyDef3D def;
        def.type = m_bodyType == BodyType::Dynamic ? physics::BodyType::Dynamic : physics::BodyType::Kinematic;
        def.position = position;
        def.mass = m_mass;
        def.linearDamping = m_linearDamping;
        def.angularDamping = m_angularDamping;
        def.useGravity = m_useGravity;
        def.linearVelocity = m_initialVelocity;
        def.userData = owner;
        
        m_actor = m_world->createDynamicBody(def);
    }
    
    if (m_actor) {
        attachShape();
        m_bodyInitialized = true;
    }
}

void RigidBody3DComponent::attachShape() {
    if (!m_actor || !m_world) return;
    
    switch (m_shapeType) {
        case ShapeType::Box:
            m_world->addBoxShape(m_actor, m_boxHalfExtents);
            break;
        case ShapeType::Sphere:
            m_world->addSphereShape(m_actor, m_sphereRadius);
            break;
        case ShapeType::Capsule:
            m_world->addCapsuleShape(m_actor, m_capsuleRadius, m_capsuleHalfHeight);
            break;
    }
}

void RigidBody3DComponent::destroyBody() {
    if (m_actor && m_world) {
        m_world->destroyBody(m_actor);
        m_actor = nullptr;
    }
    m_bodyInitialized = false;
}

void RigidBody3DComponent::syncActorFromBody() {
    if (!m_actor || !m_bodyInitialized) return;
    
    auto* owner = getOwner();
    if (!owner) return;
    
    glm::vec3 pos = m_world->getBodyPosition(m_actor);
    owner->setPosition(pos.x, pos.y);
    // TODO: Sync 3D position when Actor supports it
}

void RigidBody3DComponent::syncBodyFromActor() {
    if (!m_actor || !m_bodyInitialized) return;
    
    auto* owner = getOwner();
    if (!owner) return;
    
    glm::vec3 pos(owner->getX(), owner->getY(), 0.0f);
    m_world->setBodyPosition(m_actor, pos);
}

// ============================================================================
// BODY TYPE
// ============================================================================

void RigidBody3DComponent::setBodyType(BodyType type) {
    m_bodyType = type;
    // If already initialized, would need to recreate body
}

// ============================================================================
// PHYSICS PROPERTIES
// ============================================================================

void RigidBody3DComponent::setMass(float mass) {
    m_mass = mass;
    if (auto* dynamic = getDynamicActor()) {
        PxRigidBodyExt::setMassAndUpdateInertia(*dynamic, mass);
    }
}

void RigidBody3DComponent::setUseGravity(bool use) {
    m_useGravity = use;
    if (m_actor) {
        m_actor->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !use);
    }
}

void RigidBody3DComponent::setLinearDamping(float damping) {
    m_linearDamping = damping;
    if (auto* dynamic = getDynamicActor()) {
        dynamic->setLinearDamping(damping);
    }
}

void RigidBody3DComponent::setAngularDamping(float damping) {
    m_angularDamping = damping;
    if (auto* dynamic = getDynamicActor()) {
        dynamic->setAngularDamping(damping);
    }
}

// ============================================================================
// VELOCITY & FORCES
// ============================================================================

void RigidBody3DComponent::setVelocity(const glm::vec3& velocity) {
    if (auto* dynamic = getDynamicActor()) {
        m_world->setBodyVelocity(dynamic, velocity);
    } else {
        m_initialVelocity = velocity;
    }
}

glm::vec3 RigidBody3DComponent::getVelocity() const {
    if (auto* dynamic = getDynamicActor()) {
        return m_world->getBodyVelocity(dynamic);
    }
    return m_initialVelocity;
}

void RigidBody3DComponent::setAngularVelocity(const glm::vec3& velocity) {
    if (auto* dynamic = getDynamicActor()) {
        dynamic->setAngularVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
    }
}

glm::vec3 RigidBody3DComponent::getAngularVelocity() const {
    if (auto* dynamic = getDynamicActor()) {
        PxVec3 vel = dynamic->getAngularVelocity();
        return glm::vec3(vel.x, vel.y, vel.z);
    }
    return glm::vec3(0);
}

void RigidBody3DComponent::addForce(const glm::vec3& force) {
    if (auto* dynamic = getDynamicActor()) {
        m_world->addForce(dynamic, force);
    }
}

void RigidBody3DComponent::addImpulse(const glm::vec3& impulse) {
    if (auto* dynamic = getDynamicActor()) {
        m_world->addImpulse(dynamic, impulse);
    }
}

void RigidBody3DComponent::addTorque(const glm::vec3& torque) {
    if (auto* dynamic = getDynamicActor()) {
        dynamic->addTorque(PxVec3(torque.x, torque.y, torque.z));
    }
}

// ============================================================================
// POSITION & ROTATION
// ============================================================================

glm::vec3 RigidBody3DComponent::getPosition() const {
    if (m_actor && m_world) {
        return m_world->getBodyPosition(m_actor);
    }
    return glm::vec3(0);
}

void RigidBody3DComponent::setPosition(const glm::vec3& position) {
    if (m_actor && m_world) {
        m_world->setBodyPosition(m_actor, position);
    }
}

glm::vec3 RigidBody3DComponent::getRotation() const {
    if (!m_actor) return glm::vec3(0);
    
    PxTransform transform = m_actor->getGlobalPose();
    // Convert quaternion to euler (simplified)
    PxQuat q = transform.q;
    
    float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
    float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
    float roll = std::atan2(sinr_cosp, cosr_cosp);
    
    float sinp = 2.0f * (q.w * q.y - q.z * q.x);
    float pitch = std::abs(sinp) >= 1.0f ? std::copysign(3.14159f / 2.0f, sinp) : std::asin(sinp);
    
    float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
    float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
    float yaw = std::atan2(siny_cosp, cosy_cosp);
    
    return glm::vec3(roll, pitch, yaw);
}

void RigidBody3DComponent::setRotation(const glm::vec3& rotation) {
    if (!m_actor) return;
    
    PxTransform transform = m_actor->getGlobalPose();
    
    // Convert euler to quaternion
    float cy = std::cos(rotation.z * 0.5f);
    float sy = std::sin(rotation.z * 0.5f);
    float cp = std::cos(rotation.y * 0.5f);
    float sp = std::sin(rotation.y * 0.5f);
    float cr = std::cos(rotation.x * 0.5f);
    float sr = std::sin(rotation.x * 0.5f);
    
    transform.q = PxQuat(
        sr * cp * cy - cr * sp * sy,
        cr * sp * cy + sr * cp * sy,
        cr * cp * sy - sr * sp * cy,
        cr * cp * cy + sr * sp * sy
    );
    
    m_actor->setGlobalPose(transform);
}

// ============================================================================
// PHYSX ACCESS
// ============================================================================

PxRigidDynamic* RigidBody3DComponent::getDynamicActor() const {
    if (!m_actor) return nullptr;
    return m_actor->is<PxRigidDynamic>();
}

} // namespace engine
