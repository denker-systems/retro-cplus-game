/**
 * @file QuestLogState.h
 * @brief Quest Log UI overlay
 */
#pragma once

#include "IState.h"

/**
 * @brief Visar quest log som overlay
 */
class QuestLogState : public IState {
public:
    QuestLogState();
    ~QuestLogState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    int m_selectedQuest = 0;
    int m_scrollOffset = 0;
};
