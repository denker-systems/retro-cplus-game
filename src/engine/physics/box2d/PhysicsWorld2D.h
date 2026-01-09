/**
 * @file PhysicsWorld2D.h
 * @brief Box2D world wrapper for 2D physics simulation
 * 
 * Provides a clean interface to Box2D v3 for platformer physics.
 * Owned by Scene, manages all physics bodies in that scene.
 */
#pragma once

#include "engine/physics/IPhysicsWorld.h"
#include <box2d/box2d.h>
#include <glm/glm.hpp>
#include <vector>
#include <functional>
#include <unordered_map>

struct SDL_Renderer;

namespace engine {

class ActorObject;

namespace physics {

// ============================================================================
// COLLISION CATEGORIES
// ============================================================================

enum class CollisionCategory : uint64_t {
    None       = 0,
    Player     = 1 << 0,
    Enemy      = 1 << 1,
    Ground     = 1 << 2,
    Platform   = 1 << 3,
    Wall       = 1 << 4,
    Trigger    = 1 << 5,
    Projectile = 1 << 6,
    Pickup     = 1 << 7,
    All        = 0xFFFFFFFFFFFFFFFF
};

// ============================================================================
// BODY DEFINITION
// ============================================================================

enum class BodyType2D {
    Static,     // Doesn't move (ground, walls)
    Dynamic,    // Full physics simulation (player, enemies)
    Kinematic   // Moves but not affected by forces (moving platforms)
};

struct BodyDef2D {
    BodyType2D type = BodyType2D::Dynamic;
    glm::vec2 position{0, 0};       // In pixels
    float angle = 0.0f;             // In radians
    glm::vec2 linearVelocity{0, 0}; // In pixels/sec
    float angularVelocity = 0.0f;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    float gravityScale = 1.0f;
    bool fixedRotation = true;      // Prevent rotation (for characters)
    bool isBullet = false;          // Continuous collision detection
    void* userData = nullptr;       // Typically ActorObject*
};

// ============================================================================
// SHAPE DEFINITION
// ============================================================================

enum class ShapeType2D {
    Box,
    Circle,
    Capsule
};

struct ShapeDef2D {
    ShapeType2D type = ShapeType2D::Box;
    glm::vec2 size{32, 32};         // Width/height for box, radius for circle
    glm::vec2 offset{0, 0};         // Offset from body center
    float density = 1.0f;
    float friction = 0.3f;
    void* userData = nullptr;       // Typically ActorObject* for trigger callbacks
    float restitution = 0.0f;       // Bounciness
    bool isSensor = false;          // Trigger only, no collision response
    CollisionCategory category = CollisionCategory::All;
    CollisionCategory mask = CollisionCategory::All;
};

// ============================================================================
// PHYSICS WORLD 2D
// ============================================================================

/**
 * @brief Box2D world wrapper - owned by Scene
 */
class PhysicsWorld2D {
public:
    PhysicsWorld2D();
    ~PhysicsWorld2D();
    
    // Non-copyable
    PhysicsWorld2D(const PhysicsWorld2D&) = delete;
    PhysicsWorld2D& operator=(const PhysicsWorld2D&) = delete;
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    void initialize(glm::vec2 gravity = {0.0f, 980.0f});
    void shutdown();
    void step(float deltaTime);
    
    // ========================================================================
    // WORLD SETTINGS
    // ========================================================================
    
    void setGravity(glm::vec2 gravity);
    glm::vec2 getGravity() const;
    
    // ========================================================================
    // BODY MANAGEMENT
    // ========================================================================
    
    b2BodyId createBody(const BodyDef2D& def);
    void destroyBody(b2BodyId bodyId);
    
    b2ShapeId addShape(b2BodyId bodyId, const ShapeDef2D& def);
    void removeShape(b2ShapeId shapeId);
    
    // ========================================================================
    // BODY QUERIES
    // ========================================================================
    
    glm::vec2 getBodyPosition(b2BodyId bodyId) const;
    void setBodyPosition(b2BodyId bodyId, glm::vec2 position);
    
    float getBodyAngle(b2BodyId bodyId) const;
    void setBodyAngle(b2BodyId bodyId, float angle);
    
    glm::vec2 getBodyVelocity(b2BodyId bodyId) const;
    void setBodyVelocity(b2BodyId bodyId, glm::vec2 velocity);
    
    void applyForce(b2BodyId bodyId, glm::vec2 force);
    void applyImpulse(b2BodyId bodyId, glm::vec2 impulse);
    
    // ========================================================================
    // RAYCASTING
    // ========================================================================
    
    struct RaycastResult {
        bool hit = false;
        glm::vec2 point{0, 0};
        glm::vec2 normal{0, 0};
        float fraction = 0.0f;
        b2ShapeId shapeId;
    };
    
    RaycastResult raycast(glm::vec2 start, glm::vec2 end, CollisionCategory mask = CollisionCategory::All) const;
    
    // ========================================================================
    // DEBUG RENDERING
    // ========================================================================
    
    void setDebugDraw(bool enabled) { m_debugDrawEnabled = enabled; }
    bool isDebugDrawEnabled() const { return m_debugDrawEnabled; }
    void debugDraw(SDL_Renderer* renderer, glm::vec2 cameraOffset = {0, 0}, float zoom = 1.0f);
    
    // ========================================================================
    // CONTACT CALLBACKS
    // ========================================================================
    
    std::function<void(const ContactInfo&)> onContactBegin;
    std::function<void(const ContactInfo&)> onContactEnd;
    
    // ========================================================================
    // STATE
    // ========================================================================
    
    bool isInitialized() const { return m_initialized; }
    b2WorldId getWorldId() const { return m_worldId; }
    
private:
    b2WorldId m_worldId;
    bool m_initialized = false;
    bool m_debugDrawEnabled = false;
    
    // Track bodies for cleanup
    std::vector<b2BodyId> m_bodies;
};

} // namespace physics
} // namespace engine
