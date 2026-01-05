/**
 * @file PauseState.cpp
 * @brief Implementation av pausmeny
 */
#include "PauseState.h"
#include "MenuState.h"
#include "OptionsState.h"
#include "../Game.h"
#include "../audio/AudioManager.h"
#include "../graphics/FontManager.h"
#include <iostream>

PauseState::PauseState() {
    std::cout << "PauseState created" << std::endl;
}

void PauseState::enter() {
    std::cout << "PauseState::enter()" << std::endl;
    m_selectedOption = 0;
}

void PauseState::exit() {
    std::cout << "PauseState::exit()" << std::endl;
}

void PauseState::update(float deltaTime) {
    (void)deltaTime;
}

void PauseState::render(SDL_Renderer* renderer) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = { 0, 0, 640, 400 };
    SDL_RenderFillRect(renderer, &overlay);
    
    // Pausruta
    SDL_Rect pauseBox = { 170, 100, 300, 200 };
    SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    SDL_RenderFillRect(renderer, &pauseBox);
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 100, 100, 140, 255);
    SDL_RenderDrawRect(renderer, &pauseBox);
    
    // Titel
    FontManager::instance().renderTextCentered(renderer, "title", 
        "PAUSED", 320, 115, {200, 180, 100, 255});
    
    // Menyalternativ
    for (int i = 0; i < NUM_OPTIONS; i++) {
        renderOption(renderer, i, 170 + i * 40, i == m_selectedOption);
    }
}

void PauseState::renderOption(SDL_Renderer* renderer, int index, int y, bool selected) {
    SDL_Rect rect = { 220, y, 200, 30 };
    
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        SDL_RenderFillRect(renderer, &rect);
        FontManager::instance().renderTextCentered(renderer, "default", 
            m_options[index], 320, y + 5, {40, 40, 60, 255});
    } else {
        SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
        SDL_RenderFillRect(renderer, &rect);
        FontManager::instance().renderTextCentered(renderer, "default", 
            m_options[index], 320, y + 5, {180, 180, 200, 255});
    }
}

void PauseState::handleEvent(const SDL_Event& event) {
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
                    // Resume - pop this state
                    if (m_game) {
                        m_game->popState();
                    }
                } else if (m_selectedOption == 1) {
                    // Options
                    if (m_game) {
                        m_game->pushState(std::make_unique<OptionsState>());
                    }
                } else if (m_selectedOption == 2) {
                    // Quit to Menu
                    if (m_game) {
                        m_game->changeState(std::make_unique<MenuState>());
                    }
                }
                break;
                
            case SDL_SCANCODE_ESCAPE:
                // Resume
                if (m_game) {
                    m_game->popState();
                }
                break;
                
            default:
                break;
        }
    }
}
