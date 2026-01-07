/**
 * @file PhysicsConversions.h
 * @brief Pixel/Meter conversions for Box2D integration
 * 
 * Box2D uses meters, our game uses pixels.
 * Standard: 32 pixels = 1 meter
 */
#pragma once

#include <glm/glm.hpp>
#include <box2d/box2d.h>

namespace engine {
namespace physics {

/** Pixels per meter - adjust for your game scale */
constexpr float PIXELS_PER_METER = 32.0f;

// ============================================================================
// SCALAR CONVERSIONS
// ============================================================================

inline float pixelsToMeters(float pixels) {
    return pixels / PIXELS_PER_METER;
}

inline float metersToPixels(float meters) {
    return meters * PIXELS_PER_METER;
}

// ============================================================================
// VECTOR CONVERSIONS (GLM)
// ============================================================================

inline b2Vec2 toBox2D(const glm::vec2& v) {
    return b2Vec2{
        pixelsToMeters(v.x),
        pixelsToMeters(v.y)
    };
}

inline glm::vec2 fromBox2D(b2Vec2 v) {
    return glm::vec2{
        metersToPixels(v.x),
        metersToPixels(v.y)
    };
}

// ============================================================================
// ANGLE CONVERSIONS
// ============================================================================

inline float degreesToRadians(float degrees) {
    return degrees * (3.14159265359f / 180.0f);
}

inline float radiansToDegrees(float radians) {
    return radians * (180.0f / 3.14159265359f);
}

// ============================================================================
// COMMON PHYSICS VALUES
// ============================================================================

namespace defaults {
    constexpr float GRAVITY_Y = 9.81f;           // Earth gravity in m/s²
    constexpr float PLAYER_DENSITY = 1.0f;
    constexpr float PLAYER_FRICTION = 0.3f;
    constexpr float GROUND_FRICTION = 0.6f;
    constexpr float PLATFORM_FRICTION = 0.4f;
    constexpr int VELOCITY_ITERATIONS = 8;
    constexpr int POSITION_ITERATIONS = 3;
}

} // namespace physics
} // namespace engine
