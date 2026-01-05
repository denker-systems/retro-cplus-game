/**
 * @file Player.cpp
 * @brief Implementation av spelarkaraktär med point-and-click
 */
#include "Player.h"
#include <cmath>

Player::Player(float x, float y) 
    : m_x(x), m_y(y), m_targetX(x), m_targetY(y) {}

void Player::move(int dx, int dy, float deltaTime) {
    // Tangentbord avbryter point-and-click
    if (dx != 0 || dy != 0) {
        m_hasTarget = false;
        m_moving = true;
        
        if (dx != 0) {
            m_facingRight = (dx > 0);
        }

        m_x += dx * m_speed * deltaTime;
        m_y += dy * m_speed * deltaTime;
        
        // Begränsa till walk area
        clampToWalkArea();
    }
}

void Player::setTarget(float x, float y) {
    // Centrera målet på spelarens fötter
    m_targetX = x - m_width / 2;
    m_targetY = y - m_height;
    m_hasTarget = true;
    
    // Uppdatera riktning direkt
    m_facingRight = (m_targetX > m_x);
}

void Player::stop() {
    m_hasTarget = false;
    m_moving = false;
}

void Player::update(float deltaTime) {
    // Point-and-click rörelse
    if (m_hasTarget) {
        float dx = m_targetX - m_x;
        float dy = m_targetY - m_y;
        float distance = std::sqrt(dx * dx + dy * dy);
        
        // Nära nog? Stanna.
        if (distance < 5.0f) {
            m_hasTarget = false;
            m_moving = false;
        } else {
            m_moving = true;
            // Normalisera och flytta
            float moveX = (dx / distance) * m_speed * deltaTime;
            float moveY = (dy / distance) * m_speed * deltaTime;
            
            m_x += moveX;
            m_y += moveY;
            
            m_facingRight = (dx > 0);
            clampToWalkArea();
        }
    }
    
    // Animation
    if (m_moving) {
        m_animTimer += deltaTime;
        if (m_animTimer >= m_animSpeed) {
            m_animTimer = 0;
            m_currentFrame = (m_currentFrame + 1) % 4;
        }
    } else {
        m_currentFrame = 0;
        m_animTimer = 0;
    }
}

void Player::clampToWalkArea() {
    if (m_x < 0) m_x = 0;
    if (m_x > 640 - m_width) m_x = 640 - m_width;
    if (m_y < 260) m_y = 260;                          // Walk area övre gräns
    if (m_y > 400 - m_height - 25) m_y = 400 - m_height - 25;
}

void Player::render(SDL_Renderer* renderer) {
    // TODO: Ersätt med SpriteSheet när sprites finns
    // Temporär placeholder: Rita en enkel karaktär med rektanglar
    
    SDL_Rect rect = {
        static_cast<int>(m_x),
        static_cast<int>(m_y),
        m_width,
        m_height
    };

    // Kropp (ljusgrå)
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Huvud (hudfärg)
    SDL_Rect head = {
        static_cast<int>(m_x) + 8,
        static_cast<int>(m_y) + 2,
        16,
        14
    };
    SDL_SetRenderDrawColor(renderer, 240, 200, 160, 255);
    SDL_RenderFillRect(renderer, &head);

    // Byxor (mörkgrå)
    SDL_Rect pants = {
        static_cast<int>(m_x) + 4,
        static_cast<int>(m_y) + 24,
        24,
        16
    };
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &pants);
}
