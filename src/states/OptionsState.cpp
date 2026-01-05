/**
 * @file OptionsState.cpp
 * @brief Implementation av inställningsmeny
 */
#include "OptionsState.h"
#include "MenuState.h"
#include "../Game.h"
#include "../VideoSettings.h"
#include "../audio/AudioManager.h"
#include "../graphics/FontManager.h"
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
    FontManager::instance().renderTextCentered(renderer, "title", 
        "OPTIONS", 320, 50, {180, 160, 100, 255});
    
    // Resolution
    std::string resStr = VideoSettings::getResolutionName(VideoSettings::instance().getResolution());
    renderOption(renderer, "Resolution:", resStr, 110, m_selectedOption == 0);
    
    // Input Mode
    std::string inputStr = (m_inputMode == InputMode::KeyboardMouse) 
                           ? "Keyboard + Mouse" : "Gamepad";
    renderOption(renderer, "Input:", inputStr, 160, m_selectedOption == 1);
    
    // Music Volume
    int musicPct = static_cast<int>(m_musicVolume * 100);
    renderOption(renderer, "Music:", std::to_string(musicPct) + "%", 210, m_selectedOption == 2);
    
    // SFX Volume
    int sfxPct = static_cast<int>(m_sfxVolume * 100);
    renderOption(renderer, "SFX:", std::to_string(sfxPct) + "%", 260, m_selectedOption == 3);
    
    // Back
    renderOption(renderer, "Back", "", 320, m_selectedOption == 4);
    
    // Instruktioner
    FontManager::instance().renderTextCentered(renderer, "default",
        "LEFT/RIGHT to change, ESC to go back", 320, 380, {80, 80, 100, 255});
}

void OptionsState::renderOption(SDL_Renderer* renderer, const std::string& label,
                                 const std::string& value, int y, bool selected) {
    // Bakgrund för alternativ
    SDL_Rect rect = { 150, y, 340, 40 };
    
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        SDL_RenderFillRect(renderer, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 100, 100, 120, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    // Label text
    SDL_Color textColor = selected ? SDL_Color{40, 40, 60, 255} : SDL_Color{180, 180, 200, 255};
    FontManager::instance().renderText(renderer, "default", label, 165, y + 10, textColor);
    
    // Value text eller bar
    if (!value.empty()) {
        if (value.back() == '%') {
            // Volym-bar
            int pct = std::stoi(value.substr(0, value.length() - 1));
            SDL_Rect barBg = { 280, y + 12, 150, 16 };
            SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
            SDL_RenderFillRect(renderer, &barBg);
            
            SDL_Rect bar = { 280, y + 12, static_cast<int>(150 * pct / 100), 16 };
            SDL_SetRenderDrawColor(renderer, 100, 200, 100, 255);
            SDL_RenderFillRect(renderer, &bar);
            
            FontManager::instance().renderText(renderer, "default", value, 440, y + 10, textColor);
        } else {
            // Vanligt värde
            FontManager::instance().renderText(renderer, "default", value, 280, y + 10, textColor);
        }
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
                    // Föregående upplösning
                    VideoSettings::instance().setResolution(
                        VideoSettings::instance().prevResolution());
                } else if (m_selectedOption == 1) {
                    // Toggle input mode
                    m_inputMode = (m_inputMode == InputMode::KeyboardMouse) 
                                  ? InputMode::Gamepad : InputMode::KeyboardMouse;
                } else if (m_selectedOption == 2) {
                    m_musicVolume = std::max(0.0f, m_musicVolume - 0.1f);
                    AudioManager::instance().setMusicVolume(m_musicVolume);
                } else if (m_selectedOption == 3) {
                    m_sfxVolume = std::max(0.0f, m_sfxVolume - 0.1f);
                    AudioManager::instance().setSFXVolume(m_sfxVolume);
                }
                break;
                
            case SDL_SCANCODE_RIGHT:
            case SDL_SCANCODE_D:
                // Öka värde
                if (m_selectedOption == 0) {
                    // Nästa upplösning
                    VideoSettings::instance().setResolution(
                        VideoSettings::instance().nextResolution());
                } else if (m_selectedOption == 1) {
                    m_inputMode = (m_inputMode == InputMode::KeyboardMouse) 
                                  ? InputMode::Gamepad : InputMode::KeyboardMouse;
                } else if (m_selectedOption == 2) {
                    m_musicVolume = std::min(1.0f, m_musicVolume + 0.1f);
                    AudioManager::instance().setMusicVolume(m_musicVolume);
                } else if (m_selectedOption == 3) {
                    m_sfxVolume = std::min(1.0f, m_sfxVolume + 0.1f);
                    AudioManager::instance().setSFXVolume(m_sfxVolume);
                }
                break;
                
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_SPACE:
                if (m_selectedOption == 4) {
                    // Back to menu
                    AudioManager::instance().playSound("select");
                    if (m_game) {
                        m_game->changeState(std::make_unique<MenuState>());
                    }
                }
                break;
                
            case SDL_SCANCODE_ESCAPE:
                AudioManager::instance().playSound("select");
                if (m_game) {
                    m_game->changeState(std::make_unique<MenuState>());
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
