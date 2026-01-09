/**
 * @file PhysicsWorld3D.cpp
 * @brief PhysX-based 3D physics world implementation
 */
#include "PhysicsWorld3D.h"

// PhysX includes
#include <PxPhysicsAPI.h>
#include <characterkinematic/PxControllerManager.h>
#include <iostream>

using namespace physx;

namespace engine {
namespace physics {

// ============================================================================
// PHYSX ERROR CALLBACK
// ============================================================================

class PhysXErrorCallback : public PxErrorCallback {
public:
    void reportError(PxErrorCode::Enum code, const char* message, const char* file, int line) override {
        const char* errorType = "Unknown";
        switch (code) {
            case PxErrorCode::eNO_ERROR: return;
            case PxErrorCode::eDEBUG_INFO: errorType = "Info"; break;
            case PxErrorCode::eDEBUG_WARNING: errorType = "Warning"; break;
            case PxErrorCode::eINVALID_PARAMETER: errorType = "Invalid Parameter"; break;
            case PxErrorCode::eINVALID_OPERATION: errorType = "Invalid Operation"; break;
            case PxErrorCode::eOUT_OF_MEMORY: errorType = "Out of Memory"; break;
            case PxErrorCode::eINTERNAL_ERROR: errorType = "Internal Error"; break;
            case PxErrorCode::eABORT: errorType = "Abort"; break;
            case PxErrorCode::ePERF_WARNING: errorType = "Performance Warning"; break;
            default: break;
        }
        std::cerr << "[PhysX " << errorType << "] " << message << " (" << file << ":" << line << ")" << std::endl;
    }
};

static PhysXErrorCallback gErrorCallback;
static PxDefaultAllocator gAllocator;

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

PhysicsWorld3D::PhysicsWorld3D() = default;

PhysicsWorld3D::~PhysicsWorld3D() {
    shutdown();
}

// ============================================================================
// LIFECYCLE
// ============================================================================

void PhysicsWorld3D::initialize() {
    if (m_initialized) return;
    
    std::cout << "[PhysicsWorld3D] Initializing PhysX..." << std::endl;
    
    initializeFoundation();
    initializePVD();
    initializeCuda();
    initializePhysics();
    initializeScene();
    
    m_initialized = true;
    std::cout << "[PhysicsWorld3D] Initialized successfully" 
              << (m_cudaContext ? " (GPU acceleration enabled)" : " (CPU only)")
              << std::endl;
}

void PhysicsWorld3D::initializeFoundation() {
    m_foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
    if (!m_foundation) {
        std::cerr << "[PhysicsWorld3D] PxCreateFoundation failed!" << std::endl;
        return;
    }
}

void PhysicsWorld3D::initializePVD() {
#ifdef _DEBUG
    m_pvd = PxCreatePvd(*m_foundation);
    if (m_pvd) {
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        m_pvd->connect(*transport, PxPvdInstrumentationFlag::eALL);
        std::cout << "[PhysicsWorld3D] PVD connection attempted (localhost:5425)" << std::endl;
    }
#endif
}

void PhysicsWorld3D::initializeCuda() {
#if PX_SUPPORT_GPU_PHYSX
    PxCudaContextManagerDesc cudaDesc;
    m_cudaContext = PxCreateCudaContextManager(*m_foundation, cudaDesc, PxGetProfilerCallback());
    if (m_cudaContext && !m_cudaContext->contextIsValid()) {
        m_cudaContext->release();
        m_cudaContext = nullptr;
        std::cout << "[PhysicsWorld3D] CUDA context invalid, using CPU" << std::endl;
    }
#endif
}

void PhysicsWorld3D::initializePhysics() {
    PxTolerancesScale scale;
    scale.length = 1.0f;   // 1 unit = 1 meter
    scale.speed = 10.0f;   // Typical speed
    
    m_physics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_foundation, scale, true, m_pvd);
    if (!m_physics) {
        std::cerr << "[PhysicsWorld3D] PxCreatePhysics failed!" << std::endl;
        return;
    }
    
