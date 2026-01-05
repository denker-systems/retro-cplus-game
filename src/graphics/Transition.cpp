/**
 * @file Transition.cpp
 * @brief Implementation av skärmövergångar
 */
#include "Transition.h"

Transition& Transition::instance() {
    static Transition instance;
    return instance;
}

void Transition::fadeToBlack(float duration, std::function<void()> onMidpoint) {
    m_type = TransitionType::FadeOut;
    m_duration = duration / 2.0f;  // Halva tiden för fade out, halva för fade in
    m_timer = 0.0f;
    m_alpha = 0.0f;
    m_onMidpoint = onMidpoint;
    m_midpointCalled = false;
}

void Transition::update(float deltaTime) {
    if (m_type == TransitionType::None) return;
    
    m_timer += deltaTime;
    
    if (m_type == TransitionType::FadeOut) {
        m_alpha = (m_timer / m_duration) * 255.0f;
        if (m_alpha >= 255.0f) {
            m_alpha = 255.0f;
            
            // Kör midpoint callback
            if (!m_midpointCalled && m_onMidpoint) {
                m_onMidpoint();
                m_midpointCalled = true;
            }
            
            // Byt till fade in
            m_type = TransitionType::FadeIn;
            m_timer = 0.0f;
        }
    } else if (m_type == TransitionType::FadeIn) {
        m_alpha = 255.0f - (m_timer / m_duration) * 255.0f;
        if (m_alpha <= 0.0f) {
            m_alpha = 0.0f;
            m_type = TransitionType::None;
        }
    }
}

void Transition::render(SDL_Renderer* renderer) {
    if (m_type == TransitionType::None) return;
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(m_alpha));
    SDL_Rect rect = {0, 0, 640, 400};
    SDL_RenderFillRect(renderer, &rect);
}
