/**
 * @file Player.h
 * @brief Spelarkaraktär med rörelse och animation
 */
#pragma once

#include <SDL.h>

/**
 * @brief Spelarkaraktären som användaren kontrollerar
 * 
 * Hanterar rörelse, animation och rendering.
 * Position och hastighet är i pixlar/sekund.
 */
class Player {
public:
    Player(float x, float y);

    /** @brief Flytta spelaren baserat på input (-1, 0, 1) */
    void move(int dx, int dy, float deltaTime);
    
    /** @brief Uppdatera animation */
    void update(float deltaTime);
    
    /** @brief Rita spelaren */
    void render(SDL_Renderer* renderer);

    float getX() const { return m_x; }
    float getY() const { return m_y; }

private:
    float m_x, m_y;              // Position i pixlar
    float m_speed = 200.0f;      // Pixlar per sekund
    int m_width = 32;            // Sprite bredd
    int m_height = 48;           // Sprite höjd
    
    // Animation state
    int m_currentFrame = 0;      // Nuvarande frame index
    float m_animTimer = 0.0f;    // Timer för frame-byte
    float m_animSpeed = 0.1f;    // Sekunder per frame
    bool m_moving = false;       // Rör sig just nu
    bool m_facingRight = true;   // Riktning för sprite flip
};