    // Create default material
    m_defaultMaterial = m_physics->createMaterial(0.5f, 0.5f, 0.3f);
}

void PhysicsWorld3D::initializeScene() {
    PxSceneDesc sceneDesc(m_physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(m_gravity.x, m_gravity.y, m_gravity.z);
    
    // CPU dispatcher
    m_dispatcher = PxDefaultCpuDispatcherCreate(4);  // 4 threads
    sceneDesc.cpuDispatcher = m_dispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
    
    // GPU acceleration if available
#if PX_SUPPORT_GPU_PHYSX
    if (m_cudaContext) {
        sceneDesc.cudaContextManager = m_cudaContext;
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
        sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;
    }
#endif
    
    m_scene = m_physics->createScene(sceneDesc);
    if (!m_scene) {
        std::cerr << "[PhysicsWorld3D] createScene failed!" << std::endl;
        return;
    }
    
    // Enable PVD client
#ifdef _DEBUG
    PxPvdSceneClient* pvdClient = m_scene->getScenePvdClient();
    if (pvdClient) {
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
        pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
    }
#endif
}

void PhysicsWorld3D::shutdown() {
    if (!m_initialized) return;
    
    std::cout << "[PhysicsWorld3D] Shutting down..." << std::endl;
    
    // Destroy tracked bodies
    for (auto* body : m_bodies) {
        if (body) {
            m_scene->removeActor(*body);
            body->release();
        }
    }
    m_bodies.clear();
    
    // Release controller manager first
    if (m_controllerManager) { m_controllerManager->release(); m_controllerManager = nullptr; }
    
    // Release PhysX objects in order
    if (m_scene) { m_scene->release(); m_scene = nullptr; }
    if (m_dispatcher) { m_dispatcher->release(); m_dispatcher = nullptr; }
    if (m_defaultMaterial) { m_defaultMaterial->release(); m_defaultMaterial = nullptr; }
    if (m_physics) { m_physics->release(); m_physics = nullptr; }
    if (m_pvd) { m_pvd->release(); m_pvd = nullptr; }
    if (m_cudaContext) { m_cudaContext->release(); m_cudaContext = nullptr; }
    if (m_foundation) { m_foundation->release(); m_foundation = nullptr; }
    
    m_initialized = false;
    std::cout << "[PhysicsWorld3D] Shutdown complete" << std::endl;
}

void PhysicsWorld3D::step(float deltaTime) {
    if (!m_initialized || !m_scene) return;
    
    // Clamp delta time
    float clampedDt = std::min(deltaTime, 1.0f / 30.0f);
    
    m_scene->simulate(clampedDt);
    m_scene->fetchResults(true);
}

// ============================================================================
// PROPERTIES
// ============================================================================

void PhysicsWorld3D::setGravity(glm::vec3 gravity) {
    m_gravity = gravity;
    if (m_scene) {
        m_scene->setGravity(PxVec3(gravity.x, gravity.y, gravity.z));
    }
}

glm::vec3 PhysicsWorld3D::getGravity() const {
    return m_gravity;
}

PxControllerManager* PhysicsWorld3D::getControllerManager() {
    if (!m_scene) return nullptr;
    
    // Create on demand
    if (!m_controllerManager) {
        m_controllerManager = PxCreateControllerManager(*m_scene);
        if (m_controllerManager) {
            std::cout << "[PhysicsWorld3D] Controller manager created" << std::endl;
        }
    }
    return m_controllerManager;
}

// ============================================================================
// BODY CREATION
// ============================================================================

PxRigidDynamic* PhysicsWorld3D::createDynamicBody(const BodyDef3D& def) {
    if (!m_physics || !m_scene) return nullptr;
    
    PxTransform transform(PxVec3(def.position.x, def.position.y, def.position.z));
    PxRigidDynamic* body = m_physics->createRigidDynamic(transform);
    
    if (body) {
        body->setMass(def.mass);
        body->setLinearDamping(def.linearDamping);
        body->setAngularDamping(def.angularDamping);
        body->setLinearVelocity(PxVec3(def.linearVelocity.x, def.linearVelocity.y, def.linearVelocity.z));
        body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, !def.useGravity);
        body->userData = def.userData;
        
        m_scene->addActor(*body);
        m_bodies.push_back(body);
    }
    
    return body;
}

PxRigidStatic* PhysicsWorld3D::createStaticBody(const glm::vec3& position, void* userData) {
    if (!m_physics || !m_scene) return nullptr;
    
    PxTransform transform(PxVec3(position.x, position.y, position.z));
    PxRigidStatic* body = m_physics->createRigidStatic(transform);
    
    if (body) {
        body->userData = userData;
        m_scene->addActor(*body);
        m_bodies.push_back(body);
    }
    
    return body;
}

void PhysicsWorld3D::destroyBody(PxRigidActor* body) {
    if (!body || !m_scene) return;
    
    auto it = std::find(m_bodies.begin(), m_bodies.end(), body);
    if (it != m_bodies.end()) {
        m_bodies.erase(it);
    }
    
    m_scene->removeActor(*body);
    body->release();
}

// ============================================================================
// SHAPES
// ============================================================================

PxShape* PhysicsWorld3D::addBoxShape(PxRigidActor* body, const glm::vec3& halfExtents) {
    if (!body || !m_physics || !m_defaultMaterial) return nullptr;
    
    PxBoxGeometry geometry(halfExtents.x, halfExtents.y, halfExtents.z);
    
    // Create shape with both simulation and scene query flags for character controller collision
    PxShapeFlags shapeFlags = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
    PxShape* shape = m_physics->createShape(geometry, *m_defaultMaterial, true, shapeFlags);
    
    if (shape) {
        body->attachShape(*shape);
        shape->release();  // Decrement reference, body now owns it
    }
    
    return shape;
}

PxShape* PhysicsWorld3D::addSphereShape(PxRigidActor* body, float radius) {
    if (!body || !m_physics || !m_defaultMaterial) return nullptr;
    
    PxSphereGeometry geometry(radius);
    PxShapeFlags shapeFlags = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
    PxShape* shape = m_physics->createShape(geometry, *m_defaultMaterial, true, shapeFlags);
    
    if (shape) {
        body->attachShape(*shape);
        shape->release();
    }
    
    return shape;
}

PxShape* PhysicsWorld3D::addCapsuleShape(PxRigidActor* body, float radius, float halfHeight) {
    if (!body || !m_physics || !m_defaultMaterial) return nullptr;
    
    PxCapsuleGeometry geometry(radius, halfHeight);
    PxShapeFlags shapeFlags = PxShapeFlag::eSIMULATION_SHAPE | PxShapeFlag::eSCENE_QUERY_SHAPE;
    PxShape* shape = m_physics->createShape(geometry, *m_defaultMaterial, true, shapeFlags);
    
    if (shape) {
        body->attachShape(*shape);
        shape->release();
    }
    
    return shape;
}

// ============================================================================
// BODY QUERIES
// ============================================================================

glm::vec3 PhysicsWorld3D::getBodyPosition(PxRigidActor* body) const {
    if (!body) return glm::vec3(0);
    PxTransform transform = body->getGlobalPose();
    return glm::vec3(transform.p.x, transform.p.y, transform.p.z);
}

void PhysicsWorld3D::setBodyPosition(PxRigidActor* body, const glm::vec3& position) {
    if (!body) return;
    PxTransform transform = body->getGlobalPose();
    transform.p = PxVec3(position.x, position.y, position.z);
    body->setGlobalPose(transform);
}

glm::vec3 PhysicsWorld3D::getBodyVelocity(PxRigidDynamic* body) const {
    if (!body) return glm::vec3(0);
    PxVec3 vel = body->getLinearVelocity();
    return glm::vec3(vel.x, vel.y, vel.z);
}

void PhysicsWorld3D::setBodyVelocity(PxRigidDynamic* body, const glm::vec3& velocity) {
    if (!body) return;
    body->setLinearVelocity(PxVec3(velocity.x, velocity.y, velocity.z));
}

void PhysicsWorld3D::addForce(PxRigidDynamic* body, const glm::vec3& force) {
    if (!body) return;
    body->addForce(PxVec3(force.x, force.y, force.z));
}

void PhysicsWorld3D::addImpulse(PxRigidDynamic* body, const glm::vec3& impulse) {
    if (!body) return;
    body->addForce(PxVec3(impulse.x, impulse.y, impulse.z), PxForceMode::eIMPULSE);
}

// ============================================================================
// RAYCASTING
// ============================================================================

PhysicsWorld3D::RaycastResult3D PhysicsWorld3D::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) const {
    RaycastResult3D result;
    if (!m_scene) return result;
    
    PxVec3 pxOrigin(origin.x, origin.y, origin.z);
    PxVec3 pxDir(direction.x, direction.y, direction.z);
    pxDir.normalize();
    
    PxRaycastBuffer hit;
    if (m_scene->raycast(pxOrigin, pxDir, maxDistance, hit)) {
        result.hit = true;
        result.point = glm::vec3(hit.block.position.x, hit.block.position.y, hit.block.position.z);
        result.normal = glm::vec3(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z);
        result.distance = hit.block.distance;
        result.actor = hit.block.actor;
    }
    
    return result;
}

} // namespace physics
} // namespace engine
