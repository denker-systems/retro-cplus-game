/**
 * @file PlayState.cpp
 * @brief Implementation av gameplay state
 */
#include "PlayState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "DialogState.h"
#include "../Game.h"
#include "../entities/PlayerCharacter.h"
#include "../Room.h"
#include "../Input.h"
#include "../graphics/FontManager.h"
#include "../systems/DialogSystem.h"
#include <iostream>

PlayState::PlayState() {
    std::cout << "PlayState created" << std::endl;
}

PlayState::~PlayState() = default;

void PlayState::enter() {
    std::cout << "PlayState::enter()" << std::endl;
    
    // Skapa spelkomponenter
    m_input = std::make_unique<Input>();
    m_player = std::make_unique<PlayerCharacter>(160.0f, 300.0f);
    
    // Skapa rum med hotspots
    m_room = std::make_unique<Room>("tavern", "Tavern");
    m_room->setWalkArea(0, 640, 260, 350);
    m_room->addHotspot("bartender", "Bartender", 80, 270, 40, 60, HotspotType::NPC);
    m_room->addHotspot("chest", "Old Chest", 400, 300, 40, 40, HotspotType::Item);
    m_room->addHotspot("door", "Exit Door", 580, 260, 50, 90, HotspotType::Exit);
    
    // Registrera Bartender-dialog
    DialogTree bartenderDialog;
    bartenderDialog.id = "bartender_intro";
    bartenderDialog.npcName = "Bartender";
    bartenderDialog.startNodeId = 0;
    
    DialogNode node0;
    node0.id = 0;
    node0.speaker = "Bartender";
    node0.text = "Welcome to the Rusty Anchor! What can I get ya?";
    node0.choices = {
        {"I'm looking for information.", 1},
        {"Just passing through.", 2},
        {"Nothing, thanks.", -1}
    };
    bartenderDialog.nodes.push_back(node0);
    
    DialogNode node1;
    node1.id = 1;
    node1.speaker = "Bartender";
    node1.text = "Information, eh? Well, I hear things... What do you want to know?";
    node1.nextNodeId = -1;
    bartenderDialog.nodes.push_back(node1);
    
    DialogNode node2;
    node2.id = 2;
    node2.speaker = "Bartender";
    node2.text = "Safe travels then, stranger.";
    node2.nextNodeId = -1;
    bartenderDialog.nodes.push_back(node2);
    
    DialogSystem::instance().addDialog(bartenderDialog);
}

void PlayState::exit() {
    std::cout << "PlayState::exit()" << std::endl;
    
    // Frigör resurser
    m_player.reset();
    m_room.reset();
    m_input.reset();
}

void PlayState::update(float deltaTime) {
    // Tangentbord-rörelse
    int dx = 0, dy = 0;
    if (m_input->isKeyDown(SDL_SCANCODE_LEFT) || m_input->isKeyDown(SDL_SCANCODE_A)) dx = -1;
    if (m_input->isKeyDown(SDL_SCANCODE_RIGHT) || m_input->isKeyDown(SDL_SCANCODE_D)) dx = 1;
    if (m_input->isKeyDown(SDL_SCANCODE_UP) || m_input->isKeyDown(SDL_SCANCODE_W)) dy = -1;
    if (m_input->isKeyDown(SDL_SCANCODE_DOWN) || m_input->isKeyDown(SDL_SCANCODE_S)) dy = 1;
    
    m_player->move(dx, dy, deltaTime);
    
    // Kolla hotspot under musen
    int mx = m_input->getMouseX();
    int my = m_input->getMouseY();
    Hotspot* hs = m_room->getHotspotAt(mx, my);
    m_hoveredHotspot = hs ? hs->name : "";
    
    // Point-and-click (vänsterklick)
    if (m_input->isMouseClicked(SDL_BUTTON_LEFT)) {
        if (hs) {
            // Klickade på hotspot - interagera
            std::cout << "Clicked on: " << hs->name << " (" << hs->id << ")" << std::endl;
            
            if (hs->type == HotspotType::NPC) {
                // Starta dialog med NPC
                if (hs->id == "bartender") {
                    DialogSystem::instance().startDialog("bartender_intro");
                    if (m_game) {
                        m_game->pushState(std::make_unique<DialogState>());
                    }
                }
            } else if (hs->type == HotspotType::Item) {
                std::cout << "You examine the " << hs->name << std::endl;
            } else if (hs->type == HotspotType::Exit) {
                std::cout << "You look at the exit..." << std::endl;
            }
        } else if (my > 260 && my < 375) {
            // Klickade i walk area - gå dit
            m_player->setTarget(static_cast<float>(mx), static_cast<float>(my));
        }
    }
    
    m_player->update(deltaTime);
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
    
    // Visa hotspot-namn i UI-bar
    if (!m_hoveredHotspot.empty()) {
        FontManager::instance().renderText(renderer, "default", 
            m_hoveredHotspot, 10, 378, {255, 255, 200, 255});
    } else {
        FontManager::instance().renderText(renderer, "default",
            m_room->getName(), 10, 378, {150, 150, 180, 255});
    }
}

void PlayState::handleEvent(const SDL_Event& event) {
    m_input->handleEvent(event);
    
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            // Öppna pausmeny (overlay)
            if (m_game) {
                m_game->pushState(std::make_unique<PauseState>());
            }
        }
    }
}
