/**
 * @file StaticMeshActor.cpp
 * @brief Static Mesh Actor implementation
 */
#include "StaticMeshActor.h"
#include "engine/physics/physx/PhysicsWorld3D.h"
#include "engine/utils/Logger.h"

namespace engine {

StaticMeshActor::StaticMeshActor(const std::string& name)
    : ActorObjectExtended(name)
{
    // Create mesh component
    m_meshComponent = addComponent<MeshComponent>();
    m_meshComponent->setPrimitive(PrimitiveMeshType::Cube);
    
    LOG_DEBUG("[StaticMeshActor] Created: " + name);
}

void StaticMeshActor::update(float deltaTime) {
    ActorObjectExtended::update(deltaTime);
    
    // Sync position from physics if enabled
    if (m_physicsEnabled && m_rigidBody && m_rigidBody->isInitialized()) {
        syncFromPhysics();
    }
}

void StaticMeshActor::render(SDL_Renderer* renderer) {
    // 3D rendering handled by Viewport3DPanel
}

// ============================================================================
// MESH
// ============================================================================

void StaticMeshActor::setMeshPrimitive(PrimitiveMeshType type) {
    if (m_meshComponent) {
        m_meshComponent->setPrimitive(type);
    }
}

PrimitiveMeshType StaticMeshActor::getMeshPrimitive() const {
    return m_meshComponent ? m_meshComponent->getPrimitive() : PrimitiveMeshType::Cube;
}

void StaticMeshActor::setMeshPath(const std::string& path) {
    if (m_meshComponent) {
        m_meshComponent->setMeshPath(path);
    }
}

const std::string& StaticMeshActor::getMeshPath() const {
    static std::string empty;
    return m_meshComponent ? m_meshComponent->getMeshPath() : empty;
}

void StaticMeshActor::setMeshColor(const glm::vec3& color) {
    if (m_meshComponent) {
        m_meshComponent->setColor(color);
    }
}

glm::vec3 StaticMeshActor::getMeshColor() const {
    return m_meshComponent ? m_meshComponent->getColor() : glm::vec3(0.8f);
}

void StaticMeshActor::setMeshScale(const glm::vec3& scale) {
    if (m_meshComponent) {
        m_meshComponent->setScale(scale);
    }
}

glm::vec3 StaticMeshActor::getMeshScale() const {
    return m_meshComponent ? m_meshComponent->getScale() : glm::vec3(1.0f);
}

// ============================================================================
// PHYSICS
// ============================================================================

void StaticMeshActor::setPhysicsEnabled(bool enabled) {
    if (m_physicsEnabled == enabled) return;
    
    m_physicsEnabled = enabled;
    
    if (enabled) {
        createPhysicsBody();
    } else {
        destroyPhysicsBody();
    }
    
    LOG_INFO("[StaticMeshActor] Physics " + std::string(enabled ? "enabled" : "disabled") + 
             " for: " + getName());
}

void StaticMeshActor::setBodyType(physics::BodyType type) {
    m_bodyType = type;
    
    if (m_rigidBody) {
        m_rigidBody->setBodyType(static_cast<RigidBody3DComponent::BodyType>(type));
    }
}

void StaticMeshActor::setMass(float mass) {
    m_mass = mass;
    
    if (m_rigidBody) {
        m_rigidBody->setMass(mass);
    }
}

void StaticMeshActor::setUseGravity(bool use) {
    m_useGravity = use;
    
    if (m_rigidBody) {
        m_rigidBody->setUseGravity(use);
    }
}

void StaticMeshActor::initializePhysics(physics::PhysicsWorld3D* world) {
    m_physicsWorld = world;
    
    if (m_physicsEnabled && !m_rigidBody) {
        createPhysicsBody();
    }
    
    if (m_rigidBody && world) {
        m_rigidBody->initializeBody(world);
        LOG_INFO("[StaticMeshActor] Physics initialized for: " + getName());
    }
}

void StaticMeshActor::createPhysicsBody() {
    if (m_rigidBody) return;  // Already exists
    
    m_rigidBody = addComponent<RigidBody3DComponent>();
    m_rigidBody->setBodyType(static_cast<RigidBody3DComponent::BodyType>(m_bodyType));
    m_rigidBody->setMass(m_mass);
    m_rigidBody->setUseGravity(m_useGravity);
    
    // Set shape based on mesh
    if (m_meshComponent) {
        switch (m_meshComponent->getPrimitive()) {
            case PrimitiveMeshType::Sphere:
                m_rigidBody->setShapeType(RigidBody3DComponent::ShapeType::Sphere);
                m_rigidBody->setSphereRadius(m_meshComponent->getRadius());
                break;
            case PrimitiveMeshType::Capsule:
            case PrimitiveMeshType::Cylinder:
                m_rigidBody->setShapeType(RigidBody3DComponent::ShapeType::Capsule);
                m_rigidBody->setCapsuleSize(
                    m_meshComponent->getScale().x * 0.5f,
                    m_meshComponent->getScale().y * 0.5f
                );
                break;
            default:
                m_rigidBody->setShapeType(RigidBody3DComponent::ShapeType::Box);
                m_rigidBody->setBoxExtents(m_meshComponent->getHalfExtents());
                break;
        }
    }
    
    // Initialize if we have a world
    if (m_physicsWorld) {
        m_rigidBody->initializeBody(m_physicsWorld);
    }
    
    LOG_DEBUG("[StaticMeshActor] Physics body created for: " + getName());
}

void StaticMeshActor::destroyPhysicsBody() {
    if (!m_rigidBody) return;
    
    m_rigidBody->shutdown();
    removeComponent<RigidBody3DComponent>();
    m_rigidBody = nullptr;
    
    LOG_DEBUG("[StaticMeshActor] Physics body destroyed for: " + getName());
}

void StaticMeshActor::syncFromPhysics() {
    if (!m_rigidBody || !m_rigidBody->isInitialized()) return;
    
    m_position3D = m_rigidBody->getPosition();
    m_rotation3D = m_rigidBody->getRotation();
    
    // Sync to 2D/viewport coordinates:
    // Physics X → 2D X (scaled to pixels)
    // Physics Y → actor Z (height, scaled to pixels)
    // Physics Z → 2D Y (depth, scaled to pixels)
    setPosition(m_position3D.x * 100.0f, m_position3D.z * 100.0f);
    setZ(m_position3D.y * 100.0f);
}

// ============================================================================
// 3D TRANSFORM
// ============================================================================

void StaticMeshActor::setPosition3D(const glm::vec3& pos) {
    m_position3D = pos;
    // Sync to 2D: X→X, Z→Y (depth), Y→Z (height)
    setPosition(pos.x * 100.0f, pos.z * 100.0f);
    setZ(pos.y * 100.0f);
    
    if (m_rigidBody && m_rigidBody->isInitialized()) {
        m_rigidBody->setPosition(pos);
    }
}

void StaticMeshActor::setRotation3D(const glm::vec3& rot) {
    m_rotation3D = rot;
    
    if (m_rigidBody && m_rigidBody->isInitialized()) {
        m_rigidBody->setRotation(rot);
    }
}

} // namespace engine
