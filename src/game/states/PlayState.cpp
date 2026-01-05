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
#include "game/Game.h"
#include "engine/entities/PlayerCharacter.h"
#include "engine/Room.h"
#include "engine/input/Input.h"
#include "engine/graphics/FontManager.h"
#include "engine/systems/DialogSystem.h"
#include "engine/systems/InventorySystem.h"
#include "engine/systems/RoomManager.h"
#include "engine/systems/QuestSystem.h"
#include "engine/systems/AISystem.h"
#include "engine/graphics/Transition.h"
#include "engine/data/GameDataLoader.h"
#include "engine/entities/NPC.h"
#include "engine/utils/Logger.h"
#include <iostream>
#include <cmath>

PlayState::PlayState() {
    std::cout << "PlayState created" << std::endl;
}

PlayState::~PlayState() = default;

void PlayState::enter() {
    LOG_DEBUG("PlayState::enter() - initialized=" + std::string(m_initialized ? "true" : "false"));
    
    // Första gången - ladda allt
    if (!m_initialized) {
        LOG_INFO("PlayState initializing for first time");
        
        // Skapa spelkomponenter
        m_input = std::make_unique<Input>();
        m_player = std::make_unique<PlayerCharacter>(160.0f, 300.0f);
        
        // Ladda all speldata från JSON-filer (skicka renderer för att ladda texturer)
        SDL_Renderer* renderer = m_game->getRenderer();
        GameDataLoader::loadAll(renderer);
        
        // Sätt callback för rumsbyte
        RoomManager::instance().setOnRoomChange([this](const std::string& roomId) {
            onRoomChange(roomId);
        });
        
        // Starta i tavern
        RoomManager::instance().changeRoom("tavern");
        
        // Setup hot reload för JSON-filer
        // Hot reload removed - restart game to see editor changes
        
        m_initialized = true;
    } else {
        LOG_DEBUG("PlayState resuming (already initialized)");
    }
}

void PlayState::exit() {
    LOG_DEBUG("PlayState::exit()");
}

void PlayState::onRoomChange(const std::string& roomId) {
    std::cout << "Player entered: " << roomId << std::endl;
    
    // Uppdatera spelarens walk area från rummet
    Room* room = RoomManager::instance().getCurrentRoom();
    if (room) {
        const auto& wa = room->getWalkArea();
        m_player->setWalkArea(wa.minX, wa.maxX, wa.minY, wa.maxY);
        
        // Använd rummets player spawn position
        float spawnX, spawnY;
        room->getPlayerSpawn(spawnX, spawnY);
        m_player->setPosition(spawnX, spawnY);
    }
    
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
    
    // Hitta närmaste hotspot för E-interaktion
    m_nearbyHotspot = getNearbyHotspot(INTERACT_DISTANCE);
    
    // Point-and-click (vänsterklick)
    if (m_input->isMouseClicked(SDL_BUTTON_LEFT)) {
        if (hs) {
            interactWithHotspot(hs);
        } else if (my > 260 && my < 375) {
            m_player->setTarget(static_cast<float>(mx), static_cast<float>(my));
        }
    }
    
    m_player->update(deltaTime);
    m_input->update();
    
    // Uppdatera AI-system (NPC beteenden och scheman)
    AISystem::instance().update(deltaTime);
    
    // Uppdatera NPCs i aktuellt rum
    if (room) {
        room->updateNPCs(deltaTime);
    }
    
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
        room->renderNPCs(renderer);
    }
    
    // Rita spelare med depth scale
    float playerScale = 1.0f;
    if (room) {
        const auto& wa = room->getWalkArea();
        float playerY = m_player->getY();
        
        // Beräkna scale baserat på Y-position inom walk area
        if (wa.maxY > wa.minY) {
            float t = (playerY - wa.minY) / (wa.maxY - wa.minY);
            t = std::max(0.0f, std::min(1.0f, t));  // Clamp 0-1
            playerScale = wa.scaleTop + t * (wa.scaleBottom - wa.scaleTop);
        }
    }
    m_player->renderScaled(renderer, playerScale);
    
    // Visa hotspot-namn i UI-bar
    if (!m_hoveredHotspot.empty()) {
        FontManager::instance().renderText(renderer, "default", 
            m_hoveredHotspot, 10, 378, {255, 255, 200, 255});
    } else if (m_nearbyHotspot) {
        // Visa nearby hotspot med [E] prompt
        std::string prompt = "[E] " + m_nearbyHotspot->name;
        FontManager::instance().renderText(renderer, "default",
            prompt, 10, 378, {100, 255, 100, 255});
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
        } else if (event.key.keysym.scancode == SDL_SCANCODE_E) {
            // Interagera med nearby hotspot
            if (m_nearbyHotspot) {
                interactWithHotspot(m_nearbyHotspot);
            }
        }
    }
}

