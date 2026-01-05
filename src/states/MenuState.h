#pragma once

#include "IState.h"
#include <string>

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
    int m_selectedOption = 0;
    static constexpr int NUM_OPTIONS = 2;
    std::string m_options[NUM_OPTIONS] = { "Start Game", "Quit" };
    
    void renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, bool selected);
};
