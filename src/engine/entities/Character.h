/**
 * @file Character.h
 * @brief Abstract base class för alla karaktärer
 * 
 * MIGRATION NOTE: Now inherits from Pawn (UE5-style) instead of Entity
 * This allows Character to be possessed by Controllers for better separation of concerns.
 */
#pragma once

#include "engine/actors/Pawn.h"
#include <string>

/**
 * @brief Basklass för karaktärer (player, NPC, enemies)
 * 
 * Lägger till health, name och rörelse-funktionalitet.
 * Now inherits from Pawn to support Controller possession.
 */
class Character : public engine::Pawn {
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
    
    // Walk area clamping
    void setWalkArea(int minX, int maxX, int minY, int maxY);
    virtual void clampToWalkArea();
    
    // Dimensions (from old Entity class)
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    void setWidth(int w) { m_width = w; }
    void setHeight(int h) { m_height = h; }
    
protected:
    /** @brief Uppdatera rörelse mot target */
    void updateMovement(float deltaTime);
    
    std::string m_name;
    
    // Physics/dimensions (moved from Entity)
    float m_x = 0.0f;
    float m_y = 0.0f;
    int m_width = 32;
    int m_height = 32;
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
