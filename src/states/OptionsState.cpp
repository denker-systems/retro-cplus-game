/**
 * @file OptionsState.cpp
 * @brief Implementation av inställningsmeny
 */
#include "OptionsState.h"
#include "MenuState.h"
#include "../Game.h"
#include "../audio/AudioManager.h"
#include <iostream>

OptionsState::OptionsState() {
    std::cout << "OptionsState created" << std::endl;
}

void OptionsState::enter() {
    std::cout << "OptionsState::enter()" << std::endl;
    m_input = std::make_unique<Input>();
    m_selectedOption = 0;
    
    // Hämta nuvarande inställningar
    m_inputMode = m_input->getInputMode();
    m_musicVolume = AudioManager::instance().getMusicVolume();
    m_sfxVolume = AudioManager::instance().getSFXVolume();
}

void OptionsState::exit() {
    std::cout << "OptionsState::exit()" << std::endl;
    m_input.reset();
}

void OptionsState::update(float deltaTime) {
    (void)deltaTime;
    m_input->update();
}

void OptionsState::render(SDL_Renderer* renderer) {
    // Bakgrund
    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
    SDL_RenderClear(renderer);
    
    // Titel
    SDL_Rect titleRect = { 230, 60, 180, 40 };
    SDL_SetRenderDrawColor(renderer, 180, 160, 100, 255);
    SDL_RenderFillRect(renderer, &titleRect);
    
    // Input Mode
    std::string inputStr = (m_inputMode == InputMode::KeyboardMouse) 
                           ? "Keyboard + Mouse" : "Gamepad";
    renderOption(renderer, "Input:", inputStr, 140, m_selectedOption == 0);
    
    // Music Volume
    int musicPct = static_cast<int>(m_musicVolume * 100);
    renderOption(renderer, "Music:", std::to_string(musicPct) + "%", 200, m_selectedOption == 1);
    
    // SFX Volume
    int sfxPct = static_cast<int>(m_sfxVolume * 100);
    renderOption(renderer, "SFX:", std::to_string(sfxPct) + "%", 260, m_selectedOption == 2);
    
    // Back
    renderOption(renderer, "Back", "", 340, m_selectedOption == 3);
    
    // Instruktioner
    SDL_Rect instructRect = { 150, 380, 340, 15 };
    SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
    SDL_RenderFillRect(renderer, &instructRect);
}

void OptionsState::renderOption(SDL_Renderer* renderer, const std::string& label,
                                 const std::string& value, int y, bool selected) {
    (void)label;
    (void)value;
    
    // Bakgrund för alternativ
    SDL_Rect rect = { 150, y, 340, 40 };
    
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    // Värde-indikator (bar för volym)
    if (!value.empty() && value.back() == '%') {
        int pct = std::stoi(value.substr(0, value.length() - 1));
        SDL_Rect bar = { 300, y + 10, static_cast<int>(150 * pct / 100), 20 };
        SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
        SDL_RenderFillRect(renderer, &bar);
    }
}

void OptionsState::handleEvent(const SDL_Event& event) {
    m_input->handleEvent(event);
    
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                m_selectedOption = (m_selectedOption - 1 + NUM_OPTIONS) % NUM_OPTIONS;
                AudioManager::instance().playSound("navigate");
                break;
                
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                m_selectedOption = (m_selectedOption + 1) % NUM_OPTIONS;
                AudioManager::instance().playSound("navigate");
                break;
                
            case SDL_SCANCODE_LEFT:
            case SDL_SCANCODE_A:
                // Minska värde
                if (m_selectedOption == 0) {
                    // Toggle input mode
                    m_inputMode = (m_inputMode == InputMode::KeyboardMouse) 
                                  ? InputMode::Gamepad : InputMode::KeyboardMouse;
                } else if (m_selectedOption == 1) {
                    m_musicVolume = std::max(0.0f, m_musicVolume - 0.1f);
                    AudioManager::instance().setMusicVolume(m_musicVolume);
                } else if (m_selectedOption == 2) {
                    m_sfxVolume = std::max(0.0f, m_sfxVolume - 0.1f);
                    AudioManager::instance().setSFXVolume(m_sfxVolume);
                }
                break;
                
            case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_D:
                // Öka värde
                if (m_selectedOption == 0) {
                    m_inputMode = (m_inputMode == InputMode::KeyboardMouse) 
                                  ? InputMode::Gamepad : InputMode::KeyboardMouse;
                } else if (m_selectedOption == 1) {
                    m_musicVolume = std::min(1.0f, m_musicVolume + 0.1f);
                    AudioManager::instance().setMusicVolume(m_musicVolume);
                } else if (m_selectedOption == 2) {
                    m_sfxVolume = std::min(1.0f, m_sfxVolume + 0.1f);
                    AudioManager::instance().setSFXVolume(m_sfxVolume);
                }
                break;
                
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_SPACE:
            case SDL_SCANCODE_ESCAPE:
                if (m_selectedOption == 3 || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    // Back to menu
                    AudioManager::instance().playSound("select");
                    if (m_game) {
                        m_game->changeState(std::make_unique<MenuState>());
                    }
                }
                break;
                
            default:
                break;
        }
    }
    
    // Gamepad-stöd
    if (m_input->isButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_UP)) {
        m_selectedOption = (m_selectedOption - 1 + NUM_OPTIONS) % NUM_OPTIONS;
    }
    if (m_input->isButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
        m_selectedOption = (m_selectedOption + 1) % NUM_OPTIONS;
    }
    if (m_input->isButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
        if (m_selectedOption == 1) {
            m_musicVolume = std::max(0.0f, m_musicVolume - 0.1f);
            AudioManager::instance().setMusicVolume(m_musicVolume);
        } else if (m_selectedOption == 2) {
            m_sfxVolume = std::max(0.0f, m_sfxVolume - 0.1f);
            AudioManager::instance().setSFXVolume(m_sfxVolume);
        }
    }
    if (m_input->isButtonPressed(SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
        if (m_selectedOption == 1) {
            m_musicVolume = std::min(1.0f, m_musicVolume + 0.1f);
            AudioManager::instance().setMusicVolume(m_musicVolume);
        } else if (m_selectedOption == 2) {
            m_sfxVolume = std::min(1.0f, m_sfxVolume + 0.1f);
            AudioManager::instance().setSFXVolume(m_sfxVolume);
        }
    }
    if (m_input->isButtonPressed(SDL_CONTROLLER_BUTTON_A)) {
        if (m_selectedOption == 3) {
            if (m_game) {
                m_game->changeState(std::make_unique<MenuState>());
            }
        }
    }
    if (m_input->isButtonPressed(SDL_CONTROLLER_BUTTON_B)) {
        if (m_game) {
            m_game->changeState(std::make_unique<MenuState>());
        }
    }
}
