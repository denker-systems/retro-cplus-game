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
    
    /** @brief Ladda sprite sheet för animationer */
    bool loadSprite(SDL_Renderer* renderer, const std::string& path);
    
    /** @brief Flytta med tangentbord (dx/dy: -1, 0, 1) */
    void move(int dx, int dy, float deltaTime);

private:
    void updateAnimation(float deltaTime);
    void renderPlaceholder(SDL_Renderer* renderer);
    
    std::unique_ptr<SpriteSheet> m_spriteSheet;
    std::unique_ptr<Animation> m_walkAnim;
    std::unique_ptr<Animation> m_idleAnim;
};
