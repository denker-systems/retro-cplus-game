/**
 * @file Input.cpp
 * @brief Implementation av tangentbord, mus och gamepad-hantering
 */
#include "Input.h"
#include <iostream>
#include <cmath>

Input::Input() {
    m_currentKeys.fill(false);
    m_previousKeys.fill(false);
    m_currentMouse.fill(false);
    m_previousMouse.fill(false);
    m_currentButtons.fill(false);
    m_previousButtons.fill(false);
    
    // Försök öppna gamepad om någon är ansluten
    openGamepad();
}

Input::~Input() {
    closeGamepad();
}

void Input::openGamepad() {
    // Kolla efter anslutna gamepads
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            m_gamepad = SDL_GameControllerOpen(i);
            if (m_gamepad) {
                std::cout << "Gamepad connected: " 
                          << SDL_GameControllerName(m_gamepad) << std::endl;
                return;
            }
        }
    }
}

void Input::closeGamepad() {
    if (m_gamepad) {
        SDL_GameControllerClose(m_gamepad);
        m_gamepad = nullptr;
    }
}

float Input::normalizeAxis(Sint16 value) const {
    float normalized = value / 32767.0f;
    // Applicera deadzone
    if (std::abs(normalized) < DEADZONE) {
        return 0.0f;
    }
    return normalized;
}

void Input::handleEvent(const SDL_Event& event) {
    // Tangentbord
    if (event.type == SDL_KEYDOWN && !event.key.repeat) {
        m_currentKeys[event.key.keysym.scancode] = true;
    }
    else if (event.type == SDL_KEYUP) {
        m_currentKeys[event.key.keysym.scancode] = false;
    }
    
    // Mus - position
    else if (event.type == SDL_MOUSEMOTION) {
        m_mouseX = event.motion.x;
        m_mouseY = event.motion.y;
    }
    // Mus - knappar
    else if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button <= 5) {
            m_currentMouse[event.button.button - 1] = true;
        }
        m_mouseX = event.button.x;
        m_mouseY = event.button.y;
    }
    else if (event.type == SDL_MOUSEBUTTONUP) {
        if (event.button.button <= 5) {
            m_currentMouse[event.button.button - 1] = false;
        }
    }
    
    // Gamepad ansluten/bortkopplad
    else if (event.type == SDL_CONTROLLERDEVICEADDED) {
        if (!m_gamepad) {
            openGamepad();
        }
    }
    else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
        if (m_gamepad && event.cdevice.which == SDL_JoystickInstanceID(
                SDL_GameControllerGetJoystick(m_gamepad))) {
            closeGamepad();
            std::cout << "Gamepad disconnected" << std::endl;
        }
    }
    
    // Gamepad knappar
    else if (event.type == SDL_CONTROLLERBUTTONDOWN) {
        m_currentButtons[event.cbutton.button] = true;
    }
    else if (event.type == SDL_CONTROLLERBUTTONUP) {
        m_currentButtons[event.cbutton.button] = false;
    }
    
    // Gamepad axlar (sticks och triggers)
    else if (event.type == SDL_CONTROLLERAXISMOTION) {
        switch (event.caxis.axis) {
            case SDL_CONTROLLER_AXIS_LEFTX:
                m_leftStickX = normalizeAxis(event.caxis.value);
                break;
            case SDL_CONTROLLER_AXIS_LEFTY:
                m_leftStickY = normalizeAxis(event.caxis.value);
                break;
            case SDL_CONTROLLER_AXIS_RIGHTX:
                m_rightStickX = normalizeAxis(event.caxis.value);
                break;
            case SDL_CONTROLLER_AXIS_RIGHTY:
                m_rightStickY = normalizeAxis(event.caxis.value);
                break;
        }
    }
}

void Input::update() {
    m_previousKeys = m_currentKeys;
    m_previousMouse = m_currentMouse;
    m_previousButtons = m_currentButtons;
}

// Tangentbord
bool Input::isKeyDown(SDL_Scancode key) const {
    return m_currentKeys[key];
}

bool Input::isKeyPressed(SDL_Scancode key) const {
    return m_currentKeys[key] && !m_previousKeys[key];
}

bool Input::isKeyReleased(SDL_Scancode key) const {
    return !m_currentKeys[key] && m_previousKeys[key];
}

// Mus
bool Input::isMouseDown(int button) const {
    if (button < 1 || button > 5) return false;
    return m_currentMouse[button - 1];
}

bool Input::isMouseClicked(int button) const {
    if (button < 1 || button > 5) return false;
    return m_currentMouse[button - 1] && !m_previousMouse[button - 1];
}

bool Input::isMouseReleased(int button) const {
    if (button < 1 || button > 5) return false;
    return !m_currentMouse[button - 1] && m_previousMouse[button - 1];
}

// Gamepad
bool Input::isButtonDown(SDL_GameControllerButton button) const {
    if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX) return false;
    return m_currentButtons[button];
}

bool Input::isButtonPressed(SDL_GameControllerButton button) const {
    if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX) return false;
    return m_currentButtons[button] && !m_previousButtons[button];
}

bool Input::isButtonReleased(SDL_GameControllerButton button) const {
    if (button < 0 || button >= SDL_CONTROLLER_BUTTON_MAX) return false;
    return !m_currentButtons[button] && m_previousButtons[button];
}
