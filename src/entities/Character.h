/**
 * @file Character.h
 * @brief Abstract base class för alla karaktärer
 */
#pragma once

#include "Entity.h"
#include <string>

/**
 * @brief Abstrakt basklass för karaktärer (player, NPC, enemies)
 * 
 * Lägger till health, name och rörelse-funktionalitet.
 */
class Character : public Entity {
public:
    Character(float x, float y, int width, int height, const std::string& name);
    ~Character() override = default;
    
    // Rörelse
    /** @brief Sätt målposition för point-and-click rörelse */
    void setTarget(float x, float y);
    
    /** @brief Stoppa rörelse */
    void stop();
    
    /** @brief Kolla om karaktären rör sig */
    bool isMoving() const { return m_moving; }
    
    // Attribut
    const std::string& getName() const { return m_name; }
    int getHealth() const { return m_health; }
    void setHealth(int health) { m_health = health; }
    bool isAlive() const { return m_health > 0; }
    
    // Riktning
    bool isFacingRight() const { return m_facingRight; }
    
    // Speed
    float getSpeed() const { return m_speed; }
    void setSpeed(float speed) { m_speed = speed; }

protected:
    /** @brief Uppdatera rörelse mot target */
    void updateMovement(float deltaTime);
    
    /** @brief Begränsa position till walk area */
    virtual void clampToWalkArea();
    
    std::string m_name;
    int m_health = 100;
    
    // Rörelse
    float m_targetX, m_targetY;
    float m_speed = 150.0f;
    bool m_moving = false;
    bool m_hasTarget = false;
    bool m_facingRight = true;
    
    // Animation
    int m_currentFrame = 0;
    float m_animTimer = 0.0f;
    float m_animSpeed = 0.1f;
};
