/**
 * @file Transition.h
 * @brief Skärmövergångar (fade in/out)
 */
#pragma once

#include <SDL.h>
#include <functional>

/**
 * @brief Typ av transition
 */
enum class TransitionType {
    None,
    FadeIn,
    FadeOut
};

/**
 * @brief Hanterar fade transitions
 */
class Transition {
public:
    static Transition& instance();
    
    /** @brief Starta fade out, kör callback, sedan fade in */
    void fadeToBlack(float duration, std::function<void()> onMidpoint);
    
    /** @brief Uppdatera transition */
    void update(float deltaTime);
    
    /** @brief Rendera overlay */
    void render(SDL_Renderer* renderer);
    
    /** @brief Är transition aktiv? */
    bool isActive() const { return m_type != TransitionType::None; }

private:
    Transition() = default;
    
    TransitionType m_type = TransitionType::None;
    float m_alpha = 0.0f;
    float m_duration = 0.5f;
    float m_timer = 0.0f;
    std::function<void()> m_onMidpoint;
    bool m_midpointCalled = false;
};
