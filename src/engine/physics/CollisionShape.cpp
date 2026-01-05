/**
 * @file CollisionShape.cpp
 * @brief Implementation of collision shapes
 */
#include "CollisionShape.h"
#include <cmath>
#include <algorithm>

namespace engine {

// ═══════════════════════════════════════════════════════════════════════════
// AABB SHAPE
// ═══════════════════════════════════════════════════════════════════════════

AABBShape::AABBShape() : CollisionShape(Type::AABB), m_rect{0, 0, 0, 0} {}

AABBShape::AABBShape(float x, float y, float width, float height)
    : CollisionShape(Type::AABB) {
    setRect(x, y, width, height);
}

AABBShape::AABBShape(const SDL_Rect& rect)
    : CollisionShape(Type::AABB), m_rect(rect) {}

void AABBShape::setRect(float x, float y, float width, float height) {
    m_rect.x = static_cast<int>(x);
    m_rect.y = static_cast<int>(y);
    m_rect.w = static_cast<int>(width);
    m_rect.h = static_cast<int>(height);
}

bool AABBShape::overlaps(const CollisionShape& other) const {
    switch (other.getType()) {
        case Type::AABB:
            return overlapsAABB(static_cast<const AABBShape&>(other));
        case Type::Circle:
            return overlapsCircle(static_cast<const CircleShape&>(other));
        case Type::Polygon:
            // TODO: Implement AABB vs Polygon
            return false;
    }
    return false;
}

bool AABBShape::overlapsAABB(const AABBShape& other) const {
    return SDL_HasIntersection(&m_rect, &other.m_rect);
}

bool AABBShape::overlapsCircle(const CircleShape& circle) const {
    // Find closest point on AABB to circle center
    Vec2 center = circle.getCenter();
    float closestX = std::max(static_cast<float>(m_rect.x), 
                              std::min(center.x, static_cast<float>(m_rect.x + m_rect.w)));
    float closestY = std::max(static_cast<float>(m_rect.y), 
                              std::min(center.y, static_cast<float>(m_rect.y + m_rect.h)));
    
    Vec2 closest(closestX, closestY);
    float distSquared = center.distanceSquaredTo(closest);
    float radiusSquared = circle.getRadius() * circle.getRadius();
    
    return distSquared <= radiusSquared;
}

// ═══════════════════════════════════════════════════════════════════════════
// CIRCLE SHAPE
// ═══════════════════════════════════════════════════════════════════════════

CircleShape::CircleShape() : CollisionShape(Type::Circle) {}

CircleShape::CircleShape(float x, float y, float radius)
    : CollisionShape(Type::Circle), m_center(x, y), m_radius(radius) {}

CircleShape::CircleShape(const Vec2& center, float radius)
    : CollisionShape(Type::Circle), m_center(center), m_radius(radius) {}

SDL_Rect CircleShape::getBounds() const {
    return SDL_Rect{
        static_cast<int>(m_center.x - m_radius),
        static_cast<int>(m_center.y - m_radius),
        static_cast<int>(m_radius * 2),
        static_cast<int>(m_radius * 2)
    };
}

bool CircleShape::overlaps(const CollisionShape& other) const {
    switch (other.getType()) {
        case Type::Circle:
            return overlapsCircle(static_cast<const CircleShape&>(other));
        case Type::AABB:
            return overlapsAABB(static_cast<const AABBShape&>(other));
        case Type::Polygon:
            // TODO: Implement Circle vs Polygon
            return false;
    }
    return false;
}

bool CircleShape::overlapsCircle(const CircleShape& other) const {
    float distSquared = m_center.distanceSquaredTo(other.m_center);
    float radiusSum = m_radius + other.m_radius;
    return distSquared <= (radiusSum * radiusSum);
}

bool CircleShape::overlapsAABB(const AABBShape& aabb) const {
    return aabb.overlapsCircle(*this);
}

// ═══════════════════════════════════════════════════════════════════════════
// POLYGON SHAPE
// ═══════════════════════════════════════════════════════════════════════════

PolygonShape::PolygonShape() : CollisionShape(Type::Polygon) {}

PolygonShape::PolygonShape(const std::vector<Vec2>& vertices)
    : CollisionShape(Type::Polygon), m_vertices(vertices) {}

SDL_Rect PolygonShape::getBounds() const {
    if (m_vertices.empty()) {
        return SDL_Rect{0, 0, 0, 0};
    }
    
    float minX = m_vertices[0].x;
    float minY = m_vertices[0].y;
    float maxX = m_vertices[0].x;
    float maxY = m_vertices[0].y;
    
    for (const Vec2& v : m_vertices) {
        minX = std::min(minX, v.x);
        minY = std::min(minY, v.y);
        maxX = std::max(maxX, v.x);
        maxY = std::max(maxY, v.y);
    }
    
    return SDL_Rect{
        static_cast<int>(minX),
        static_cast<int>(minY),
        static_cast<int>(maxX - minX),
        static_cast<int>(maxY - minY)
    };
}

bool PolygonShape::overlaps(const CollisionShape& other) const {
    switch (other.getType()) {
        case Type::Polygon:
            return overlapsPolygon(static_cast<const PolygonShape&>(other));
        case Type::AABB:
        case Type::Circle:
            // TODO: Implement Polygon vs AABB/Circle
            return false;
    }
    return false;
}

bool PolygonShape::overlapsPolygon(const PolygonShape& other) const {
    // TODO: Implement SAT (Separating Axis Theorem)
    // For now, return false
    return false;
}

} // namespace engine
