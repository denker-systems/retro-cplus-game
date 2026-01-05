/**
 * @file Input.cpp
 * @brief Implementation av tangentbordshantering
 */
#include "Input.h"

Input::Input() {
    m_currentKeys.fill(false);
    m_previousKeys.fill(false);
}

void Input::handleEvent(const SDL_Event& event) {
    // Ignorera key repeat events (när tangent hålls nere)
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        m_currentKeys[event.key.keysym.scancode] = true;
    }
    else if (event.type == SDL_KEYUP) {
        m_currentKeys[event.key.keysym.scancode] = false;
    }
}

void Input::update() {
    // Spara nuvarande state som föregående för nästa frame
    m_previousKeys = m_currentKeys;
}

bool Input::isKeyDown(SDL_Scancode key) const {
    return m_currentKeys[key];
}

bool Input::isKeyPressed(SDL_Scancode key) const {
    // True endast om tangent är nere nu men inte förra framen
    return m_currentKeys[key] && !m_previousKeys[key];
}

bool Input::isKeyReleased(SDL_Scancode key) const {
    // True endast om tangent var nere förra framen men inte nu
    return !m_currentKeys[key] && m_previousKeys[key];
}
