/**
 * @file InventoryState.cpp
 * @brief Implementation av Inventory UI
 */
#include "InventoryState.h"
#include "../Game.h"
#include "../systems/InventorySystem.h"
#include "../graphics/FontManager.h"
#include "../audio/AudioManager.h"
#include <iostream>

InventoryState::InventoryState() {
    std::cout << "InventoryState created" << std::endl;
}

void InventoryState::enter() {
    std::cout << "InventoryState::enter()" << std::endl;
    m_selectedIndex = 0;
}

void InventoryState::exit() {
    std::cout << "InventoryState::exit()" << std::endl;
}

void InventoryState::update(float deltaTime) {
    (void)deltaTime;
}

void InventoryState::render(SDL_Renderer* renderer) {
    // Semi-transparent bakgrund
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, 640, 400};
    SDL_RenderFillRect(renderer, &overlay);
    
    // Inventory panel
    SDL_Rect panel = {120, 60, 400, 280};
    SDL_SetRenderDrawColor(renderer, 40, 35, 50, 250);
    SDL_RenderFillRect(renderer, &panel);
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 120, 100, 80, 255);
    SDL_RenderDrawRect(renderer, &panel);
    
    // Titel
    FontManager::instance().renderTextCentered(renderer, "title",
        "INVENTORY", 320, 75, {200, 180, 120, 255});
    
    // Item slots (2 rader x 4 kolumner)
    const auto& items = InventorySystem::instance().getItems();
    int slotSize = 70;
    int startX = 145;
    int startY = 120;
    int cols = 4;
    
    for (int i = 0; i < InventorySystem::MAX_ITEMS; i++) {
        int col = i % cols;
        int row = i / cols;
        int x = startX + col * (slotSize + 10);
        int y = startY + row * (slotSize + 10);
        
        bool hasItem = (i < static_cast<int>(items.size()));
        renderItemSlot(renderer, i, x, y, i == m_selectedIndex);
        
        // Rita item-namn under slot om vald
        if (hasItem && i == m_selectedIndex) {
            const Item& item = items[i];
            FontManager::instance().renderTextCentered(renderer, "default",
                item.name, 320, 290, {255, 255, 200, 255});
            FontManager::instance().renderTextCentered(renderer, "default",
                item.description, 320, 310, {180, 180, 200, 255});
        }
    }
    
    // Instruktioner
    FontManager::instance().renderTextCentered(renderer, "default",
        "Arrow keys to select, ESC/I to close", 320, 345, {100, 100, 120, 255});
}

void InventoryState::renderItemSlot(SDL_Renderer* renderer, int index, int x, int y, bool selected) {
    int slotSize = 70;
    
    // Slot bakgrund
    SDL_Rect slot = {x, y, slotSize, slotSize};
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 80, 70, 100, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 50, 45, 60, 255);
    }
    SDL_RenderFillRect(renderer, &slot);
    
    // Ram
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 200, 180, 100, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 80, 70, 90, 255);
    }
    SDL_RenderDrawRect(renderer, &slot);
    
    // Rita item om det finns
    const auto items = InventorySystem::instance().getItems();
    if (index < static_cast<int>(items.size())) {
        // Placeholder - färgad rektangel per item-typ
        SDL_Rect itemRect = {x + 10, y + 10, 50, 50};
        
        const std::string& itemId = items[index].id;
        if (itemId == "rusty_key") {
            SDL_SetRenderDrawColor(renderer, 180, 140, 60, 255);
        } else if (itemId == "gold_coin") {
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
        } else if (itemId == "old_letter") {
            SDL_SetRenderDrawColor(renderer, 220, 200, 180, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
        }
        SDL_RenderFillRect(renderer, &itemRect);
    }
}

void InventoryState::handleEvent(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN) return;
    
    const auto& items = InventorySystem::instance().getItems();
    int itemCount = static_cast<int>(items.size());
    int cols = 4;
    
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_A:
            if (m_selectedIndex > 0) {
                m_selectedIndex--;
                AudioManager::instance().playSound("navigate");
            }
            break;
            
        case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_D:
            if (m_selectedIndex < InventorySystem::MAX_ITEMS - 1) {
                m_selectedIndex++;
                AudioManager::instance().playSound("navigate");
            }
            break;
            
        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_W:
            if (m_selectedIndex >= cols) {
                m_selectedIndex -= cols;
                AudioManager::instance().playSound("navigate");
            }
            break;
            
        case SDL_SCANCODE_DOWN:
        case SDL_SCANCODE_S:
            if (m_selectedIndex < InventorySystem::MAX_ITEMS - cols) {
                m_selectedIndex += cols;
                AudioManager::instance().playSound("navigate");
            }
            break;
            
        case SDL_SCANCODE_ESCAPE:
        case SDL_SCANCODE_I:
            AudioManager::instance().playSound("select");
            if (m_game) {
                m_game->popState();
            }
            break;
            
        default:
            break;
    }
}
