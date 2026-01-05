#include "Input.h"

Input::Input() {
    m_currentKeys.fill(false);
    m_previousKeys.fill(false);
}

void Input::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        m_currentKeys[event.key.keysym.scancode] = true;
    }
    else if (event.type == SDL_KEYUP) {
        m_currentKeys[event.key.keysym.scancode] = false;
    }
}

void Input::update() {
    m_previousKeys = m_currentKeys;
}

bool Input::isKeyDown(SDL_Scancode key) const {
    return m_currentKeys[key];
}

bool Input::isKeyPressed(SDL_Scancode key) const {
    return m_currentKeys[key] && !m_previousKeys[key];
}

bool Input::isKeyReleased(SDL_Scancode key) const {
    return !m_currentKeys[key] && m_previousKeys[key];
}
