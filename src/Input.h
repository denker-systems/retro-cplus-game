/**
 * @file Input.h
 * @brief Tangentbordshantering med state tracking
 */
#pragma once

#include <SDL.h>
#include <array>

/**
 * @brief Hanterar tangentbordsinput med press/release detection
 * 
 * Spårar både nuvarande och föregående frame för att
 * detektera nytryckta och nysläppta tangenter.
 */
class Input {
public:
    Input();

    /** @brief Uppdatera tangentstate från SDL event */
    void handleEvent(const SDL_Event& event);
    
    /** @brief Anropa i slutet av varje frame för att spara state */
    void update();

    /** @brief Tangent är nere (hålls) */
    bool isKeyDown(SDL_Scancode key) const;
    
    /** @brief Tangent trycktes just ner (denna frame) */
    bool isKeyPressed(SDL_Scancode key) const;
    
    /** @brief Tangent släpptes just (denna frame) */
    bool isKeyReleased(SDL_Scancode key) const;

private:
    std::array<bool, SDL_NUM_SCANCODES> m_currentKeys{};   // Nuvarande frame
    std::array<bool, SDL_NUM_SCANCODES> m_previousKeys{};  // Föregående frame
};
