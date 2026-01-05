/**
 * @file EditorState.cpp
 * @brief Implementation av separat editor state
 */
#include "EditorState.h"
#include "../Game.h"
#include "../systems/WorldState.h"
#include "../systems/RoomManager.h"
#include "../systems/DialogSystem.h"
#include "../systems/QuestSystem.h"
#include "../systems/InventorySystem.h"
#include "../audio/AudioManager.h"
#include "../graphics/FontManager.h"
#include "../data/DataLoader.h"
#include "../data/TiledImporter.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <filesystem>

EditorState::EditorState() {
    LOG_INFO("EditorState created");
}

void EditorState::enter() {
    LOG_INFO("EditorState::enter()");
    m_currentTab = EditorTab::Overview;
    m_scrollY = 0;
    
    // Ladda all game data (om inte redan laddat)
    auto& loader = DataLoader::instance();
    if (loader.getRooms().empty()) {
        LOG_INFO("EditorState: Loading game data...");
        loader.loadAll();
    }
    
    // Bygg rum-flowchart positioner
    m_roomNodes.clear();
    auto& rooms = loader.getRooms();
    
    int x = 50;
    int y = 100;
    int col = 0;
    
    for (const auto& room : rooms) {
        RoomNode node;
        node.id = room.id;
        node.x = x + col * 150;
        node.y = y;
        node.width = 120;
        node.height = 40;
        m_roomNodes.push_back(node);
        
        col++;
        if (col >= 4) {
            col = 0;
            y += 80;
        }
    }
}

void EditorState::exit() {
    LOG_INFO("EditorState::exit()");
}

void EditorState::update(float deltaTime) {
    // Uppdatera status message timer
    if (m_statusTimer > 0) {
        m_statusTimer -= deltaTime;
        if (m_statusTimer <= 0) {
            m_statusMessage.clear();
        }
    }
}

void EditorState::render(SDL_Renderer* renderer) {
    // Bakgrund
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_RenderClear(renderer);
    
    // Titel
    SDL_Color titleColor = {255, 200, 100, 255};
    FontManager::instance().renderText(renderer, "default", 
        "=== RETRO ADVENTURE EDITOR ===", 180, 10, titleColor);
    
    // Tabs
    renderTabs(renderer);
    
    // Tab content
    switch (m_currentTab) {
        case EditorTab::Overview:   renderOverviewTab(renderer); break;
        case EditorTab::Rooms:      renderRoomsTab(renderer); break;
        case EditorTab::Dialogs:    renderDialogsTab(renderer); break;
        case EditorTab::Quests:     renderQuestsTab(renderer); break;
        case EditorTab::Items:      renderItemsTab(renderer); break;
        case EditorTab::WorldState: renderWorldStateTab(renderer); break;
        case EditorTab::Audio:      renderAudioTab(renderer); break;
    }
    
    // Status message
    if (!m_statusMessage.empty()) {
        SDL_Color statusColor = {100, 255, 100, 255};
        FontManager::instance().renderText(renderer, "default", m_statusMessage, 10, 365, statusColor);
    }
    
    // Footer
    SDL_Color footerColor = {150, 150, 150, 255};
    std::string footerText = "ESC: Back | TAB: Switch tabs | T: Toggle Tiled/Manual";
    FontManager::instance().renderText(renderer, "default", footerText, 10, 380, footerColor);
}

void EditorState::renderTabs(SDL_Renderer* renderer) {
    const char* tabNames[] = {
        "Overview", "Rooms", "Dialogs", "Quests", "Items", "WorldState", "Audio"
    };
    
    int x = 10;
    int y = 35;
    
    for (int i = 0; i < 7; i++) {
        bool selected = (static_cast<int>(m_currentTab) == i);
        
        SDL_Rect tabRect = {x, y, 85, 20};
        
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
        }
        SDL_RenderFillRect(renderer, &tabRect);
        
        SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
        SDL_RenderDrawRect(renderer, &tabRect);
        
        SDL_Color textColor = selected ? SDL_Color{255, 255, 100, 255} : SDL_Color{200, 200, 200, 255};
        FontManager::instance().renderText(renderer, "default", tabNames[i], x + 5, y + 3, textColor);
        
        x += 90;
    }
}

