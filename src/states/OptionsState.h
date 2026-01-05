/**
 * @file OptionsState.h
 * @brief Inställningar - input-läge, volym, etc.
 */
#pragma once

#include "IState.h"
#include "../Input.h"
#include <string>
#include <memory>

/**
 * @brief Inställningsmeny för spelkonfiguration
 */
class OptionsState : public IState {
public:
    OptionsState();
    ~OptionsState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    void renderOption(SDL_Renderer* renderer, const std::string& label, 
                      const std::string& value, int y, bool selected);
    
    std::unique_ptr<Input> m_input;
    int m_selectedOption = 0;
    
    // Inställningar
    InputMode m_inputMode = InputMode::KeyboardMouse;
    float m_musicVolume = 0.7f;
    float m_sfxVolume = 1.0f;
    
    static constexpr int NUM_OPTIONS = 4;  // Input, Music, SFX, Back
};