Hotspot* PlayState::getNearbyHotspot(float maxDistance) {
    Room* room = RoomManager::instance().getCurrentRoom();
    if (!room || !m_player) return nullptr;
    
    float playerX = m_player->getX() + 16;  // Center of player
    float playerY = m_player->getY() + 32;
    
    Hotspot* nearest = nullptr;
    float nearestDist = maxDistance;
    
    for (auto& hotspot : room->getHotspots()) {
        // Beräkna centrum av hotspot
        float hsX = hotspot.rect.x + hotspot.rect.w / 2.0f;
        float hsY = hotspot.rect.y + hotspot.rect.h / 2.0f;
        
        // Beräkna avstånd
        float dx = playerX - hsX;
        float dy = playerY - hsY;
        float dist = std::sqrt(dx * dx + dy * dy);
        
        if (dist < nearestDist) {
            nearestDist = dist;
            nearest = const_cast<Hotspot*>(&hotspot);
        }
    }
    
    return nearest;
}

void PlayState::interactWithHotspot(Hotspot* hotspot) {
    if (!hotspot) {
        LOG_WARNING("interactWithHotspot called with null hotspot");
        return;
    }
    
    LOG_INFO("Interact: " + hotspot->name + " (" + hotspot->id + ") type=" + std::to_string(static_cast<int>(hotspot->type)));
    
    if (hotspot->type == HotspotType::NPC) {
        LOG_DEBUG("NPC interaction, dialogId=" + hotspot->dialogId);
        if (!hotspot->dialogId.empty()) {
            LOG_DEBUG("Starting dialog: " + hotspot->dialogId);
            DialogSystem::instance().startDialog(hotspot->dialogId);
            QuestSystem::instance().updateObjective(ObjectiveType::Talk, hotspot->id);
            if (m_game) {
                LOG_DEBUG("Pushing DialogState");
                m_game->pushState(std::make_unique<DialogState>());
            }
        }
    } else if (hotspot->type == HotspotType::Item) {
        LOG_DEBUG("Item interaction: " + hotspot->id);
        if (hotspot->id == "chest") {
            if (!InventorySystem::instance().hasItem("rusty_key")) {
                LOG_INFO("Adding rusty_key to inventory");
                InventorySystem::instance().addItem("rusty_key");
                QuestSystem::instance().updateObjective(ObjectiveType::Collect, "rusty_key");
            } else {
                LOG_DEBUG("Chest already looted");
            }
        }
    } else if (hotspot->type == HotspotType::Exit) {
        LOG_DEBUG("Exit interaction, target=" + hotspot->targetRoom);
        if (!hotspot->targetRoom.empty() && !Transition::instance().isActive()) {
            std::string target = hotspot->targetRoom;
            float spawnX = (hotspot->rect.x > 300) ? 80.0f : 550.0f;
            
            LOG_INFO("Room transition to: " + target);
            Transition::instance().fadeToBlack(0.5f, [target, spawnX]() {
                LOG_DEBUG("Fade complete, changing room to: " + target);
                RoomManager::instance().setSpawnPosition(spawnX, 300.0f);
                RoomManager::instance().changeRoom(target);
            });
        }
    } else if (hotspot->type == HotspotType::Examine) {
        LOG_DEBUG("Examine: " + hotspot->name);
        QuestSystem::instance().updateObjective(ObjectiveType::Examine, hotspot->id);
    }
}

// Hot reload removed - restart game to see editor changes
// This simplifies the architecture and avoids threading/timing issues
