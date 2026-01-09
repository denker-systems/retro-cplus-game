/**
 * @file StaticMeshActor.h
 * @brief Static Mesh Actor with optional physics
 * 
 * A 3D mesh actor that can optionally have physics simulation.
 * Physics can be enabled/disabled via Properties panel.
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include "engine/components/MeshComponent.h"
#include "engine/components/RigidBody3DComponent.h"
#include "engine/physics/IPhysicsWorld.h"

namespace engine {

namespace physics { class PhysicsWorld3D; }

/**
 * @brief Static Mesh Actor with optional physics
 * 
 * Usage:
 *   auto actor = std::make_unique<StaticMeshActor>("Box");
 *   actor->setMeshPrimitive(PrimitiveMeshType::Cube);
 *   actor->setPhysicsEnabled(true);
 *   actor->setBodyType(physics::BodyType::Dynamic);
 */
class StaticMeshActor : public ActorObjectExtended {
public:
    StaticMeshActor(const std::string& name = "StaticMesh");
    virtual ~StaticMeshActor() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // ========================================================================
    // MESH
    // ========================================================================
    
    void setMeshPrimitive(PrimitiveMeshType type);
    PrimitiveMeshType getMeshPrimitive() const;
    
    void setMeshPath(const std::string& path);
    const std::string& getMeshPath() const;
    
    void setMeshColor(const glm::vec3& color);
    glm::vec3 getMeshColor() const;
    
    void setMeshScale(const glm::vec3& scale);
    glm::vec3 getMeshScale() const;
    
    MeshComponent* getMeshComponent() const { return m_meshComponent; }
    
    // ========================================================================
    // PHYSICS
    // ========================================================================
    
    void setPhysicsEnabled(bool enabled);
    bool isPhysicsEnabled() const { return m_physicsEnabled; }
    
    void setBodyType(physics::BodyType type);
    physics::BodyType getBodyType() const { return m_bodyType; }
    
    void setMass(float mass);
    float getMass() const { return m_mass; }
    
    void setUseGravity(bool use);
    bool usesGravity() const { return m_useGravity; }
    
    void initializePhysics(physics::PhysicsWorld3D* world);
    RigidBody3DComponent* getRigidBody() const { return m_rigidBody; }
    
    // ========================================================================
    // 3D TRANSFORM
    // ========================================================================
    
    void setPosition3D(const glm::vec3& pos);
    glm::vec3 getPosition3D() const { return m_position3D; }
    
    void setRotation3D(const glm::vec3& rot);
    glm::vec3 getRotation3D() const { return m_rotation3D; }
    
    // Physics sync (called during play mode)
    void syncFromPhysics();
    
private:
    void createPhysicsBody();
    void destroyPhysicsBody();
    
    // Components
    MeshComponent* m_meshComponent = nullptr;
    RigidBody3DComponent* m_rigidBody = nullptr;
    
    // Physics settings (cached for when physics is enabled)
    bool m_physicsEnabled = false;
    physics::BodyType m_bodyType = physics::BodyType::Static;
    float m_mass = 1.0f;
    bool m_useGravity = true;
    
    // 3D Transform
    glm::vec3 m_position3D{0.0f, 0.0f, 0.0f};
    glm::vec3 m_rotation3D{0.0f, 0.0f, 0.0f};
    
    // Physics world reference
    physics::PhysicsWorld3D* m_physicsWorld = nullptr;
};

} // namespace engine
