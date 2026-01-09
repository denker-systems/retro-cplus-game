/**
 * @file IPhysicsWorld.h
 * @brief Abstract base class for physics worlds (2D and 3D)
 * 
 * Provides common interface for both Box2D (2D) and PhysX (3D) physics.
 * Follows OOP inheritance pattern for clean separation.
 */
#pragma once

#include <glm/glm.hpp>
#include <functional>
#include <string>

struct SDL_Renderer;

namespace engine {

class ActorObject;

namespace physics {

/**
 * @brief Physics dimension enum
 */
enum class PhysicsDimension {
    Physics2D,
    Physics3D
};

/**
 * @brief Body type enum (shared between 2D and 3D)
 */
enum class BodyType {
    Static,     // Doesn't move (ground, walls)
    Dynamic,    // Full physics simulation
    Kinematic   // Controlled movement (moving platforms)
};

/**
 * @brief Generic contact info for collision callbacks
 */
struct ContactInfo {
    ActorObject* actorA = nullptr;
    ActorObject* actorB = nullptr;
    glm::vec3 normal{0, 0, 0};
    glm::vec3 point{0, 0, 0};
    bool isSensorContact = false;
};

/**
 * @brief Abstract physics world interface
 * 
 * Base class for PhysicsWorld2D (Box2D) and PhysicsWorld3D (PhysX)
 */
class IPhysicsWorld {
public:
    virtual ~IPhysicsWorld() = default;
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    virtual void step(float deltaTime) = 0;
    
    // ========================================================================
    // PROPERTIES
    // ========================================================================
    
    virtual PhysicsDimension getDimension() const = 0;
    virtual bool isInitialized() const = 0;
    
    virtual void setGravity(glm::vec3 gravity) = 0;
    virtual glm::vec3 getGravity() const = 0;
    
    // ========================================================================
    // DEBUG
    // ========================================================================
    
    virtual void setDebugDraw(bool enabled) = 0;
    virtual bool isDebugDrawEnabled() const = 0;
    
    // ========================================================================
    // CALLBACKS
    // ========================================================================
    
    std::function<void(const ContactInfo&)> onContactBegin;
    std::function<void(const ContactInfo&)> onContactEnd;
    
protected:
    IPhysicsWorld() = default;
    
    // Non-copyable
    IPhysicsWorld(const IPhysicsWorld&) = delete;
    IPhysicsWorld& operator=(const IPhysicsWorld&) = delete;
};

} // namespace physics
} // namespace engine
