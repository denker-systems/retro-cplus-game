/**
 * @file ActorObject.h
 * @brief Base class for all actors in the game world
 * 
 * ActorObject is the base for all gameplay objects that exist in a scene.
 * Provides transform (position, rotation, scale) and is the owner of components.
 * 
 * This replaces the old Node2D system with a cleaner Actor-based approach.
 */
#pragma once

#include "Object.h"
#include "Vec2.h"
#include <SDL.h>

namespace engine {

/**
 * @brief Base class for all game actors
 * 
 * Features:
 * - Transform (position, rotation, scale)
 * - Lifecycle hooks
 * - Component ownership (via ActorObjectExtended)
 * 
 * This is similar to AActor in Unreal Engine.
 */
class ActorObject : public Object {
public:
    ActorObject(const std::string& name);
    virtual ~ActorObject() = default;
    
    // ========================================================================
    // TRANSFORM
    // ========================================================================
    
    void setPosition(float x, float y);
    void setPosition(const Vec2& pos);
    Vec2 getPosition() const { return m_position; }
    float getX() const { return m_position.x; }
    float getY() const { return m_position.y; }
    
    void setRotation(float angle) { m_rotation = angle; }
    float getRotation() const { return m_rotation; }
    
    void setScale(float sx, float sy);
    void setScale(const Vec2& scale);
    Vec2 getScale() const { return m_scale; }
    
    // ========================================================================
    // VISIBILITY (Compatibility with legacy Node2D)
    // ========================================================================
    
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    
    // Global transform (compatibility - simplified without parent hierarchy)
    Vec2 getGlobalPosition() const { return m_position; }
    float getGlobalRotation() const { return m_rotation; }
    Vec2 getGlobalScale() const { return m_scale; }
    
    // ========================================================================
    // LIFECYCLE HOOKS
    // ========================================================================
    
    /**
     * @brief Called when actor is added to scene
     */
    virtual void onActorBegin() {}
    
    /**
     * @brief Called when actor is removed from scene
     */
    virtual void onActorEnd() {}
    
    // ========================================================================
    // LIFECYCLE (Override from Object)
    // ========================================================================
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
protected:
    Vec2 m_position{0.0f, 0.0f};
    float m_rotation = 0.0f;
    Vec2 m_scale{1.0f, 1.0f};
    bool m_visible = true;
};

} // namespace engine