void EditorState::renderOverviewTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Game Overview - Room Flowchart]", 10, 65, cyan);
    
    renderRoomFlowchart(renderer);
}

void EditorState::renderRoomFlowchart(SDL_Renderer* renderer) {
    // Rita rum-noder
    for (const auto& node : m_roomNodes) {
        bool selected = (node.id == m_selectedRoom);
        
        // Bakgrund
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
        }
        
        SDL_Rect rect = {node.x, node.y, node.width, node.height};
        SDL_RenderFillRect(renderer, &rect);
        
        // Ram
        SDL_SetRenderDrawColor(renderer, 150, 150, 200, 255);
        SDL_RenderDrawRect(renderer, &rect);
        
        // Text
        SDL_Color textColor = selected ? SDL_Color{255, 255, 100, 255} : SDL_Color{255, 255, 255, 255};
        FontManager::instance().renderText(renderer, "default", node.id, node.x + 5, node.y + 12, textColor);
    }
    
    // Rita kopplingar mellan rum (baserat på exits)
    SDL_SetRenderDrawColor(renderer, 100, 150, 100, 255);
    auto& rooms = DataLoader::instance().getRooms();
    
    for (const auto& room : rooms) {
        auto fromIt = std::find_if(m_roomNodes.begin(), m_roomNodes.end(),
            [&](const RoomNode& n) { return n.id == room.id; });
        
        if (fromIt == m_roomNodes.end()) continue;
        
        for (const auto& hs : room.hotspots) {
            if (!hs.targetRoom.empty()) {
                auto toIt = std::find_if(m_roomNodes.begin(), m_roomNodes.end(),
                    [&](const RoomNode& n) { return n.id == hs.targetRoom; });
                
                if (toIt != m_roomNodes.end()) {
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

void EditorState::renderRoomsTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Rooms]", 10, 65, cyan);
    
    // Workflow toggle
    std::string workflowText = m_useTiledWorkflow ? "Mode: Tiled" : "Mode: Manual";
    FontManager::instance().renderText(renderer, "default", workflowText, 350, 68, 
                                      m_useTiledWorkflow ? green : yellow);
    
    // Import/Export knappar (bara synliga om Tiled mode)
    if (m_useTiledWorkflow) {
        renderButton(renderer, "Import Tiled", 450, 65, 90, 18, false);
        renderButton(renderer, "Export All", 550, 65, 80, 18, false);
    } else {
        renderButton(renderer, "Add Room", 450, 65, 80, 18, false);
        renderButton(renderer, "Edit Room", 550, 65, 80, 18, false);
    }
    
    auto& rooms = DataLoader::instance().getRooms();
    int y = 90;
    
    for (const auto& room : rooms) {
        bool selected = (room.id == m_selectedRoom);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + room.id + " (" + room.name + ")";
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (selected) {
            // Visa hotspots för valt rum
            FontManager::instance().renderText(renderer, "default", 
                "  Hotspots: " + std::to_string(room.hotspots.size()), 30, y, green);
            y += 12;
            
            for (const auto& hs : room.hotspots) {
                std::string hsText = "    - " + hs.name + " [" + hs.id + "]";
                if (!hs.targetRoom.empty()) {
                    hsText += " -> " + hs.targetRoom;
                }
                FontManager::instance().renderText(renderer, "default", hsText, 30, y, white);
                y += 12;
            }
        }
        
        if (y > 360) break;
    }
}

void EditorState::renderDialogsTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color magenta = {255, 100, 255, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Dialogs]", 10, 65, magenta);
    
    auto& dialogs = DataLoader::instance().getDialogs();
    int y = 85;
    
    for (const auto& dialog : dialogs) {
        bool selected = (dialog.id == m_selectedDialog);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + dialog.id + 
                          " (" + std::to_string(dialog.nodes.size()) + " nodes)";
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (selected && !dialog.nodes.empty()) {
            // Visa första noden
            FontManager::instance().renderText(renderer, "default",
                "  Start: " + dialog.nodes[0].text.substr(0, 40) + "...", 30, y, white);
            y += 12;
        }
        
        if (y > 360) break;
    }
}

void EditorState::renderQuestsTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color orange = {255, 180, 100, 255};
    SDL_Color green = {100, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Quests]", 10, 65, orange);
    
    auto& quests = DataLoader::instance().getQuests();
    int y = 85;
    
    for (const auto& quest : quests) {
        bool selected = (quest.id == m_selectedQuest);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + quest.id + ": " + quest.title;
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (selected) {
            FontManager::instance().renderText(renderer, "default",
                "  Objectives: " + std::to_string(quest.objectives.size()), 30, y, green);
            y += 12;
            
            for (const auto& obj : quest.objectives) {
                std::string objText = "    - " + obj.description;
                FontManager::instance().renderText(renderer, "default", objText, 30, y, white);
                y += 12;
            }
        }
        
        if (y > 360) break;
    }
}

