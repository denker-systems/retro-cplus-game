/**
 * @file PlayState.cpp
 * @brief Implementation av gameplay state
 */
#include "PlayState.h"
#include "MenuState.h"
#include "PauseState.h"
#include "DialogState.h"
#include "InventoryState.h"
#include "QuestLogState.h"
#include "../Game.h"
#include "../entities/PlayerCharacter.h"
#include "../Room.h"
#include "../Input.h"
#include "../graphics/FontManager.h"
#include "../systems/DialogSystem.h"
#include "../systems/InventorySystem.h"
#include "../systems/RoomManager.h"
#include "../systems/QuestSystem.h"
#include "../graphics/Transition.h"
#include "../data/GameDataLoader.h"
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
    
    // Ladda all speldata från JSON-filer
    GameDataLoader::loadAll();
    
    // Sätt callback för rumsbyte
    RoomManager::instance().setOnRoomChange([this](const std::string& roomId) {
        onRoomChange(roomId);
    });
    
    // Starta i tavern
    RoomManager::instance().changeRoom("tavern");
}

void PlayState::exit() {
    std::cout << "PlayState::exit()" << std::endl;
    
    // Frigör resurser
    m_player.reset();
    m_input.reset();
}

void PlayState::onRoomChange(const std::string& roomId) {
    std::cout << "Player entered: " << roomId << std::endl;
    
    // Sätt spelarens position baserat på vilket rum
    float spawnX, spawnY;
    RoomManager::instance().getSpawnPosition(spawnX, spawnY);
    m_player->setPosition(spawnX, spawnY);
    
    // Uppdatera quest objective för GoTo
    QuestSystem::instance().updateObjective(ObjectiveType::GoTo, roomId);
}

void PlayState::update(float deltaTime) {
    Room* room = RoomManager::instance().getCurrentRoom();
    if (!room) return;
    
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
    Hotspot* hs = room->getHotspotAt(mx, my);
    m_hoveredHotspot = hs ? hs->name : "";
    
    // Point-and-click (vänsterklick)
    if (m_input->isMouseClicked(SDL_BUTTON_LEFT)) {
        if (hs) {
            std::cout << "Clicked on: " << hs->name << " (" << hs->id << ")" << std::endl;
            
            if (hs->type == HotspotType::NPC) {
                if (hs->id == "bartender") {
                    DialogSystem::instance().startDialog("bartender_intro");
                    QuestSystem::instance().updateObjective(ObjectiveType::Talk, "bartender");
                    if (m_game) {
                        m_game->pushState(std::make_unique<DialogState>());
                    }
                }
            } else if (hs->type == HotspotType::Item) {
                if (hs->id == "chest") {
                    if (!InventorySystem::instance().hasItem("rusty_key")) {
                        InventorySystem::instance().addItem("rusty_key");
                        QuestSystem::instance().updateObjective(ObjectiveType::Collect, "rusty_key");
                    } else {
                        std::cout << "The chest is empty." << std::endl;
                    }
                }
            } else if (hs->type == HotspotType::Exit) {
                // Byt rum med fade transition!
                if (!hs->targetRoom.empty() && !Transition::instance().isActive()) {
                    std::string target = hs->targetRoom;
                    float spawnX = (hs->rect.x > 300) ? 80.0f : 550.0f;
                    
                    Transition::instance().fadeToBlack(0.5f, [target, spawnX]() {
                        RoomManager::instance().setSpawnPosition(spawnX, 300.0f);
                        RoomManager::instance().changeRoom(target);
                    });
                }
            } else if (hs->type == HotspotType::Examine) {
                std::cout << "You examine the " << hs->name << "..." << std::endl;
            }
        } else if (my > 260 && my < 375) {
            m_player->setTarget(static_cast<float>(mx), static_cast<float>(my));
        }
    }
    
    m_player->update(deltaTime);
    m_input->update();
    
    // Uppdatera transition
    Transition::instance().update(deltaTime);
}

void PlayState::render(SDL_Renderer* renderer) {
    Room* room = RoomManager::instance().getCurrentRoom();
    
    // Bakgrund
    SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
    SDL_RenderClear(renderer);
    
    // Rita rum
    if (room) {
        room->render(renderer);
    }
    
    // Rita spelare
    m_player->render(renderer);
    
    // Visa hotspot-namn i UI-bar
    if (!m_hoveredHotspot.empty()) {
        FontManager::instance().renderText(renderer, "default", 
            m_hoveredHotspot, 10, 378, {255, 255, 200, 255});
    } else if (room) {
        FontManager::instance().renderText(renderer, "default",
            room->getName(), 10, 378, {150, 150, 180, 255});
    }
    
    // Visa inventory count
    std::string invText = "Items: " + std::to_string(InventorySystem::instance().getItemCount());
    FontManager::instance().renderText(renderer, "default", invText, 550, 378, {180, 180, 200, 255});
    
    // Rita transition overlay sist
    Transition::instance().render(renderer);
}

void PlayState::handleEvent(const SDL_Event& event) {
    m_input->handleEvent(event);
    
    if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
            // Öppna pausmeny (overlay)
            if (m_game) {
                m_game->pushState(std::make_unique<PauseState>());
            }
        } else if (event.key.keysym.scancode == SDL_SCANCODE_I) {
            // Öppna inventory (overlay)
            if (m_game) {
                m_game->pushState(std::make_unique<InventoryState>());
            }
        } else if (event.key.keysym.scancode == SDL_SCANCODE_J) {
            // Öppna quest log (overlay)
            if (m_game) {
                m_game->pushState(std::make_unique<QuestLogState>());
            }
        }
    }
}
