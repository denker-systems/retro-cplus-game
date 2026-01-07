/**
 * @file CollisionComponent.h
 * @brief Collision Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/core/Vec2.h"
#include <functional>

namespace engine {

// ActorObject is defined in CoreRedirects.h, no forward declaration needed

/**
 * @brief Component for collision detection
 */
class CollisionComponent : public ActorComponent {
public:
    using CollisionCallback = std::function<void(ActorObject*)>;
    
    enum class CollisionShape {
        Box,
        Circle
    };
    
    CollisionComponent(const std::string& name = "CollisionComponent");
    virtual ~CollisionComponent() = default;
    
    void setShape(CollisionShape shape) { m_shape = shape; }
    CollisionShape getShape() const { return m_shape; }
    
    void setSize(Vec2 size) { m_size = size; }
    Vec2 getSize() const { return m_size; }
    
    void setRadius(float radius) { m_radius = radius; }
    float getRadius() const { return m_radius; }
    
    void setOffset(Vec2 offset) { m_offset = offset; }
    Vec2 getOffset() const { return m_offset; }
    
    void setOnCollision(CollisionCallback callback) { m_onCollision = callback; }
    
    bool checkCollision(CollisionComponent* other);
    
private:
    CollisionShape m_shape = CollisionShape::Box;
    Vec2 m_size{32, 32};
    float m_radius = 16.0f;
    Vec2 m_offset{0, 0};
    CollisionCallback m_onCollision;
};

} // namespace engine
