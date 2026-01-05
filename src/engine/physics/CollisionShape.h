/**
 * @file CollisionShape.h
 * @brief Collision shapes for physics
 */
#pragma once

#include "core/Vec2.h"
#include <SDL.h>
#include <vector>

namespace engine {

/**
 * @brief Base collision shape
 */
class CollisionShape {
public:
    enum class Type {
        AABB,      // Axis-Aligned Bounding Box
        Circle,
        Polygon
    };
    
    CollisionShape(Type type) : m_type(type) {}
    virtual ~CollisionShape() = default;
    
    Type getType() const { return m_type; }
    
    /** @brief Check collision with another shape */
    virtual bool overlaps(const CollisionShape& other) const = 0;
    
    /** @brief Get bounding box (for broad phase) */
    virtual SDL_Rect getBounds() const = 0;
    
protected:
    Type m_type;
};

/**
 * @brief Axis-Aligned Bounding Box
 */
class AABBShape : public CollisionShape {
public:
    AABBShape();
    AABBShape(float x, float y, float width, float height);
    AABBShape(const SDL_Rect& rect);
    
    bool overlaps(const CollisionShape& other) const override;
    SDL_Rect getBounds() const override { return m_rect; }
    
    const SDL_Rect& getRect() const { return m_rect; }
    void setRect(const SDL_Rect& rect) { m_rect = rect; }
    void setRect(float x, float y, float width, float height);
    
    Vec2 getPosition() const { return Vec2(static_cast<float>(m_rect.x), static_cast<float>(m_rect.y)); }
    Vec2 getSize() const { return Vec2(static_cast<float>(m_rect.w), static_cast<float>(m_rect.h)); }
    
    /** @brief Check AABB vs AABB collision */
    bool overlapsAABB(const AABBShape& other) const;
    
    /** @brief Check AABB vs Circle collision */
    bool overlapsCircle(const class CircleShape& circle) const;
    
protected:
    SDL_Rect m_rect;
};

/**
 * @brief Circle collision shape
 */
class CircleShape : public CollisionShape {
public:
    CircleShape();
    CircleShape(float x, float y, float radius);
    CircleShape(const Vec2& center, float radius);
    
    bool overlaps(const CollisionShape& other) const override;
    SDL_Rect getBounds() const override;
    
    const Vec2& getCenter() const { return m_center; }
    void setCenter(const Vec2& center) { m_center = center; }
    void setCenter(float x, float y) { m_center = Vec2(x, y); }
    
    float getRadius() const { return m_radius; }
    void setRadius(float radius) { m_radius = radius; }
    
    /** @brief Check Circle vs Circle collision */
    bool overlapsCircle(const CircleShape& other) const;
    
    /** @brief Check Circle vs AABB collision */
    bool overlapsAABB(const AABBShape& aabb) const;
    
protected:
    Vec2 m_center;
    float m_radius = 0.0f;
};

/**
 * @brief Polygon collision shape (convex)
 */
class PolygonShape : public CollisionShape {
public:
    PolygonShape();
    explicit PolygonShape(const std::vector<Vec2>& vertices);
    
    bool overlaps(const CollisionShape& other) const override;
    SDL_Rect getBounds() const override;
    
    const std::vector<Vec2>& getVertices() const { return m_vertices; }
    void setVertices(const std::vector<Vec2>& vertices) { m_vertices = vertices; }
    
    /** @brief Check Polygon vs Polygon collision (SAT) */
    bool overlapsPolygon(const PolygonShape& other) const;
    
protected:
    std::vector<Vec2> m_vertices;
};

} // namespace engine
