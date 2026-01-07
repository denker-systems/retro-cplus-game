/**
 * @file PhysicsWorld2D.cpp
 * @brief Box2D world wrapper implementation
 */
#include "PhysicsWorld2D.h"
#include "PhysicsConversions.h"
#include <SDL.h>
#include <iostream>

namespace engine {
namespace physics {

// ============================================================================
// CONSTRUCTOR / DESTRUCTOR
// ============================================================================

PhysicsWorld2D::PhysicsWorld2D()
    : m_worldId(b2_nullWorldId)
{
}

PhysicsWorld2D::~PhysicsWorld2D() {
    shutdown();
}

// ============================================================================
// LIFECYCLE
// ============================================================================

void PhysicsWorld2D::initialize(glm::vec2 gravity) {
    if (m_initialized) {
        return;
    }
    
    b2WorldDef worldDef = b2DefaultWorldDef();
    worldDef.gravity = toBox2D(gravity);
    
    m_worldId = b2CreateWorld(&worldDef);
    m_initialized = true;
    
    std::cout << "[PhysicsWorld2D] Initialized with gravity (" 
              << gravity.x << ", " << gravity.y << ") pixels/s²" << std::endl;
}

void PhysicsWorld2D::shutdown() {
    if (!m_initialized) {
        return;
    }
    
    // Clear tracked bodies
    m_bodies.clear();
    
    // Destroy world
    b2DestroyWorld(m_worldId);
    m_worldId = b2_nullWorldId;
    m_initialized = false;
    
    std::cout << "[PhysicsWorld2D] Shutdown complete" << std::endl;
}

void PhysicsWorld2D::step(float deltaTime) {
    if (!m_initialized) return;
    
    // Box2D recommends fixed timestep, but we'll use variable for now
    // Clamp deltaTime to prevent spiral of death
    float clampedDt = std::min(deltaTime, 1.0f / 30.0f);
    
    b2World_Step(m_worldId, clampedDt, defaults::VELOCITY_ITERATIONS);
}

// ============================================================================
// WORLD SETTINGS
// ============================================================================

void PhysicsWorld2D::setGravity(glm::vec2 gravity) {
    if (!m_initialized) return;
    b2World_SetGravity(m_worldId, toBox2D(gravity));
}

glm::vec2 PhysicsWorld2D::getGravity() const {
    if (!m_initialized) return {0, 0};
    return fromBox2D(b2World_GetGravity(m_worldId));
}

// ============================================================================
// BODY MANAGEMENT
// ============================================================================

b2BodyId PhysicsWorld2D::createBody(const BodyDef2D& def) {
    if (!m_initialized) return b2_nullBodyId;
    
    b2BodyDef bodyDef = b2DefaultBodyDef();
    
    // Set body type
    switch (def.type) {
        case BodyType2D::Static:
            bodyDef.type = b2_staticBody;
            break;
        case BodyType2D::Dynamic:
            bodyDef.type = b2_dynamicBody;
            break;
        case BodyType2D::Kinematic:
            bodyDef.type = b2_kinematicBody;
            break;
    }
    
    bodyDef.position = toBox2D(def.position);
    bodyDef.rotation = b2MakeRot(def.angle);
    bodyDef.linearVelocity = toBox2D(def.linearVelocity);
    bodyDef.angularVelocity = def.angularVelocity;
    bodyDef.linearDamping = def.linearDamping;
    bodyDef.angularDamping = def.angularDamping;
    bodyDef.gravityScale = def.gravityScale;
    bodyDef.fixedRotation = def.fixedRotation;
    bodyDef.isBullet = def.isBullet;
    bodyDef.userData = def.userData;
    
    b2BodyId bodyId = b2CreateBody(m_worldId, &bodyDef);
    m_bodies.push_back(bodyId);
    
    return bodyId;
}

void PhysicsWorld2D::destroyBody(b2BodyId bodyId) {
    if (!m_initialized) return;
    if (!b2Body_IsValid(bodyId)) return;
    
    b2DestroyBody(bodyId);
    
    // Remove from tracking
    auto it = std::find_if(m_bodies.begin(), m_bodies.end(),
        [bodyId](const b2BodyId& id) {
            return B2_ID_EQUALS(id, bodyId);
        });
    if (it != m_bodies.end()) {
        m_bodies.erase(it);
    }
}

b2ShapeId PhysicsWorld2D::addShape(b2BodyId bodyId, const ShapeDef2D& def) {
    if (!m_initialized) return b2_nullShapeId;
    if (!b2Body_IsValid(bodyId)) return b2_nullShapeId;
    
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = def.density;
    shapeDef.material.friction = def.friction;
    shapeDef.material.restitution = def.restitution;
    shapeDef.isSensor = def.isSensor;
    shapeDef.filter.categoryBits = static_cast<uint64_t>(def.category);
    shapeDef.filter.maskBits = static_cast<uint64_t>(def.mask);
    
    b2ShapeId shapeId = b2_nullShapeId;
    
    switch (def.type) {
        case ShapeType2D::Box: {
            b2Polygon box = b2MakeOffsetBox(
                pixelsToMeters(def.size.x / 2.0f),
                pixelsToMeters(def.size.y / 2.0f),
                toBox2D(def.offset),
                b2MakeRot(0)
            );
            shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);
            break;
        }
        case ShapeType2D::Circle: {
            b2Circle circle;
            circle.center = toBox2D(def.offset);
            circle.radius = pixelsToMeters(def.size.x / 2.0f);
            shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);
            break;
        }
        case ShapeType2D::Capsule: {
            b2Capsule capsule;
            float halfHeight = pixelsToMeters(def.size.y / 2.0f - def.size.x / 2.0f);
            capsule.center1 = b2Vec2{toBox2D(def.offset).x, toBox2D(def.offset).y - halfHeight};
            capsule.center2 = b2Vec2{toBox2D(def.offset).x, toBox2D(def.offset).y + halfHeight};
            capsule.radius = pixelsToMeters(def.size.x / 2.0f);
            shapeId = b2CreateCapsuleShape(bodyId, &shapeDef, &capsule);
            break;
        }
    }
    
