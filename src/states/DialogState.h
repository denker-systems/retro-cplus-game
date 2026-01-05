/**
 * @file DialogState.h
 * @brief State för att visa dialoger som overlay
 */
#pragma once

#include "IState.h"
#include <string>

/**
 * @brief Visar dialog som overlay över PlayState
 */
class DialogState : public IState {
public:
    DialogState();
    ~DialogState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    void renderDialogBox(SDL_Renderer* renderer);
    void renderChoices(SDL_Renderer* renderer);
    
    int m_selectedChoice = 0;
};
