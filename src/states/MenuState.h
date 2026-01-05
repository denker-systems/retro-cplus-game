/**
 * @file MenuState.h
 * @brief Huvudmeny state med Start och Quit
 */
#pragma once

#include "IState.h"
#include <string>

/**
 * @brief Huvudmeny - första skärmen spelaren ser
 * 
 * Navigera med piltangenter, välj med Enter.
 */
class MenuState : public IState {
public:
    MenuState();
    ~MenuState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    int m_selectedOption = 0;  // Nuvarande markerat alternativ
    static constexpr int NUM_OPTIONS = 4;
    std::string m_options[NUM_OPTIONS] = { "Start Game", "Editor", "Options", "Quit" };
    
    /** @brief Rita menyalternativ (placeholder tills fonts finns) */
    void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, bool selected);
};