void EditorState::renderItemsTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color blue = {100, 150, 255, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Items]", 10, 65, blue);
    
    auto& items = DataLoader::instance().getItems();
    int y = 85;
    
    for (const auto& item : items) {
        bool selected = (item.id == m_selectedItem);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + item.id + ": " + item.name;
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (selected) {
            FontManager::instance().renderText(renderer, "default",
                "  " + item.description, 30, y, white);
            y += 12;
            
            if (item.combinable) {
                FontManager::instance().renderText(renderer, "default",
                    "  Combines with: " + item.combinesWith + " -> " + item.combineResult, 
                    30, y, blue);
                y += 12;
            }
        }
        
        if (y > 360) break;
    }
}

void EditorState::renderWorldStateTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color red = {255, 100, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[WorldState - Flags & Counters]", 10, 65, green);
    
    auto& world = WorldState::instance();
    auto flags = world.getAllFlags();
    auto counters = world.getAllCounters();
    
    int y = 85;
    
    // Flags
    FontManager::instance().renderText(renderer, "default", 
        "Flags (" + std::to_string(flags.size()) + "):", 20, y, white);
    y += 15;
    
    for (const auto& [key, value] : flags) {
        SDL_Color color = value ? green : red;
        std::string text = "  " + key + ": " + (value ? "true" : "false");
        FontManager::instance().renderText(renderer, "default", text, 30, y, color);
        y += 12;
        if (y > 250) break;
    }
    
    y += 10;
    
    // Counters
    FontManager::instance().renderText(renderer, "default",
        "Counters (" + std::to_string(counters.size()) + "):", 20, y, white);
    y += 15;
    
    for (const auto& [key, value] : counters) {
        std::string text = "  " + key + ": " + std::to_string(value);
        FontManager::instance().renderText(renderer, "default", text, 30, y, white);
        y += 12;
        if (y > 360) break;
    }
}

void EditorState::renderAudioTab(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color purple = {200, 100, 255, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Audio]", 10, 65, purple);
    
    auto& audio = AudioManager::instance();
    int y = 85;
    
    std::string musicVol = "Music Volume: " + std::to_string(static_cast<int>(audio.getMusicVolume() * 100)) + "%";
    FontManager::instance().renderText(renderer, "default", musicVol, 20, y, white);
    y += 15;
    
    std::string sfxVol = "SFX Volume: " + std::to_string(static_cast<int>(audio.getSFXVolume() * 100)) + "%";
    FontManager::instance().renderText(renderer, "default", sfxVol, 20, y, white);
    y += 15;
    
    std::string muted = "Muted: " + std::string(audio.isMuted() ? "Yes" : "No");
    FontManager::instance().renderText(renderer, "default", muted, 20, y, white);
    y += 15;
    
    std::string state = "Music State: " + std::to_string(static_cast<int>(audio.getMusicState()));
    FontManager::instance().renderText(renderer, "default", state, 20, y, white);
}

