/**
 * @file Player.h
 * @brief Spelarkaraktär med point-and-click rörelse
 */
#pragma once

#include <SDL.h>

/**
 * @brief Spelarkaraktären som användaren kontrollerar
 * 
 * Stödjer både tangentbord (WASD) och point-and-click rörelse.
 */
class Player {
public:
    Player(float x, float y);

    /** @brief Flytta spelaren baserat på input (-1, 0, 1) */
    void move(int dx, int dy, float deltaTime);
    
    /** @brief Sätt mål för point-and-click rörelse */
    void setTarget(float x, float y);
    
    /** @brief Avbryt pågående rörelse */
    void stop();
    
    /** @brief Uppdatera rörelse och animation */
    void update(float deltaTime);
    
    /** @brief Rita spelaren */
    void render(SDL_Renderer* renderer);

    float getX() const { return m_x; }
    float getY() const { return m_y; }
    bool isMoving() const { return m_moving; }

private:
    void clampToWalkArea();      // Begränsa till gångbart område
    
    float m_x, m_y;              // Nuvarande position
    float m_targetX, m_targetY;  // Mål för point-and-click
    float m_speed = 150.0f;      // Pixlar per sekund
    int m_width = 32;
    int m_height = 48;
    
    // Animation
    int m_currentFrame = 0;
    float m_animTimer = 0.0f;
    float m_animSpeed = 0.1f;
    bool m_moving = false;
    bool m_facingRight = true;
    bool m_hasTarget = false;    // Har ett point-and-click mål
};
