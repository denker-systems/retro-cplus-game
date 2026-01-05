/**
 * @file NPC.cpp
 * @brief Implementation av NPC
 */
#include "NPC.h"
#include <SDL.h>

NPC::NPC(float x, float y, const std::string& name)
    : Character(x, y, 32, 48, name) {
}

void NPC::update(float deltaTime) {
    // NPC kan ha idle animation eller enkla beteenden
    m_animTimer += deltaTime;
    if (m_animTimer >= m_animSpeed) {
        m_animTimer = 0.0f;
        m_currentFrame = (m_currentFrame + 1) % 2;  // Enkel idle
    }
}

void NPC::render(SDL_Renderer* renderer) {
    // Placeholder - grön rektangel för NPC
    SDL_Rect rect = {
        static_cast<int>(m_x),
        static_cast<int>(m_y),
        m_width,
        m_height
    };
    
    SDL_SetRenderDrawColor(renderer, 50, 180, 80, 255);
    SDL_RenderFillRect(renderer, &rect);
    
    // Enkel "huvud"
    SDL_Rect head = {
        static_cast<int>(m_x) + 8,
        static_cast<int>(m_y) - 10,
        16, 16
    };
    SDL_SetRenderDrawColor(renderer, 220, 180, 140, 255);
    SDL_RenderFillRect(renderer, &head);
}