    return shapeId;
}

void PhysicsWorld2D::removeShape(b2ShapeId shapeId) {
    if (!m_initialized) return;
    if (!b2Shape_IsValid(shapeId)) return;
    b2DestroyShape(shapeId, true); // true = update body mass
}

// ============================================================================
// BODY QUERIES
// ============================================================================

glm::vec2 PhysicsWorld2D::getBodyPosition(b2BodyId bodyId) const {
    if (!b2Body_IsValid(bodyId)) return {0, 0};
    return fromBox2D(b2Body_GetPosition(bodyId));
}

void PhysicsWorld2D::setBodyPosition(b2BodyId bodyId, glm::vec2 position) {
    if (!b2Body_IsValid(bodyId)) return;
    b2Body_SetTransform(bodyId, toBox2D(position), b2Body_GetRotation(bodyId));
}

float PhysicsWorld2D::getBodyAngle(b2BodyId bodyId) const {
    if (!b2Body_IsValid(bodyId)) return 0.0f;
    return b2Rot_GetAngle(b2Body_GetRotation(bodyId));
}

void PhysicsWorld2D::setBodyAngle(b2BodyId bodyId, float angle) {
    if (!b2Body_IsValid(bodyId)) return;
    b2Body_SetTransform(bodyId, b2Body_GetPosition(bodyId), b2MakeRot(angle));
}

glm::vec2 PhysicsWorld2D::getBodyVelocity(b2BodyId bodyId) const {
    if (!b2Body_IsValid(bodyId)) return {0, 0};
    return fromBox2D(b2Body_GetLinearVelocity(bodyId));
}

void PhysicsWorld2D::setBodyVelocity(b2BodyId bodyId, glm::vec2 velocity) {
    if (!b2Body_IsValid(bodyId)) return;
    b2Body_SetLinearVelocity(bodyId, toBox2D(velocity));
}

void PhysicsWorld2D::applyForce(b2BodyId bodyId, glm::vec2 force) {
    if (!b2Body_IsValid(bodyId)) return;
    b2Body_ApplyForceToCenter(bodyId, toBox2D(force), true);
}

