/**
 * @file Input.h
 * @brief Tangentbord, mus och gamepad-hantering
 */
#pragma once

#include <SDL.h>
#include <array>

/** @brief Vilket input-läge som är aktivt */
enum class InputMode {
    KeyboardMouse,
    Gamepad
};

/**
 * @brief Hanterar tangentbord, mus och Xbox-gamepad
 * 
 * Stödjer automatisk växling mellan input-lägen.
 */
class Input {
public:
    Input();
    ~Input();

    /** @brief Uppdatera input state från SDL event */
    void handleEvent(const SDL_Event& event);
    
    /** @brief Anropa i slutet av varje frame */
    void update();
    
    /** @brief Sätt aktivt input-läge */
    void setInputMode(InputMode mode) { m_inputMode = mode; }
    InputMode getInputMode() const { return m_inputMode; }

    // Tangentbord
    bool isKeyDown(SDL_Scancode key) const;
    bool isKeyPressed(SDL_Scancode key) const;
    bool isKeyReleased(SDL_Scancode key) const;
    
    // Mus
    int getMouseX() const { return m_mouseX; }
    int getMouseY() const { return m_mouseY; }
    bool isMouseDown(int button) const;
    bool isMouseClicked(int button) const;
    bool isMouseReleased(int button) const;
    
    // Gamepad (Xbox-layout)
    bool isGamepadConnected() const { return m_gamepad != nullptr; }
    
    /** @brief Gamepad-knapp är nere */
    bool isButtonDown(SDL_GameControllerButton button) const;
    
    /** @brief Gamepad-knapp trycktes just */
    bool isButtonPressed(SDL_GameControllerButton button) const;
    
    /** @brief Gamepad-knapp släpptes just */
    bool isButtonReleased(SDL_GameControllerButton button) const;
    
    /** @brief Hämta vänster stick X (-1.0 till 1.0) */
    float getLeftStickX() const { return m_leftStickX; }
    
    /** @brief Hämta vänster stick Y (-1.0 till 1.0) */
    float getLeftStickY() const { return m_leftStickY; }
    
    /** @brief Hämta höger stick X */
    float getRightStickX() const { return m_rightStickX; }
    
    /** @brief Hämta höger stick Y */
    float getRightStickY() const { return m_rightStickY; }

private:
    void openGamepad();
    void closeGamepad();
    float normalizeAxis(Sint16 value) const;
    
    InputMode m_inputMode = InputMode::KeyboardMouse;
    
    // Tangentbord
    std::array<bool, SDL_NUM_SCANCODES> m_currentKeys{};
    std::array<bool, SDL_NUM_SCANCODES> m_previousKeys{};
    
    // Mus
    int m_mouseX = 0;
    int m_mouseY = 0;
    std::array<bool, 5> m_currentMouse{};
    std::array<bool, 5> m_previousMouse{};
    
    // Gamepad
    SDL_GameController* m_gamepad = nullptr;
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> m_currentButtons{};
    std::array<bool, SDL_CONTROLLER_BUTTON_MAX> m_previousButtons{};
    float m_leftStickX = 0.0f;
    float m_leftStickY = 0.0f;
    float m_rightStickX = 0.0f;
    float m_rightStickY = 0.0f;
    static constexpr float DEADZONE = 0.15f;  // Stick deadzone
};
