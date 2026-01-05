/**
 * @file PlayerCharacter.cpp
 * @brief Implementation av spelarkaraktär
 */
#include "PlayerCharacter.h"

PlayerCharacter::PlayerCharacter(float x, float y)
    : Character(x, y, 32, 48, "Player") {}

void PlayerCharacter::move(int dx, int dy, float deltaTime) {
    if (dx != 0 || dy != 0) {
        m_hasTarget = false;
        m_moving = true;
        
        if (dx != 0) {
            m_facingRight = (dx > 0);
        }
        
        m_x += dx * m_speed * deltaTime;
        m_y += dy * m_speed * deltaTime;
        clampToWalkArea();
    }
}

void PlayerCharacter::update(float deltaTime) {
    updateMovement(deltaTime);
    updateAnimation(deltaTime);
}

void PlayerCharacter::updateAnimation(float deltaTime) {
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

void PlayerCharacter::render(SDL_Renderer* renderer) {
    renderPlaceholder(renderer);
}

void PlayerCharacter::renderPlaceholder(SDL_Renderer* renderer) {
    // TODO: Ersätt med SpriteSheet
    SDL_Rect rect = {
        static_cast<int>(m_x),
        static_cast<int>(m_y),
        m_width,
        m_height
    };

    // Kropp
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Huvud
    SDL_Rect head = {
        static_cast<int>(m_x) + 8,
        static_cast<int>(m_y) + 2,
        16, 14
    };
    SDL_SetRenderDrawColor(renderer, 240, 200, 160, 255);
    SDL_RenderFillRect(renderer, &head);

    // Byxor
    SDL_Rect pants = {
        static_cast<int>(m_x) + 4,
        static_cast<int>(m_y) + 24,
        24, 16
    };
    SDL_SetRenderDrawColor(renderer, 60, 60, 60, 255);
    SDL_RenderFillRect(renderer, &pants);
}
