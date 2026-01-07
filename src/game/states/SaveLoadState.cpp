/**
 * @file SaveLoadState.cpp
 * @brief Implementation av Save/Load meny
 */
#include "SaveLoadState.h"
#include "game/Game.h"
#include "engine/audio/AudioManager.h"
#include "engine/graphics/FontManager.h"
#include "engine/systems/SaveSystem.h"
#include <iostream>
#include <sstream>

SaveLoadState::SaveLoadState(Mode mode) 
    : m_mode(mode) {
    std::cout << "SaveLoadState created (" 
              << (mode == Mode::Save ? "Save" : "Load") << ")" << std::endl;
}

void SaveLoadState::enter() {
    std::cout << "SaveLoadState::enter()" << std::endl;
    m_selectedSlot = 0;
    refreshSlots();
}

void SaveLoadState::exit() {
    std::cout << "SaveLoadState::exit()" << std::endl;
}

void SaveLoadState::refreshSlots() {
    m_slots.clear();
    m_slots.resize(NUM_SLOTS);
    
    for (int i = 0; i < NUM_SLOTS; i++) {
        std::string slotName = "slot_" + std::to_string(i + 1);
        SaveData data;
        
        if (SaveSystem::instance().getSaveInfo(slotName, data)) {
            m_slots[i].name = data.saveName;
            m_slots[i].timestamp = data.timestamp;
            m_slots[i].playTimeSeconds = data.playTimeSeconds;
            m_slots[i].isEmpty = false;
        } else {
            m_slots[i].name = "Empty Slot";
            m_slots[i].isEmpty = true;
        }
    }
}

void SaveLoadState::update(float deltaTime) {
    (void)deltaTime;
}

void SaveLoadState::render(SDL_Renderer* renderer) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, 640, 400};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Huvudpanel
    SDL_Rect panel = {70, 50, 500, 300};
    SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
    SDL_RenderFillRect(renderer, &panel);
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderDrawRect(renderer, &panel);
    
    // Titel
    std::string title = (m_mode == Mode::Save) ? "SAVE GAME" : "LOAD GAME";
    FontManager::instance().renderTextCentered(renderer, "title",
        title, 320, 65, {200, 180, 100, 255});
    
    // Slots
    for (int i = 0; i < NUM_SLOTS; i++) {
        renderSlot(renderer, i, 100 + i * 45, i == m_selectedSlot);
    }
    
    // Instruktioner
    FontManager::instance().renderTextCentered(renderer, "default",
        "UP/DOWN: Select   ENTER: Confirm   ESC: Back", 320, 330, {120, 120, 140, 255});
}

void SaveLoadState::renderSlot(SDL_Renderer* renderer, int index, int y, bool selected) {
    SDL_Rect slotRect = {100, y, 440, 38};
    
    // Bakgrund
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 60, 55, 80, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 40, 38, 55, 255);
    }
    SDL_RenderFillRect(renderer, &slotRect);
    
    // Ram
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 200, 180, 100, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 70, 70, 90, 255);
    }
    SDL_RenderDrawRect(renderer, &slotRect);
    
    const SlotInfo& slot = m_slots[index];
    
    // Slot nummer
    std::string slotNum = "Slot " + std::to_string(index + 1);
    SDL_Color numColor = selected ? SDL_Color{255, 220, 100, 255} : SDL_Color{150, 150, 170, 255};
    FontManager::instance().renderText(renderer, "default", slotNum, 110, y + 10, numColor);
    
    if (slot.isEmpty) {
        // Tom slot
        FontManager::instance().renderText(renderer, "default", 
            "- Empty -", 250, y + 10, {100, 100, 120, 255});
    } else {
        // Timestamp
        FontManager::instance().renderText(renderer, "default",
            slot.timestamp, 250, y + 10, {180, 180, 200, 255});
        
        // Speltid
        int hours = slot.playTimeSeconds / 3600;
        int mins = (slot.playTimeSeconds % 3600) / 60;
        std::stringstream ss;
        ss << hours << "h " << mins << "m";
        FontManager::instance().renderText(renderer, "default",
            ss.str(), 450, y + 10, {140, 180, 140, 255});
    }
}

void SaveLoadState::performAction() {
    std::string slotName = "slot_" + std::to_string(m_selectedSlot + 1);
    
    if (m_mode == Mode::Save) {
        if (SaveSystem::instance().save(slotName)) {
            std::cout << "Game saved to " << slotName << std::endl;
            AudioManager::instance().playSound("save");
            refreshSlots();
        }
    } else {
        // Load
        if (m_slots[m_selectedSlot].isEmpty) {
            std::cout << "Cannot load empty slot" << std::endl;
            AudioManager::instance().playSound("error");
            return;
        }
        
        if (SaveSystem::instance().load(slotName)) {
            std::cout << "Game loaded from " << slotName << std::endl;
            AudioManager::instance().playSound("load");
            // Pop tillbaka till gameplay
            if (m_game) {
                m_game->popState();  // Pop SaveLoadState
                m_game->popState();  // Pop PauseState (om den finns)
            }
        }
    }
}

void SaveLoadState::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_W:
                m_selectedSlot = (m_selectedSlot - 1 + NUM_SLOTS) % NUM_SLOTS;
                AudioManager::instance().playSound("navigate");
                break;
                
            case SDL_SCANCODE_DOWN:
            case SDL_SCANCODE_S:
                m_selectedSlot = (m_selectedSlot + 1) % NUM_SLOTS;
                AudioManager::instance().playSound("navigate");
                break;
                
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_SPACE:
                AudioManager::instance().playSound("select");
                performAction();
                break;
                
            case SDL_SCANCODE_ESCAPE:
                if (m_game) {
                    m_game->popState();
                }
                break;
                
            default:
                break;
        }
    }
}
