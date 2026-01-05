/**
 * @file PlayerCharacter.h
 * @brief Spelarkaraktären som användaren kontrollerar
 */
#pragma once

#include "Character.h"
#include <memory>

class SpriteSheet;
class Animation;

/**
 * @brief Spelarkaraktären med input-hantering
 * 
 * Stödjer både tangentbord (WASD) och point-and-click rörelse.
 */
class PlayerCharacter : public Character {
public:
    PlayerCharacter(float x, float y);
    ~PlayerCharacter() override;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    /** @brief Rendera med perspektiv-skalning */
    void renderScaled(SDL_Renderer* renderer, float scale);
    
    /** @brief Ladda sprite sheet för animationer */
    bool loadSprite(SDL_Renderer* renderer, const std::string& path);
    
    /** @brief Flytta med tangentbord (dx/dy: -1, 0, 1) */
    void move(int dx, int dy, float deltaTime);
    
    /** @brief Sätt walk area för clamping */
    void setWalkArea(int minX, int maxX, int minY, int maxY);

private:
    void clampToWalkArea() override;
    void updateAnimation(float deltaTime);
    void renderPlaceholder(SDL_Renderer* renderer);
    
    std::unique_ptr<SpriteSheet> m_spriteSheet;
    std::unique_ptr<Animation> m_walkAnim;
    std::unique_ptr<Animation> m_idleAnim;
    
    // Walk area bounds
    int m_walkMinX, m_walkMaxX;
    int m_walkMinY, m_walkMaxY;
};
