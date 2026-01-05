/**
 * @file PauseState.h
 * @brief Pausmeny som läggs ovanpå PlayState
 */
#pragma once

#include "IState.h"
#include <string>

/**
 * @brief Pausmeny - overlay på gameplay
 * 
 * Pushas ovanpå PlayState, ESC eller Resume återgår.
 */
class PauseState : public IState {
public:
    PauseState();
    ~PauseState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    int m_selectedOption = 0;
    static constexpr int NUM_OPTIONS = 3;
    std::string m_options[NUM_OPTIONS] = { "Resume", "Options", "Quit to Menu" };
    
    void renderOption(SDL_Renderer* renderer, int index, int y, bool selected);
};
