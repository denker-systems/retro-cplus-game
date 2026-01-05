#pragma once

#include <SDL.h>
#include <array>

class Input {
public:
    Input();

    void handleEvent(const SDL_Event& event);
    void update();

    bool isKeyDown(SDL_Scancode key) const;
    bool isKeyPressed(SDL_Scancode key) const;
    bool isKeyReleased(SDL_Scancode key) const;

private:
    std::array<bool, SDL_NUM_SCANCODES> m_currentKeys{};
    std::array<bool, SDL_NUM_SCANCODES> m_previousKeys{};
};
