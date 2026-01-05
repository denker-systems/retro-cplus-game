/**
 * @file Player.cpp
 * @brief Implementation av spelarkaraktär
 */
#include "Player.h"

Player::Player(float x, float y) : m_x(x), m_y(y) {}

void Player::move(int dx, int dy, float deltaTime) {
    m_moving = (dx != 0 || dy != 0);
    
    // Uppdatera riktning för sprite flip
    if (dx != 0) {
        m_facingRight = (dx > 0);
    }

    // Frame-oberoende rörelse (pixlar/sekund * sekunder)
    m_x += dx * m_speed * deltaTime;
    m_y += dy * m_speed * deltaTime;

    // Begränsa till walk area
    if (m_x < 0) m_x = 0;
    if (m_x > 640 - m_width) m_x = 640 - m_width;
    if (m_y < 200) m_y = 200;                        // Övre gräns (walk area)
    if (m_y > 400 - m_height - 25) m_y = 400 - m_height - 25;  // UI längst ner
}

void Player::update(float deltaTime) {
    // Uppdatera walk-animation endast vid rörelse
    if (m_moving) {
        m_animTimer += deltaTime;
        if (m_animTimer >= m_animSpeed) {
            m_animTimer = 0;
            m_currentFrame = (m_currentFrame + 1) % 4;  // 4 frames i walk cycle
        }
    } else {
        // Återställ till idle frame
        m_currentFrame = 0;
        m_animTimer = 0;
    }
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
