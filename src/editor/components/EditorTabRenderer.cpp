/**
 * @file EditorTabRenderer.cpp
 * @brief Implementation av EditorTabRenderer
 */
#include "EditorTabRenderer.h"
#include "engine/graphics/FontManager.h"
#include "engine/data/DataLoader.h"
#include "engine/systems/WorldState.h"
#include "engine/audio/AudioManager.h"
#include <algorithm>

void EditorTabRenderer::renderOverviewTab(SDL_Renderer* renderer, const std::vector<RoomNode>& roomNodes, const std::string& selectedRoom) {
    SDL_Color cyan = {100, 255, 255, 255};
    FontManager::instance().renderText(renderer, "default", "[Game Overview - Room Flowchart]", 10, 65, cyan);
    renderRoomFlowchart(renderer, roomNodes, selectedRoom);
}

void EditorTabRenderer::renderRoomFlowchart(SDL_Renderer* renderer, const std::vector<RoomNode>& roomNodes, const std::string& selectedRoom) {
    // Rita rum-noder
    for (const auto& node : roomNodes) {
        bool selected = (node.id == selectedRoom);
        
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
        }
        
        SDL_Rect rect = {node.x, node.y, node.width, node.height};
        SDL_RenderFillRect(renderer, &rect);
        
        SDL_SetRenderDrawColor(renderer, 150, 150, 200, 255);
        SDL_RenderDrawRect(renderer, &rect);
        
        SDL_Color textColor = selected ? SDL_Color{255, 255, 100, 255} : SDL_Color{255, 255, 255, 255};
        FontManager::instance().renderText(renderer, "default", node.id, node.x + 5, node.y + 12, textColor);
    }
    
    // Rita kopplingar
    SDL_SetRenderDrawColor(renderer, 100, 150, 100, 255);
    auto& rooms = DataLoader::instance().getRooms();
    
    for (const auto& room : rooms) {
        auto fromIt = std::find_if(roomNodes.begin(), roomNodes.end(),
            [&](const RoomNode& n) { return n.id == room.id; });
        
        if (fromIt == roomNodes.end()) continue;
        
        for (const auto& hs : room.hotspots) {
            if (!hs.targetScene.empty()) {
                auto toIt = std::find_if(roomNodes.begin(), roomNodes.end(),
                    [&](const RoomNode& n) { return n.id == hs.targetScene; });
                
                if (toIt != roomNodes.end()) {
                    int x1 = fromIt->x + fromIt->width / 2;
                    int y1 = fromIt->y + fromIt->height;
                    int x2 = toIt->x + toIt->width / 2;
                    int y2 = toIt->y;
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }
            }
        }
    }
}

void EditorTabRenderer::renderRoomsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedRoom) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Rooms]", 10, 65, cyan);
    
    auto& rooms = DataLoader::instance().getRooms();
    int y = 90 - scrollY;
    
    for (const auto& room : rooms) {
        bool selected = (room.id == selectedRoom);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + room.id + " (" + room.name + ")";
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (selected) {
            FontManager::instance().renderText(renderer, "default", 
                "  Hotspots: " + std::to_string(room.hotspots.size()), 30, y, green);
            y += 12;
            
            for (const auto& hs : room.hotspots) {
                std::string hsText = "    - " + hs.name + " [" + hs.id + "]";
                if (!hs.targetScene.empty()) {
                    hsText += " -> " + hs.targetScene;
                }
                FontManager::instance().renderText(renderer, "default", hsText, 30, y, white);
                y += 12;
            }
        }
        
        if (y > 360) break;
    }
}

void EditorTabRenderer::renderDialogsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedDialog) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color magenta = {255, 100, 255, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Dialogs]", 10, 65, magenta);
    
    auto& dialogs = DataLoader::instance().getDialogs();
    int y = 90 - scrollY;
    
    for (const auto& dialog : dialogs) {
        bool selected = (dialog.id == selectedDialog);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + dialog.id;
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (y > 360) break;
    }
}

void EditorTabRenderer::renderQuestsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedQuest) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color orange = {255, 180, 100, 255};
    SDL_Color green = {100, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Quests]", 10, 65, orange);
    
    auto& quests = DataLoader::instance().getQuests();
    int y = 90 - scrollY;
    
    for (const auto& quest : quests) {
        bool selected = (quest.id == selectedQuest);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + quest.id + " - " + quest.title;
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (selected) {
            FontManager::instance().renderText(renderer, "default", 
                "  Objectives: " + std::to_string(quest.objectives.size()), 30, y, green);
            y += 12;
        }
        
        if (y > 360) break;
    }
}

void EditorTabRenderer::renderItemsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedItem) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color blue = {100, 150, 255, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Items]", 10, 65, blue);
    
    auto& items = DataLoader::instance().getItems();
    int y = 90 - scrollY;
    
    for (const auto& item : items) {
        bool selected = (item.id == selectedItem);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + item.id + " - " + item.name;
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (y > 360) break;
    }
}

void EditorTabRenderer::renderWorldStateTab(SDL_Renderer* renderer, int scrollY) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color red = {255, 100, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[World State - Flags & Counters]", 10, 65, green);
    
    int y = 90 - scrollY;
    
    // Visa flags
    FontManager::instance().renderText(renderer, "default", "Flags:", 20, y, white);
    y += 15;
    
    auto& flags = WorldState::instance().getAllFlags();
    for (const auto& [key, value] : flags) {
        SDL_Color color = value ? green : red;
        std::string text = "  " + key + ": " + (value ? "true" : "false");
        FontManager::instance().renderText(renderer, "default", text, 30, y, color);
        y += 12;
        if (y > 360) break;
    }
    
    if (y <= 360) {
        y += 10;
        FontManager::instance().renderText(renderer, "default", "Counters:", 20, y, white);
        y += 15;
        
        auto& counters = WorldState::instance().getAllCounters();
        for (const auto& [key, value] : counters) {
            std::string text = "  " + key + ": " + std::to_string(value);
            FontManager::instance().renderText(renderer, "default", text, 30, y, white);
            y += 12;
            if (y > 360) break;
        }
    }
}

void EditorTabRenderer::renderAudioTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color purple = {200, 100, 255, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Audio - Music & Sounds]", 10, 65, purple);
    
    int y = 90;
    
    // Visa audio state
    bool muted = AudioManager::instance().isMuted();
    std::string muteText = "Audio: " + std::string(muted ? "MUTED" : "Playing");
    SDL_Color muteColor = muted ? SDL_Color{255, 100, 100, 255} : SDL_Color{100, 255, 100, 255};
    FontManager::instance().renderText(renderer, "default", muteText, 20, y, muteColor);
    y += 20;
    
    FontManager::instance().renderText(renderer, "default", "Press M to toggle mute", 20, y, white);
}
