/**
 * @file PlayState.cpp
 * @brief Implementation av gameplay state
 */
#include "PlayState.h"
#include "MenuState.h"
#include "../Game.h"
#include "../Player.h"
#include "../Room.h"
#include "../Input.h"
#include <iostream>

PlayState::PlayState() {
    std::cout << "PlayState created" << std::endl;
}

PlayState::~PlayState() = default;

void PlayState::enter() {
    std::cout << "PlayState::enter()" << std::endl;
    
    // Skapa spelkomponenter
    m_input = std::make_unique<Input>();
    m_player = std::make_unique<Player>(160.0f, 300.0f);
    m_room = std::make_unique<Room>("Tavern");
}

void PlayState::exit() {
    std::cout << "PlayState::exit()" << std::endl;
    
    // Frigör resurser
    m_player.reset();
    m_room.reset();
    m_input.reset();
}

void PlayState::update(float deltaTime) {
    // Samla input till rörelseverktor (-1, 0, eller 1)
    int dx = 0, dy = 0;
    if (m_input->isKeyDown(SDL_SCANCODE_LEFT) || m_input->isKeyDown(SDL_SCANCODE_A)) dx = -1;
    if (m_input->isKeyDown(SDL_SCANCODE_RIGHT) || m_input->isKeyDown(SDL_SCANCODE_D)) dx = 1;
    if (m_input->isKeyDown(SDL_SCANCODE_UP) || m_input->isKeyDown(SDL_SCANCODE_W)) dy = -1;
    if (m_input->isKeyDown(SDL_SCANCODE_DOWN) || m_input->isKeyDown(SDL_SCANCODE_S)) dy = 1;
    
    // Uppdatera spelare
    m_player->move(dx, dy, deltaTime);
    m_player->update(deltaTime);
    
    // Spara input-state för nästa frame
    m_input->update();
}

void PlayState::render(SDL_Renderer* renderer) {
    // Bakgrund
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
    SDL_RenderClear(renderer);
    
    // Rita rum
    m_room->render(renderer);
    
    // Rita spelare
    m_player->render(renderer);
}

void PlayState::handleEvent(const SDL_Event& event) {
    m_input->handleEvent(event);
    
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            // Tillbaka till meny
            if (m_game) {
                m_game->changeState(std::make_unique<MenuState>());
            }
        }
    }
}
