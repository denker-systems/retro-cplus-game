/**
 * @file Entity.h
 * @brief Abstract base class för alla spelobjekt
 */
#pragma once

#include <SDL.h>

/**
 * @brief Abstrakt basklass för alla entities i spelet
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
