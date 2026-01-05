/**
 * @file MenuState.cpp
 * @brief Implementation av huvudmeny
 */
#include "MenuState.h"
#include "StateManager.h"
#include "PlayState.h"
#include "EditorState.h"
#include "OptionsState.h"
#include "../Game.h"
#include "../audio/AudioManager.h"
#include "../graphics/FontManager.h"
#include <iostream>

MenuState::MenuState() {
    std::cout << "MenuState created" << std::endl;
}

void MenuState::enter() {
    std::cout << "MenuState::enter()" << std::endl;
    m_selectedOption = 0;
    
    // Ladda och spela menymusik
    AudioManager::instance().loadMusic("menu", "assets/sounds/MAINMENU.wav");
    AudioManager::instance().playMusic("menu");
}

void MenuState::exit() {
    std::cout << "MenuState::exit()" << std::endl;
    // Fade ut menymusik
    AudioManager::instance().fadeOutMusic(500);
}

void MenuState::update(float deltaTime) {
    (void)deltaTime;
}

void MenuState::render(SDL_Renderer* renderer) {
    // Bakgrund
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);
    
    // Titel
    FontManager::instance().renderTextCentered(renderer, "title", 
        "RETRO ADVENTURE", 320, 80, {200, 180, 100, 255});
    
    // Menyalternativ
    int startY = 180;
    for (int i = 0; i < NUM_OPTIONS; i++) {
        renderText(renderer, m_options[i], 320, startY + i * 50, i == m_selectedOption);
    }
    
    // Mute status
    bool muted = AudioManager::instance().isMuted();
    SDL_Color muteColor = muted ? SDL_Color{255, 100, 100, 255} : SDL_Color{100, 255, 100, 255};
    std::string muteText = muted ? "[M] Sound: OFF" : "[M] Sound: ON";
    FontManager::instance().renderText(renderer, "default", muteText, 10, 375, muteColor);
    
    // Instruktioner
    FontManager::instance().renderTextCentered(renderer, "default",
        "Use UP/DOWN and ENTER to select", 320, 360, {100, 100, 120, 255});
}

void MenuState::handleEvent(const SDL_Event& event) {
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
                
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_SPACE:
                AudioManager::instance().playSound("select");
                if (m_selectedOption == 0) {
                    // Start Game
                    if (m_game) {
                        m_game->changeState(std::make_unique<PlayState>());
                    }
                } else if (m_selectedOption == 1) {
                    // Editor
                    if (m_game) {
                        m_game->pushState(std::make_unique<EditorState>());
                    }
                } else if (m_selectedOption == 2) {
                    // Options
                    if (m_game) {
                        m_game->changeState(std::make_unique<OptionsState>());
                    }
                } else if (m_selectedOption == 3) {
                    // Quit
                    SDL_Event quitEvent;
                    quitEvent.type = SDL_QUIT;
                    SDL_PushEvent(&quitEvent);
                }
                break;
                
            default:
                break;
        }
    }
}

void MenuState::renderText(SDL_Renderer* renderer, const std::string& text, int x, int y, bool selected) {
    int w, h;
    FontManager::instance().getTextSize("default", text, &w, &h);
    
    if (selected) {
        // Highlight bakgrund
        SDL_Rect highlight = { x - w/2 - 15, y - 5, w + 30, h + 10 };
        SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        SDL_RenderFillRect(renderer, &highlight);
        SDL_SetRenderDrawColor(renderer, 100, 100, 140, 255);
        SDL_RenderDrawRect(renderer, &highlight);
        
        // Text (mörk på ljus bakgrund)
        FontManager::instance().renderTextCentered(renderer, "default", text, x, y, {40, 40, 60, 255});
    } else {
        // Normal text
        FontManager::instance().renderTextCentered(renderer, "default", text, x, y, {180, 180, 200, 255});
    }
}