void EditorState::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                if (m_game) {
                    m_game->popState();
                }
                break;
                
            case SDL_SCANCODE_TAB:
                if (event.key.keysym.mod & KMOD_SHIFT) {
                    handleTabSwitch(-1);
                } else {
                    handleTabSwitch(1);
                }
                break;
                
            case SDL_SCANCODE_T:
                // Toggle Tiled workflow
                m_useTiledWorkflow = !m_useTiledWorkflow;
                m_statusMessage = m_useTiledWorkflow ? 
                    "Switched to Tiled workflow" : "Switched to Manual editing";
                m_statusTimer = 2.0f;
                AudioManager::instance().playSound("select");
                break;
                
            case SDL_SCANCODE_UP:
            case SDL_SCANCODE_DOWN:
                // Navigation i listor - implementeras senare
                break;
                
            default:
                break;
        }
    }
    
    if (event.type == SDL_MOUSEMOTION) {
        m_mouseX = event.motion.x;
        m_mouseY = event.motion.y;
    }
    
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        handleMouseClick(event.button.x, event.button.y);
    }
}

void EditorState::handleTabSwitch(int direction) {
    int current = static_cast<int>(m_currentTab);
    current += direction;
    
    if (current < 0) current = 6;
    if (current > 6) current = 0;
    
    m_currentTab = static_cast<EditorTab>(current);
    AudioManager::instance().playSound("navigate");
}

void EditorState::handleMouseClick(int x, int y) {
    // Kolla tab-klick
    if (y >= 35 && y <= 55) {
        int tabIndex = (x - 10) / 90;
        if (tabIndex >= 0 && tabIndex < 7) {
            m_currentTab = static_cast<EditorTab>(tabIndex);
            AudioManager::instance().playSound("select");
        }
    }
    
    // Kolla rum-nod klick i Overview
    if (m_currentTab == EditorTab::Overview) {
        for (const auto& node : m_roomNodes) {
            if (x >= node.x && x <= node.x + node.width &&
                y >= node.y && y <= node.y + node.height) {
                m_selectedRoom = node.id;
                AudioManager::instance().playSound("select");
                break;
            }
        }
    }
    
    // Kolla workflow mode toggle (klicka på Mode-texten)
    if (m_currentTab == EditorTab::Rooms) {
        if (x >= 350 && x <= 450 && y >= 65 && y <= 80) {
            m_useTiledWorkflow = !m_useTiledWorkflow;
            m_statusMessage = m_useTiledWorkflow ? 
                "Switched to Tiled workflow" : "Switched to Manual editing";
            m_statusTimer = 2.0f;
            AudioManager::instance().playSound("select");
        }
    }
    
    // Kolla knapp-klick i Rooms tab
    if (m_currentTab == EditorTab::Rooms) {
        if (m_useTiledWorkflow) {
            // Tiled workflow knappar
            if (isButtonClicked(450, 65, 90, 18, x, y)) {
                importFromTiled();
                AudioManager::instance().playSound("select");
            }
            if (isButtonClicked(550, 65, 80, 18, x, y)) {
                exportAllToTiled();
                AudioManager::instance().playSound("select");
            }
        } else {
            // Manual editing knappar
            if (isButtonClicked(450, 65, 80, 18, x, y)) {
                m_statusMessage = "Add Room: Not yet implemented";
                m_statusTimer = 2.0f;
                AudioManager::instance().playSound("select");
            }
            if (isButtonClicked(550, 65, 80, 18, x, y)) {
                m_statusMessage = "Edit Room: Not yet implemented";
                m_statusTimer = 2.0f;
                AudioManager::instance().playSound("select");
            }
        }
    }
}

