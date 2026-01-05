/**
 * @file PlayerCharacter.h
 * @brief Spelarkaraktären som användaren kontrollerar
 */
#pragma once

#include "Character.h"

/**
 * @brief Spelarkaraktären med input-hantering
 * 
 * Stödjer både tangentbord (WASD) och point-and-click rörelse.
 */
class PlayerCharacter : public Character {
public:
    PlayerCharacter(float x, float y);
    ~PlayerCharacter() override = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    /** @brief Flytta med tangentbord (dx/dy: -1, 0, 1) */
    void move(int dx, int dy, float deltaTime);

private:
    void updateAnimation(float deltaTime);
    void renderPlaceholder(SDL_Renderer* renderer);
};
