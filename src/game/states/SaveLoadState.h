/**
 * @file SaveLoadState.h
 * @brief Save/Load meny state
 */
#pragma once

#include "IState.h"
#include <string>
#include <vector>

/**
 * @brief Save/Load meny - kan öppnas från pausmenyn
 */
class SaveLoadState : public IState {
public:
    enum class Mode { Save, Load };
    
    explicit SaveLoadState(Mode mode);
    ~SaveLoadState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    void refreshSlots();
    void renderSlot(SDL_Renderer* renderer, int index, int y, bool selected);
    void performAction();  // Save eller Load beroende på mode
    
    Mode m_mode;
    int m_selectedSlot = 0;
    
    static constexpr int NUM_SLOTS = 5;
    
    struct SlotInfo {
        std::string name;
        std::string timestamp;
        int playTimeSeconds = 0;
        bool isEmpty = true;
    };
    std::vector<SlotInfo> m_slots;
};
