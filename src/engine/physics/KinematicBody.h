/**
 * @file KinematicBody.h
 * @brief Kinematic body with move_and_slide
 */
#pragma once

#include "PhysicsBody.h"
#include <vector>

namespace engine {

/**
 * @brief Kinematic body for player/NPC movement
 * 
 * Features:
 * - move_and_slide for smooth collision response
 * - Floor detection
 * - Slope handling
 */
class KinematicBody : public PhysicsBody {
public:
    KinematicBody();
    explicit KinematicBody(const std::string& name);
    virtual ~KinematicBody() = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // MOVEMENT
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Move with collision response */
    Vec2 moveAndSlide(const Vec2& velocity, const std::vector<CollisionShape*>& obstacles);
    
    /** @brief Move and collide (returns collision info) */
    bool moveAndCollide(const Vec2& motion, const std::vector<CollisionShape*>& obstacles);
    
    // ═══════════════════════════════════════════════════════════════════
    // FLOOR DETECTION
    // ═══════════════════════════════════════════════════════════════════
    
    bool isOnFloor() const { return m_isOnFloor; }
    void setOnFloor(bool onFloor) { m_isOnFloor = onFloor; }
    
    const Vec2& getFloorNormal() const { return m_floorNormal; }
    
    /** @brief Floor angle threshold (radians) */
    float getFloorMaxAngle() const { return m_floorMaxAngle; }
    void setFloorMaxAngle(float angle) { m_floorMaxAngle = angle; }
    
    // ═══════════════════════════════════════════════════════════════════
    // SLIDE SETTINGS
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Max slide iterations */
    int getMaxSlides() const { return m_maxSlides; }
    void setMaxSlides(int max) { m_maxSlides = max; }
    
    /** @brief Stop if velocity is below this threshold */
    float getStopThreshold() const { return m_stopThreshold; }
    void setStopThreshold(float threshold) { m_stopThreshold = threshold; }
    
protected:
    Vec2 slideOnSlope(const Vec2& velocity, const Vec2& normal);
    
    bool m_isOnFloor = false;
    Vec2 m_floorNormal{0, -1};
    float m_floorMaxAngle = 0.785f;  // 45 degrees
    
    int m_maxSlides = 4;
    float m_stopThreshold = 0.01f;
};

} // namespace engine
