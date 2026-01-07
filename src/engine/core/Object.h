/**
 * @file Object.h
 * @brief Base class for all objects in the engine
 * 
 * Root of the object hierarchy. All game objects inherit from this.
 * Inspired by UObject from Unreal Engine.
 */
#pragma once

#include <string>
#include <SDL.h>

namespace engine {

/**
 * @brief Root base class for all engine objects
 * 
 * Provides:
 * - Name management
 * - Active state
 * - Lifecycle interface (update/render)
 * 
 * All actors, components, and game objects inherit from this.
 */
class Object {
public:
    Object(const std::string& name);
    virtual ~Object() = default;
    
    // ========================================================================
    // NAME MANAGEMENT
    // ========================================================================
    
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    // ========================================================================
    // STATE
    // ========================================================================
    
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
    
    // ========================================================================
    // LIFECYCLE (Pure Virtual)
    // ========================================================================
    
    /**
     * @brief Update object logic
     * @param deltaTime Time since last frame in seconds
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * @brief Render object
     * @param renderer SDL renderer
     */
    virtual void render(SDL_Renderer* renderer) = 0;
    
protected:
    std::string m_name;
    bool m_active = true;
};

} // namespace engine