void EditorState::renderDialogTree(SDL_Renderer* renderer, const std::string& dialogId) {
    (void)renderer;
    (void)dialogId;
    // TODO: Implementera dialogträd-visualisering
}

void EditorState::renderButton(SDL_Renderer* renderer, const char* text, int x, int y, int w, int h, bool highlight) {
    // Bakgrund
    if (highlight) {
        SDL_SetRenderDrawColor(renderer, 80, 100, 80, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
    }
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    
    // Ram
    SDL_SetRenderDrawColor(renderer, 100, 150, 100, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    // Text
    SDL_Color textColor = {200, 255, 200, 255};
    FontManager::instance().renderText(renderer, "default", text, x + 4, y + 2, textColor);
}

bool EditorState::isButtonClicked(int btnX, int btnY, int btnW, int btnH, int clickX, int clickY) {
    return clickX >= btnX && clickX <= btnX + btnW &&
           clickY >= btnY && clickY <= btnY + btnH;
}

void EditorState::importFromTiled() {
    namespace fs = std::filesystem;
    
    std::string tiledFolder = "assets/tiled/";
    
    // Skapa mappen om den inte finns
    if (!fs::exists(tiledFolder)) {
        fs::create_directories(tiledFolder);
        m_statusMessage = "Created assets/tiled/ - add .json files there";
        m_statusTimer = 3.0f;
        LOG_INFO("Created tiled folder: " + tiledFolder);
        return;
    }
    
    // Importera alla .json filer
    int count = 0;
    for (const auto& entry : fs::directory_iterator(tiledFolder)) {
        if (entry.path().extension() == ".json") {
            auto room = TiledImporter::importRoom(entry.path().string());
            if (!room.id.empty()) {
                count++;
            }
        }
    }
    
    if (count > 0) {
        m_statusMessage = "Imported " + std::to_string(count) + " room(s) from Tiled";
        // Uppdatera rum-noder
        enter();  // Reload
    } else {
        m_statusMessage = "No .json files found in assets/tiled/";
    }
    m_statusTimer = 3.0f;
    LOG_INFO(m_statusMessage);
}

void EditorState::exportToTiled() {
    if (m_selectedRoom.empty()) {
        m_statusMessage = "Select a room first";
        m_statusTimer = 2.0f;
        return;
    }
    
    namespace fs = std::filesystem;
    std::string tiledFolder = "assets/tiled/";
    
    if (!fs::exists(tiledFolder)) {
        fs::create_directories(tiledFolder);
    }
    
    auto& rooms = DataLoader::instance().getRooms();
    for (const auto& room : rooms) {
        if (room.id == m_selectedRoom) {
            std::string path = tiledFolder + room.id + ".json";
            if (TiledImporter::exportRoom(room, path)) {
                m_statusMessage = "Exported " + room.id + " to " + path;
            } else {
                m_statusMessage = "Failed to export " + room.id;
            }
            m_statusTimer = 3.0f;
            LOG_INFO(m_statusMessage);
            break;
        }
    }
}

void EditorState::exportAllToTiled() {
    namespace fs = std::filesystem;
    std::string tiledFolder = "assets/tiled/";
    
    if (!fs::exists(tiledFolder)) {
        fs::create_directories(tiledFolder);
    }
    
    auto& rooms = DataLoader::instance().getRooms();
    int count = 0;
    
    for (const auto& room : rooms) {
        std::string path = tiledFolder + room.id + ".json";
        if (TiledImporter::exportRoom(room, path)) {
            count++;
        }
    }
    
    m_statusMessage = "Exported " + std::to_string(count) + " room(s) to Tiled format";
    m_statusTimer = 3.0f;
    LOG_INFO(m_statusMessage);
}
