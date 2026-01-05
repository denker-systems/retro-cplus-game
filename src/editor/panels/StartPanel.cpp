/**
 * @file StartPanel.cpp
 * @brief Implementation av start panel
 */
#include "StartPanel.h"
#include "editor/EditorCore.h"
#include "engine/graphics/FontManager.h"
#include "engine/data/DataLoader.h"

StartPanel::StartPanel() = default;

void StartPanel::update(float deltaTime) {
    // Nothing to update
}

void StartPanel::render(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    SDL_Color green = {100, 255, 100, 255};
    
    // Title
    FontManager::instance().renderText(renderer, "default", 
        "RETRO ADVENTURE EDITOR", 250, 50, cyan);
    
    // Menu options
    int y = 120;
    const char* options[] = {
        "1. Edit Rooms",
        "2. Edit Dialogs",
        "3. Edit Quests",
        "4. Edit Items",
        "5. World State",
        "6. Audio Settings"
    };
    
    for (int i = 0; i < 6; i++) {
        SDL_Color color = (i == m_selectedOption) ? yellow : white;
        std::string text = (i == m_selectedOption) ? "> " : "  ";
        text += options[i];
        FontManager::instance().renderText(renderer, "default", text, 200, y, color);
        y += 20;
    }
    
    // Stats
    y = 280;
    auto& rooms = DataLoader::instance().getRooms();
    auto& dialogs = DataLoader::instance().getDialogs();
    auto& quests = DataLoader::instance().getQuests();
    auto& items = DataLoader::instance().getItems();
    
    FontManager::instance().renderText(renderer, "default", 
        "Loaded Data:", 200, y, cyan);
    y += 20;
    
    std::string stats = "  Rooms: " + std::to_string(rooms.size()) + 
                       "  Dialogs: " + std::to_string(dialogs.size()) +
                       "  Quests: " + std::to_string(quests.size()) +
                       "  Items: " + std::to_string(items.size());
    FontManager::instance().renderText(renderer, "default", stats, 200, y, green);
    
    // Instructions
    FontManager::instance().renderText(renderer, "default", 
        "Use UP/DOWN to navigate, ENTER to select", 200, 340, white);
}

bool StartPanel::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_UP:
                m_selectedOption = (m_selectedOption - 1 + 6) % 6;
                return true;
            case SDL_SCANCODE_DOWN:
                m_selectedOption = (m_selectedOption + 1) % 6;
                return true;
            case SDL_SCANCODE_RETURN:
            case SDL_SCANCODE_KP_ENTER:
                // Switch to selected panel
                switch (m_selectedOption) {
                    case 0: EditorCore::instance().switchToPanel("room"); break;
                    case 1: EditorCore::instance().setStatus("Dialog editor not yet implemented"); break;
                    case 2: EditorCore::instance().setStatus("Quest editor not yet implemented"); break;
                    case 3: EditorCore::instance().setStatus("Item editor not yet implemented"); break;
                    case 4: EditorCore::instance().setStatus("World state editor not yet implemented"); break;
                    case 5: EditorCore::instance().setStatus("Audio settings not yet implemented"); break;
                }
                return true;
            default:
                break;
        }
    }
    return false;
}
