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
#include "../systems/InventorySystem.h"
#include "../systems/RoomManager.h"
#include "../graphics/Transition.h"
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
    
    // Skapa rum och registrera i RoomManager
    setupRooms();
    
    // Sätt callback för rumsbyte
    RoomManager::instance().setOnRoomChange([this](const std::string& roomId) {
        onRoomChange(roomId);
    });
    
    // Starta i tavern
    RoomManager::instance().changeRoom("tavern");
    
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
    
    // Registrera items
    Item key;
    key.id = "rusty_key";
    key.name = "Rusty Key";
    key.description = "An old rusty key. Wonder what it opens?";
    InventorySystem::instance().registerItem(key);
    
    Item coin;
    coin.id = "gold_coin";
    coin.name = "Gold Coin";
    coin.description = "A shiny gold coin.";
    InventorySystem::instance().registerItem(coin);
    
    Item letter;
    letter.id = "old_letter";
    letter.name = "Old Letter";
    letter.description = "A faded letter with barely readable text.";
    InventorySystem::instance().registerItem(letter);
}

void PlayState::exit() {
    std::cout << "PlayState::exit()" << std::endl;
    
    // Frigör resurser
    m_player.reset();
    m_input.reset();
}

void PlayState::setupRooms() {
    // === TAVERN ===
    auto tavern = std::make_unique<Room>("tavern", "Tavern");
    tavern->setWalkArea(0, 640, 260, 350);
    tavern->addHotspot("bartender", "Bartender", 80, 270, 40, 60, HotspotType::NPC);
    tavern->addHotspot("chest", "Old Chest", 400, 300, 40, 40, HotspotType::Item);
    tavern->addExit("door_street", "Exit to Street", 580, 260, 50, 90, "street");
    RoomManager::instance().addRoom(std::move(tavern));
    
    // === STREET ===
    auto street = std::make_unique<Room>("street", "Town Street");
    street->setWalkArea(0, 640, 280, 360);
    street->addExit("door_tavern", "Tavern Door", 20, 270, 50, 90, "tavern");
    street->addExit("door_shop", "Shop Door", 300, 260, 50, 90, "shop");
    street->addHotspot("well", "Old Well", 500, 300, 50, 50, HotspotType::Examine);
    RoomManager::instance().addRoom(std::move(street));
    
    // === SHOP ===
    auto shop = std::make_unique<Room>("shop", "General Store");
    shop->setWalkArea(0, 640, 260, 350);
    shop->addHotspot("shopkeeper", "Shopkeeper", 400, 270, 40, 60, HotspotType::NPC);
    shop->addExit("door_street", "Exit to Street", 20, 260, 50, 90, "street");
    RoomManager::instance().addRoom(std::move(shop));
}

void PlayState::onRoomChange(const std::string& roomId) {
    std::cout << "Player entered: " << roomId << std::endl;
    
    // Sätt spelarens position baserat på vilket rum
    float spawnX, spawnY;
    RoomManager::instance().getSpawnPosition(spawnX, spawnY);
    m_player->setPosition(spawnX, spawnY);
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
                    if (m_game) {
                        m_game->pushState(std::make_unique<DialogState>());
                    }
                }
            } else if (hs->type == HotspotType::Item) {
                if (hs->id == "chest") {
                    if (!InventorySystem::instance().hasItem("rusty_key")) {
                        InventorySystem::instance().addItem("rusty_key");
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
        }
    }
}
