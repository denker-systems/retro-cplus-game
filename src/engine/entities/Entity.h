/**
 * @file Entity.h
 * @brief DEPRECATED: Forwarding header for legacy compatibility
 * 
 * USE INSTEAD: engine::ActorObject (in engine/core/ActorObject.h)
 * 
 * This file is kept for backward compatibility during migration.
 * All new code should use engine::ActorObject directly.
 * 
 * MIGRATION NOTE: Entity is now a forwarding alias to engine::ActorObject
 */
#pragma once

#include "engine/core/ActorObject.h"

// DEPRECATED: Use engine::ActorObject instead
using Entity = engine::ActorObject;

// Legacy class definition kept for reference (DO NOT USE)
#if 0
#include <SDL.h>

/**
 * @brief DEPRECATED: Abstrakt basklass för alla entities i spelet
 * 
 * Alla spelobjekt (characters, items, hotspots) ärver från denna.
 * Definierar gemensamt interface för update och render.
 */
class Entity {
public:
    Entity(float x, float y, int width, int height);
    virtual ~Entity() = default;
    
    /** @brief Uppdatera entity (pure virtual) */
    virtual void update(float deltaTime) = 0;
    
    /** @brief Rita entity (pure virtual) */
    virtual void render(SDL_Renderer* renderer) = 0;
    
    // Position
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    void setPosition(float x, float y) { m_x = x; m_y = y; }
    
    // Storlek
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    // Status
    bool isActive() const { return m_active; }
    void setActive(bool active) { m_active = active; }
    
    /** @brief Hämta bounding box för kollision */
    SDL_Rect getBounds() const;

protected:
    float m_x, m_y;          // Position
    int m_width, m_height;   // Storlek
    bool m_active = true;    // Om entity ska uppdateras/ritas
};
#endif
