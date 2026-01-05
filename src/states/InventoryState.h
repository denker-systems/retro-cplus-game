/**
 * @file InventoryState.h
 * @brief Inventory UI overlay
 */
#pragma once

#include "IState.h"

/**
 * @brief Visar inventory som overlay
 */
class InventoryState : public IState {
public:
    InventoryState();
    ~InventoryState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    void renderItemSlot(SDL_Renderer* renderer, int index, int x, int y, bool selected);
    
    int m_selectedIndex = 0;
};
