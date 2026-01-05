/**
 * @file Node2D.h
 * @brief 2D node with transform (position, rotation, scale)
 */
#pragma once

#include "Node.h"
#include "Vec2.h"

namespace engine {

/**
 * @brief 2D node with position, rotation, and scale
 * 
 * Transforms are relative to parent. Use getGlobalPosition() etc.
 * to get world-space values.
 */
class Node2D : public Node {
public:
    Node2D();
    explicit Node2D(const std::string& name);
    Node2D(const std::string& name, float x, float y);
    virtual ~Node2D() = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // LOCAL TRANSFORM (relative to parent)
    // ═══════════════════════════════════════════════════════════════════
    
    const Vec2& getPosition() const { return m_position; }
    void setPosition(const Vec2& pos) { m_position = pos; }
    void setPosition(float x, float y) { m_position = Vec2(x, y); }
    
    float getX() const { return m_position.x; }
    float getY() const { return m_position.y; }
    void setX(float x) { m_position.x = x; }
    void setY(float y) { m_position.y = y; }
    
    float getRotation() const { return m_rotation; }
    void setRotation(float radians) { m_rotation = radians; }
    void setRotationDegrees(float degrees) { m_rotation = degrees * 0.0174533f; }
    float getRotationDegrees() const { return m_rotation * 57.2958f; }
    
    const Vec2& getScale() const { return m_scale; }
    void setScale(const Vec2& scale) { m_scale = scale; }
    void setScale(float x, float y) { m_scale = Vec2(x, y); }
    void setScale(float uniform) { m_scale = Vec2(uniform, uniform); }
    
    // ═══════════════════════════════════════════════════════════════════
    // GLOBAL TRANSFORM (world space)
    // ═══════════════════════════════════════════════════════════════════
    
    Vec2 getGlobalPosition() const;
    float getGlobalRotation() const;
    Vec2 getGlobalScale() const;
    
    void setGlobalPosition(const Vec2& pos);
    void setGlobalPosition(float x, float y);
    
    // ═══════════════════════════════════════════════════════════════════
    // MOVEMENT HELPERS
    // ═══════════════════════════════════════════════════════════════════
    
    void translate(const Vec2& delta) { m_position += delta; }
    void translate(float dx, float dy) { m_position += Vec2(dx, dy); }
    
    void rotate(float radians) { m_rotation += radians; }
    void rotateDegrees(float degrees) { m_rotation += degrees * 0.0174533f; }
    
    /** @brief Move towards target position */
    void moveToward(const Vec2& target, float delta);
    
    /** @brief Look at a point (set rotation) */
    void lookAt(const Vec2& target);
    
    /** @brief Get distance to another node */
    float distanceTo(const Node2D* other) const;
    
    /** @brief Get direction vector to another node */
    Vec2 directionTo(const Node2D* other) const;
    
protected:
    /** @brief Get parent as Node2D (or nullptr) */
    Node2D* getParent2D() const;
    
    Vec2 m_position{0, 0};
    float m_rotation = 0.0f;  // Radians
    Vec2 m_scale{1, 1};
};

} // namespace engine
