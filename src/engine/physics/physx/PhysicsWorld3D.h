/**
 * @file PhysicsWorld3D.h
 * @brief PhysX-based 3D physics world implementation
 * 
 * Inherits from IPhysicsWorld, provides 3D physics using NVIDIA PhysX.
 * Parallel to PhysicsWorld2D (Box2D) - both can coexist.
 */
#pragma once

#include "engine/physics/IPhysicsWorld.h"
#include <glm/glm.hpp>
#include <vector>
#include <memory>

// Forward declare PhysX types to avoid header pollution
namespace physx {
    class PxFoundation;
    class PxPhysics;
    class PxScene;
    class PxMaterial;
    class PxDefaultCpuDispatcher;
    class PxPvd;
    class PxCudaContextManager;
    class PxRigidDynamic;
    class PxRigidStatic;
    class PxShape;
    class PxRigidActor;
    class PxControllerManager;
}

namespace engine {
namespace physics {

/**
 * @brief 3D Physics world using NVIDIA PhysX
 * 
 * Usage:
 *   auto world = std::make_unique<PhysicsWorld3D>();
 *   world->initialize();
 *   world->step(deltaTime);
 */
class PhysicsWorld3D : public IPhysicsWorld {
public:
    PhysicsWorld3D();
    ~PhysicsWorld3D() override;
    
    // ========================================================================
    // IPhysicsWorld INTERFACE
    // ========================================================================
    
    void initialize() override;
    void shutdown() override;
    void step(float deltaTime) override;
    
    /**
     * @brief Enable 2D mode (lock Z-axis, disable Z-rotation)
     */
    void enable2DMode();
    
    /**
     * @brief Disable 2D mode (full 3D physics)
     */
    void disable2DMode();
    
    /**
     * @brief Check if 2D mode is enabled
     */
    bool is2DMode() const { return m_is2DMode; }
    
    PhysicsDimension getDimension() const override { return PhysicsDimension::Physics3D; }
    bool isInitialized() const override { return m_initialized; }
    
    void setGravity(glm::vec3 gravity) override;
    glm::vec3 getGravity() const override;
    
    void setDebugDraw(bool enabled) override { m_debugDrawEnabled = enabled; }
    bool isDebugDrawEnabled() const override { return m_debugDrawEnabled; }
    
    // ========================================================================
    // 3D-SPECIFIC: BODY CREATION
    // ========================================================================
    
    struct BodyDef3D {
        BodyType type = BodyType::Dynamic;
        glm::vec3 position{0, 0, 0};
        glm::vec3 rotation{0, 0, 0};  // Euler angles
        glm::vec3 linearVelocity{0, 0, 0};
        float mass = 1.0f;
        float linearDamping = 0.0f;
        float angularDamping = 0.05f;
        bool useGravity = true;
        void* userData = nullptr;
    };
    
    physx::PxRigidDynamic* createDynamicBody(const BodyDef3D& def);
    physx::PxRigidStatic* createStaticBody(const glm::vec3& position, void* userData = nullptr);
    void destroyBody(physx::PxRigidActor* body);
    
    // ========================================================================
    // 3D-SPECIFIC: SHAPES
    // ========================================================================
    
    struct ShapeDef3D {
        enum class Type { Box, Sphere, Capsule, Plane };
        Type type = Type::Box;
        glm::vec3 halfExtents{0.5f, 0.5f, 0.5f};  // For box
        float radius = 0.5f;                       // For sphere/capsule
        float halfHeight = 0.5f;                   // For capsule
        float staticFriction = 0.5f;
        float dynamicFriction = 0.5f;
        float restitution = 0.3f;
        bool isTrigger = false;
    };
    
    physx::PxShape* addBoxShape(physx::PxRigidActor* body, const glm::vec3& halfExtents);
    physx::PxShape* addSphereShape(physx::PxRigidActor* body, float radius);
    physx::PxShape* addCapsuleShape(physx::PxRigidActor* body, float radius, float halfHeight);
    
    // ========================================================================
    // 3D-SPECIFIC: QUERIES
    // ========================================================================
    
    glm::vec3 getBodyPosition(physx::PxRigidActor* body) const;
    void setBodyPosition(physx::PxRigidActor* body, const glm::vec3& position);
    
    glm::vec3 getBodyVelocity(physx::PxRigidDynamic* body) const;
    void setBodyVelocity(physx::PxRigidDynamic* body, const glm::vec3& velocity);
    
    void addForce(physx::PxRigidDynamic* body, const glm::vec3& force);
    void addImpulse(physx::PxRigidDynamic* body, const glm::vec3& impulse);
    
    // ========================================================================
    // 3D-SPECIFIC: RAYCASTING
    // ========================================================================
    
    struct RaycastResult3D {
        bool hit = false;
        glm::vec3 point{0, 0, 0};
        glm::vec3 normal{0, 0, 0};
        float distance = 0.0f;
        physx::PxRigidActor* actor = nullptr;
    };
    
    RaycastResult3D raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const;
    
    // ========================================================================
    // GPU ACCELERATION
    // ========================================================================
    
    bool isGPUAccelerationAvailable() const { return m_cudaContext != nullptr; }
    
    // ========================================================================
    // PHYSX ACCESS (for advanced usage)
    // ========================================================================
    
    physx::PxScene* getScene() const { return m_scene; }
    physx::PxPhysics* getPhysics() const { return m_physics; }
    physx::PxControllerManager* getControllerManager();
    physx::PxMaterial* getDefaultMaterial() const { return m_defaultMaterial; }
    
private:
    void initializeFoundation();
    void initializeCuda();
    void initializePhysics();
    void initializeScene();
    void initializePVD();  // PhysX Visual Debugger
    
    // PhysX core
    physx::PxFoundation* m_foundation = nullptr;
    physx::PxPhysics* m_physics = nullptr;
    physx::PxScene* m_scene = nullptr;
    physx::PxDefaultCpuDispatcher* m_dispatcher = nullptr;
    physx::PxMaterial* m_defaultMaterial = nullptr;
    
    // Optional
    physx::PxPvd* m_pvd = nullptr;
    physx::PxCudaContextManager* m_cudaContext = nullptr;
    physx::PxControllerManager* m_controllerManager = nullptr;
    
    // 2D mode
    bool m_is2DMode = false;
    
    // State
    bool m_initialized = false;
    bool m_debugDrawEnabled = false;
    glm::vec3 m_gravity{0.0f, -9.81f, 0.0f};
    
    // Track bodies for cleanup
    std::vector<physx::PxRigidActor*> m_bodies;
};

} // namespace physics
} // namespace engine