void PhysicsWorld2D::applyImpulse(b2BodyId bodyId, glm::vec2 impulse) {
    if (!b2Body_IsValid(bodyId)) return;
    b2Body_ApplyLinearImpulseToCenter(bodyId, toBox2D(impulse), true);
}

// ============================================================================
// RAYCASTING
// ============================================================================

PhysicsWorld2D::RaycastResult PhysicsWorld2D::raycast(glm::vec2 start, glm::vec2 end, CollisionCategory mask) const {
    RaycastResult result;
    if (!m_initialized) return result;
    
    b2QueryFilter filter = b2DefaultQueryFilter();
    filter.maskBits = static_cast<uint64_t>(mask);
    
    b2RayResult rayResult = b2World_CastRayClosest(
        m_worldId,
        toBox2D(start),
        b2Sub(toBox2D(end), toBox2D(start)),
        filter
    );
    
    result.hit = rayResult.hit;
    if (result.hit) {
        result.point = fromBox2D(rayResult.point);
        result.normal = fromBox2D(rayResult.normal);
        result.fraction = rayResult.fraction;
        result.shapeId = rayResult.shapeId;
    }
    
    return result;
}

// ============================================================================
// DEBUG RENDERING
// ============================================================================

void PhysicsWorld2D::debugDraw(SDL_Renderer* renderer, glm::vec2 cameraOffset, float zoom) {
    if (!m_initialized || !m_debugDrawEnabled || !renderer) return;
    
    // Draw all bodies
    for (const auto& bodyId : m_bodies) {
        if (!b2Body_IsValid(bodyId)) continue;
        
        glm::vec2 pos = getBodyPosition(bodyId);
        pos = (pos - cameraOffset) * zoom;
        
        // Get body type for color
        b2BodyType type = b2Body_GetType(bodyId);
        SDL_Color color;
        switch (type) {
            case b2_staticBody:
                color = {100, 255, 100, 200}; // Green
                break;
            case b2_dynamicBody:
                color = {100, 100, 255, 200}; // Blue
                break;
            case b2_kinematicBody:
                color = {255, 255, 100, 200}; // Yellow
                break;
            default:
                color = {255, 255, 255, 200}; // White
        }
        
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        
        // Draw shapes attached to body
        int shapeCount = b2Body_GetShapeCount(bodyId);
        b2ShapeId* shapes = new b2ShapeId[shapeCount];
        b2Body_GetShapes(bodyId, shapes, shapeCount);
        
        for (int i = 0; i < shapeCount; i++) {
            b2ShapeType shapeType = b2Shape_GetType(shapes[i]);
            
            if (shapeType == b2_polygonShape) {
                b2Polygon polygon = b2Shape_GetPolygon(shapes[i]);
                
                // Draw polygon edges
                for (int j = 0; j < polygon.count; j++) {
                    glm::vec2 p1 = fromBox2D(polygon.vertices[j]);
                    glm::vec2 p2 = fromBox2D(polygon.vertices[(j + 1) % polygon.count]);
                    
                    p1 = (pos + p1 * zoom);
                    p2 = (pos + p2 * zoom);
                    
                    SDL_RenderDrawLine(renderer, 
                        static_cast<int>(p1.x), static_cast<int>(p1.y),
                        static_cast<int>(p2.x), static_cast<int>(p2.y));
                }
            }
            else if (shapeType == b2_circleShape) {
                b2Circle circle = b2Shape_GetCircle(shapes[i]);
                float radius = metersToPixels(circle.radius) * zoom;
                
                // Draw circle approximation
                int segments = 16;
                for (int j = 0; j < segments; j++) {
                    float a1 = (j / (float)segments) * 2.0f * 3.14159f;
                    float a2 = ((j + 1) / (float)segments) * 2.0f * 3.14159f;
                    
                    int x1 = static_cast<int>(pos.x + cos(a1) * radius);
                    int y1 = static_cast<int>(pos.y + sin(a1) * radius);
                    int x2 = static_cast<int>(pos.x + cos(a2) * radius);
                    int y2 = static_cast<int>(pos.y + sin(a2) * radius);
                    
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }
            }
        }
        
        delete[] shapes;
    }
}

} // namespace physics
} // namespace engine
