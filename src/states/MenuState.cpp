/**
 * @file MenuState.cpp
 * @brief Implementation av huvudmeny
 */
#include "MenuState.h"
#include "StateManager.h"
#include "PlayState.h"
#include "../Game.h"
#include "../audio/AudioManager.h"
#include <iostream>

MenuState::MenuState() {
    std::cout << "MenuState created" << std::endl;
}

void MenuState::enter() {
    std::cout << "MenuState::enter()" << std::endl;
    m_selectedOption = 0;
}

void MenuState::exit() {
    std::cout << "MenuState::exit()" << std::endl;
}

void MenuState::update(float deltaTime) {
    (void)deltaTime;
}

void MenuState::render(SDL_Renderer* renderer) {
    // Bakgrund
    SDL_SetRenderDrawColor(renderer, 20, 20, 40, 255);
    SDL_RenderClear(renderer);
    
    // Titel
    SDL_Rect titleRect = { 220, 80, 200, 40 };
    SDL_SetRenderDrawColor(renderer, 200, 180, 100, 255);
    SDL_RenderFillRect(renderer, &titleRect);
    
    // Menyalternativ
    int startY = 180;
    for (int i = 0; i < NUM_OPTIONS; i++) {
        renderText(renderer, m_options[i], 270, startY + i * 60, i == m_selectedOption);
    }
    
    // Instruktioner
    SDL_Rect instructRect = { 200, 350, 240, 20 };
    SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
    SDL_RenderFillRect(renderer, &instructRect);
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
    (void)text;
    
    SDL_Rect rect = { x, y, 100, 30 };
    
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 50, 255);
        SDL_Rect highlight = { x - 10, y - 5, 120, 40 };
        SDL_RenderFillRect(renderer, &highlight);
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 150, 150, 170, 255);
    }
    
    SDL_RenderFillRect(renderer, &rect);
}
