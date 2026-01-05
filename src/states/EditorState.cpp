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
#include <nlohmann/json.hpp>
#include <SDL_image.h>
#include <algorithm>
#include <filesystem>
#include <fstream>

using json = nlohmann::json;

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
    // Om vi redigerar ett rum, visa room editor istället
    if (m_editingRoom) {
        renderRoomEditor(renderer);
        return;
    }
    
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
        renderButton(renderer, "Edit Room", 550, 65, 80, 18, !m_selectedRoom.empty());
    }
    
    auto& rooms = DataLoader::instance().getRooms();
    int y = 90;
    
    for (const auto& room : rooms) {
        bool selected = (room.id == m_selectedRoom);
        SDL_Color color = selected ? SDL_Color{255, 255, 100, 255} : white;
        
        std::string text = (selected ? "> " : "  ") + room.id + " (" + room.name + ")";
        
        // Klickbar area för att välja rum
        SDL_Rect clickArea = {20, y, 300, 15};
        if (m_mouseX >= clickArea.x && m_mouseX <= clickArea.x + clickArea.w &&
            m_mouseY >= clickArea.y && m_mouseY <= clickArea.y + clickArea.h) {
            // Hover effect
            SDL_SetRenderDrawColor(renderer, 60, 60, 80, 100);
            SDL_RenderFillRect(renderer, &clickArea);
        }
        
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
                if (m_editingText) {
                    // Avbryt text-redigering
                    m_editingText = false;
                    SDL_StopTextInput();
                    AudioManager::instance().playSound("select");
                } else if (m_editingRoom) {
                    // Avbryt room-redigering
                    m_editingRoom = false;
                    m_statusMessage = "Cancelled editing";
                    m_statusTimer = 2.0f;
                } else if (m_game) {
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
                // Toggle Tiled workflow (bara om inte i room editor)
                if (!m_editingRoom) {
                    m_useTiledWorkflow = !m_useTiledWorkflow;
                    m_statusMessage = m_useTiledWorkflow ? 
                        "Switched to Tiled workflow" : "Switched to Manual editing";
                    m_statusTimer = 2.0f;
                    AudioManager::instance().playSound("select");
                }
                break;
                
            case SDL_SCANCODE_L:
                if (m_editingRoom && !m_editingText) {
                    // Lägg till ny layer
                    LayerData newLayer;
                    newLayer.image = "assets/backgrounds/new_layer.png";
                    newLayer.zIndex = 0;
                    newLayer.parallaxX = 1.0f;
                    newLayer.parallaxY = 1.0f;
                    newLayer.opacity = 1.0f;
                    m_editRoomData.layers.push_back(newLayer);
                    m_statusMessage = "Added new layer";
                    m_statusTimer = 2.0f;
                    AudioManager::instance().playSound("select");
                }
                break;
                
            case SDL_SCANCODE_H:
                if (m_editingRoom && !m_editingText) {
                    // Lägg till ny hotspot
                    HotspotData newHotspot;
                    newHotspot.id = "new_hotspot";
                    newHotspot.name = "New Hotspot";
                    newHotspot.type = "examine";
                    newHotspot.x = 100;
                    newHotspot.y = 100;
                    newHotspot.w = 50;
                    newHotspot.h = 50;
                    m_editRoomData.hotspots.push_back(newHotspot);
                    m_statusMessage = "Added new hotspot";
                    m_statusTimer = 2.0f;
                    AudioManager::instance().playSound("select");
                }
                break;
                
            case SDL_SCANCODE_UP:
                if (m_editingRoom && !m_editingText && m_selectedField > 0) {
                    m_selectedField--;
                    AudioManager::instance().playSound("navigate");
                }
                break;
                
            case SDL_SCANCODE_DOWN:
                if (m_editingRoom && !m_editingText && m_selectedField < 1) {
                    m_selectedField++;
                    AudioManager::instance().playSound("navigate");
                }
                break;
                
            case SDL_SCANCODE_RETURN:
                if (m_editingRoom && !m_editingText) {
                    // Börja redigera valt fält
                    m_editingText = true;
                    if (m_selectedField == 0) {
                        m_textBuffer = m_editRoomData.name;
                    } else if (m_selectedField == 1) {
                        m_textBuffer = m_editRoomData.background;
                    }
                    SDL_StartTextInput();
                } else if (m_editingText) {
                    // Spara text
                    if (m_selectedField == 0) {
                        m_editRoomData.name = m_textBuffer;
                    } else if (m_selectedField == 1) {
                        m_editRoomData.background = m_textBuffer;
                    }
                    m_editingText = false;
                    SDL_StopTextInput();
                    AudioManager::instance().playSound("select");
                }
                break;
                
            case SDL_SCANCODE_BACKSPACE:
                if (m_editingText && !m_textBuffer.empty()) {
                    m_textBuffer.pop_back();
                }
                break;
                
            default:
                break;
        }
    }
    
    // Text input
    if (event.type == SDL_TEXTINPUT && m_editingText) {
        m_textBuffer += event.text.text;
    }
    
    if (event.type == SDL_MOUSEMOTION) {
        m_mouseX = event.motion.x;
        m_mouseY = event.motion.y;
        
        float scaleX = 640.0f / 620.0f;
        float scaleY = 400.0f / 260.0f;
        
        // Resize hotspot
        if (m_visualEditor && m_resizingHotspot && m_selectedHotspot >= 0) {
            int deltaX = static_cast<int>((m_mouseX - m_dragStartX) * scaleX);
            int deltaY = static_cast<int>((m_mouseY - m_dragStartY) * scaleY);
            
            auto& hs = m_editRoomData.hotspots[m_selectedHotspot];
            
            // Handle baserat på vilket hörn (0=TL, 1=TR, 2=BL, 3=BR)
            switch (m_resizeHandle) {
                case 0: // Top-Left
                    hs.x = m_hotspotOrigX + deltaX;
                    hs.y = m_hotspotOrigY + deltaY;
                    hs.w = m_hotspotOrigW - deltaX;
                    hs.h = m_hotspotOrigH - deltaY;
                    break;
                case 1: // Top-Right
                    hs.y = m_hotspotOrigY + deltaY;
                    hs.w = m_hotspotOrigW + deltaX;
                    hs.h = m_hotspotOrigH - deltaY;
                    break;
                case 2: // Bottom-Left
                    hs.x = m_hotspotOrigX + deltaX;
                    hs.w = m_hotspotOrigW - deltaX;
                    hs.h = m_hotspotOrigH + deltaY;
                    break;
                case 3: // Bottom-Right
                    hs.w = m_hotspotOrigW + deltaX;
                    hs.h = m_hotspotOrigH + deltaY;
                    break;
            }
            
            // Min storlek
            if (hs.w < 10) hs.w = 10;
            if (hs.h < 10) hs.h = 10;
        }
        // Drag hotspot
        else if (m_visualEditor && m_draggingHotspot && m_selectedHotspot >= 0) {
            int deltaX = static_cast<int>((m_mouseX - m_dragStartX) * scaleX);
            int deltaY = static_cast<int>((m_mouseY - m_dragStartY) * scaleY);
            
            auto& hs = m_editRoomData.hotspots[m_selectedHotspot];
            hs.x = m_hotspotOrigX + deltaX;
            hs.y = m_hotspotOrigY + deltaY;
            
            // Clamp till rum-gränser
            if (hs.x < 0) hs.x = 0;
            if (hs.y < 0) hs.y = 0;
            if (hs.x + hs.w > 640) hs.x = 640 - hs.w;
            if (hs.y + hs.h > 400) hs.y = 400 - hs.h;
        }
        // Drag walk area handle
        else if (m_visualEditor && m_editingWalkArea && m_walkAreaHandle >= 0) {
            int deltaX = static_cast<int>((m_mouseX - m_dragStartX) * scaleX);
            int deltaY = static_cast<int>((m_mouseY - m_dragStartY) * scaleY);
            
            auto& wa = m_editRoomData.walkArea;
            
            switch (m_walkAreaHandle) {
                case 0: // Top
                    wa.minY = m_hotspotOrigY + deltaY;
                    break;
                case 1: // Bottom
                    wa.maxY = m_hotspotOrigH + deltaY;
                    break;
                case 2: // Left
                    wa.minX = m_hotspotOrigX + deltaX;
                    break;
                case 3: // Right
                    wa.maxX = m_hotspotOrigW + deltaX;
                    break;
            }
            
            // Clamp walk area
            if (wa.minX < 0) wa.minX = 0;
            if (wa.minY < 0) wa.minY = 0;
            if (wa.maxX > 640) wa.maxX = 640;
            if (wa.maxY > 400) wa.maxY = 400;
            if (wa.minX > wa.maxX - 20) wa.minX = wa.maxX - 20;
            if (wa.minY > wa.maxY - 20) wa.minY = wa.maxY - 20;
        }
    }
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            handleMouseClick(event.button.x, event.button.y);
        } else if (event.button.button == SDL_BUTTON_RIGHT) {
            // Right-click: Skapa ny hotspot i visual editor
            if (m_visualEditor && m_editingRoom) {
                int x = event.button.x;
                int y = event.button.y;
                
                // Kolla om klick är inom preview-området
                if (x >= 10 && x < 630 && y >= 90 && y < 350) {
                    float scaleX = 640.0f / 620.0f;
                    float scaleY = 400.0f / 260.0f;
                    
                    int roomX = static_cast<int>((x - 10) * scaleX);
                    int roomY = static_cast<int>((y - 90) * scaleY);
                    
                    HotspotData newHotspot;
                    newHotspot.id = "hotspot_" + std::to_string(m_editRoomData.hotspots.size());
                    newHotspot.name = "New Hotspot";
                    newHotspot.type = "examine";
                    newHotspot.x = roomX;
                    newHotspot.y = roomY;
                    newHotspot.w = 50;
                    newHotspot.h = 50;
                    
                    m_editRoomData.hotspots.push_back(newHotspot);
                    m_selectedHotspot = static_cast<int>(m_editRoomData.hotspots.size()) - 1;
                    m_statusMessage = "Created new hotspot";
                    m_statusTimer = 2.0f;
                    AudioManager::instance().playSound("select");
                }
            }
        }
    }
    
    if (event.type == SDL_MOUSEBUTTONUP && event.button.button == SDL_BUTTON_LEFT) {
        if (m_draggingHotspot) {
            m_draggingHotspot = false;
            m_statusMessage = "Moved hotspot";
            m_statusTimer = 1.5f;
        }
        if (m_resizingHotspot) {
            m_resizingHotspot = false;
            m_resizeHandle = -1;
            m_statusMessage = "Resized hotspot";
            m_statusTimer = 1.5f;
        }
        if (m_editingWalkArea) {
            m_editingWalkArea = false;
            m_walkAreaHandle = -1;
            m_statusMessage = "Updated walk area";
            m_statusTimer = 1.5f;
        }
    }
    
    // Delete hotspot
    if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_DELETE) {
        if (m_visualEditor && m_selectedHotspot >= 0 && 
            m_selectedHotspot < static_cast<int>(m_editRoomData.hotspots.size())) {
            m_editRoomData.hotspots.erase(m_editRoomData.hotspots.begin() + m_selectedHotspot);
            m_selectedHotspot = -1;
            m_statusMessage = "Deleted hotspot";
            m_statusTimer = 2.0f;
            AudioManager::instance().playSound("select");
        }
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
    
    // Room editor knappar
    if (m_currentTab == EditorTab::Rooms && m_editingRoom) {
        if (m_visualEditor) {
            // Visual editor knappar
            if (isButtonClicked(400, 65, 60, 18, x, y)) {
                saveRoomChanges();
                AudioManager::instance().playSound("select");
                return;
            }
            if (isButtonClicked(470, 65, 60, 18, x, y)) {
                m_visualEditor = false;
                AudioManager::instance().playSound("select");
                return;
            }
            if (isButtonClicked(540, 65, 60, 18, x, y)) {
                m_editingRoom = false;
                m_visualEditor = false;
                m_statusMessage = "Cancelled editing";
                m_statusTimer = 2.0f;
                AudioManager::instance().playSound("select");
                return;
            }
            
            // Hotspot klick/drag i visual editor
            // Kolla om klick är inom preview-området
            if (x >= 10 && x < 630 && y >= 90 && y < 350) {
                float scaleX = 620.0f / 640.0f;
                float scaleY = 260.0f / 400.0f;
                int hs_size = 6;
                int wh_size = 8;
                
                // Först: Kolla resize handles på vald hotspot
                if (m_selectedHotspot >= 0) {
                    const auto& hs = m_editRoomData.hotspots[m_selectedHotspot];
                    SDL_Rect rect = {
                        10 + static_cast<int>(hs.x * scaleX),
                        90 + static_cast<int>(hs.y * scaleY),
                        static_cast<int>(hs.w * scaleX),
                        static_cast<int>(hs.h * scaleY)
                    };
                    
                    SDL_Rect handles[4] = {
                        {rect.x - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},
                        {rect.x + rect.w - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},
                        {rect.x - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size},
                        {rect.x + rect.w - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size}
                    };
                    
                    for (int h = 0; h < 4; h++) {
                        if (x >= handles[h].x && x < handles[h].x + handles[h].w &&
                            y >= handles[h].y && y < handles[h].y + handles[h].h) {
                            m_resizingHotspot = true;
                            m_resizeHandle = h;
                            m_dragStartX = x;
                            m_dragStartY = y;
                            m_hotspotOrigX = hs.x;
                            m_hotspotOrigY = hs.y;
                            m_hotspotOrigW = hs.w;
                            m_hotspotOrigH = hs.h;
                            return;
                        }
                    }
                }
                
                // Sedan: Kolla walk area handles
                const auto& wa = m_editRoomData.walkArea;
                SDL_Rect walkRect = {
                    10 + static_cast<int>(wa.minX * scaleX),
                    90 + static_cast<int>(wa.minY * scaleY),
                    static_cast<int>((wa.maxX - wa.minX) * scaleX),
                    static_cast<int>((wa.maxY - wa.minY) * scaleY)
                };
                
                SDL_Rect walkHandles[4] = {
                    {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y - wh_size/2, wh_size, wh_size},
                    {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y + walkRect.h - wh_size/2, wh_size, wh_size},
                    {walkRect.x - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size},
                    {walkRect.x + walkRect.w - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size}
                };
                
                for (int h = 0; h < 4; h++) {
                    if (x >= walkHandles[h].x && x < walkHandles[h].x + walkHandles[h].w &&
                        y >= walkHandles[h].y && y < walkHandles[h].y + walkHandles[h].h) {
                        m_editingWalkArea = true;
                        m_walkAreaHandle = h;
                        m_dragStartX = x;
                        m_dragStartY = y;
                        m_hotspotOrigX = wa.minX;
                        m_hotspotOrigY = wa.minY;
                        m_hotspotOrigW = wa.maxX;
                        m_hotspotOrigH = wa.maxY;
                        m_selectedHotspot = -1;
                        return;
                    }
                }
                
                // Sist: Kolla om vi klickar på en hotspot
                int clickedHotspot = -1;
                for (int i = static_cast<int>(m_editRoomData.hotspots.size()) - 1; i >= 0; i--) {
                    const auto& hs = m_editRoomData.hotspots[i];
                    SDL_Rect rect = {
                        10 + static_cast<int>(hs.x * scaleX),
                        90 + static_cast<int>(hs.y * scaleY),
                        static_cast<int>(hs.w * scaleX),
                        static_cast<int>(hs.h * scaleY)
                    };
                    
                    if (x >= rect.x && x < rect.x + rect.w &&
                        y >= rect.y && y < rect.y + rect.h) {
                        clickedHotspot = i;
                        break;
                    }
                }
                
                if (clickedHotspot >= 0) {
                    m_selectedHotspot = clickedHotspot;
                    m_draggingHotspot = true;
                    m_dragStartX = x;
                    m_dragStartY = y;
                    m_hotspotOrigX = m_editRoomData.hotspots[clickedHotspot].x;
                    m_hotspotOrigY = m_editRoomData.hotspots[clickedHotspot].y;
                    m_hotspotOrigW = m_editRoomData.hotspots[clickedHotspot].w;
                    m_hotspotOrigH = m_editRoomData.hotspots[clickedHotspot].h;
                    AudioManager::instance().playSound("select");
                } else {
                    m_selectedHotspot = -1;
                }
                return;
            }
            
        } else {
            // Text editor knappar
            if (isButtonClicked(400, 65, 60, 18, x, y)) {
                saveRoomChanges();
                AudioManager::instance().playSound("select");
                return;
            }
            if (isButtonClicked(470, 65, 60, 18, x, y)) {
                m_visualEditor = true;
                loadRoomPreview(m_game->getRenderer());
                AudioManager::instance().playSound("select");
                return;
            }
            if (isButtonClicked(540, 65, 60, 18, x, y)) {
                m_editingRoom = false;
                m_statusMessage = "Cancelled editing";
                m_statusTimer = 2.0f;
                AudioManager::instance().playSound("select");
                return;
            }
        }
        return;  // Ignorera andra klick när vi redigerar
    }
    
    // Kolla workflow mode toggle (klicka på Mode-texten)
    if (m_currentTab == EditorTab::Rooms && !m_editingRoom) {
        if (x >= 350 && x <= 450 && y >= 65 && y <= 80) {
            m_useTiledWorkflow = !m_useTiledWorkflow;
            m_statusMessage = m_useTiledWorkflow ? 
                "Switched to Tiled workflow" : "Switched to Manual editing";
            m_statusTimer = 2.0f;
            AudioManager::instance().playSound("select");
        }
        
        // Klicka på rum för att välja
        int checkY = 90;
        auto& rooms = DataLoader::instance().getRooms();
        for (const auto& room : rooms) {
            if (x >= 20 && x <= 320 && y >= checkY && y <= checkY + 15) {
                m_selectedRoom = room.id;
                AudioManager::instance().playSound("select");
                break;
            }
            checkY += 15;
        }
    }
    
    // Kolla knapp-klick i Rooms tab
    if (m_currentTab == EditorTab::Rooms && !m_editingRoom) {
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
                openRoomEditor();
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

void EditorState::openRoomEditor() {
    if (m_selectedRoom.empty()) {
        m_statusMessage = "Select a room first";
        m_statusTimer = 2.0f;
        return;
    }
    
    // Hitta valt rum
    auto& rooms = DataLoader::instance().getRooms();
    for (const auto& room : rooms) {
        if (room.id == m_selectedRoom) {
            // Kopiera data
            m_editRoomData.id = room.id;
            m_editRoomData.name = room.name;
            m_editRoomData.background = room.background;
            m_editRoomData.layers = room.layers;
            m_editRoomData.walkArea = room.walkArea;
            m_editRoomData.hotspots = room.hotspots;
            
            m_editingRoom = true;
            m_selectedField = 0;
            m_statusMessage = "Editing: " + room.name;
            m_statusTimer = 2.0f;
            LOG_INFO("Opening room editor for: " + room.id);
            return;
        }
    }
}

void EditorState::renderRoomEditor(SDL_Renderer* renderer) {
    // Om visuell editor är aktiv, visa den istället
    if (m_visualEditor) {
        renderVisualEditor(renderer);
        return;
    }
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    SDL_Color green = {100, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", 
        "[Edit Room: " + m_editRoomData.name + "]", 10, 65, cyan);
    
    // Knappar
    renderButton(renderer, "Save", 400, 65, 60, 18, false);
    renderButton(renderer, "Visual", 470, 65, 60, 18, false);
    renderButton(renderer, "Cancel", 540, 65, 60, 18, false);
    
    int y = 90;
    
    // Room ID (read-only)
    FontManager::instance().renderText(renderer, "default", "ID: " + m_editRoomData.id, 20, y, white);
    y += 20;
    
    // Room Name
    std::string nameValue = (m_editingText && m_selectedField == 0) ? m_textBuffer : m_editRoomData.name;
    renderTextField(renderer, "Name:", nameValue, 20, y, 300, m_selectedField == 0);
    y += 25;
    
    // Background
    std::string bgValue = (m_editingText && m_selectedField == 1) ? m_textBuffer : m_editRoomData.background;
    renderTextField(renderer, "Background:", bgValue, 20, y, 400, m_selectedField == 1);
    y += 25;
    
    // Walk Area
    FontManager::instance().renderText(renderer, "default", "Walk Area:", 20, y, yellow);
    y += 15;
    std::string walkAreaText = "  X: " + std::to_string(m_editRoomData.walkArea.minX) + 
                               " to " + std::to_string(m_editRoomData.walkArea.maxX) +
                               " | Y: " + std::to_string(m_editRoomData.walkArea.minY) +
                               " to " + std::to_string(m_editRoomData.walkArea.maxY);
    FontManager::instance().renderText(renderer, "default", walkAreaText, 20, y, white);
    y += 20;
    
    // Layers
    FontManager::instance().renderText(renderer, "default", 
        "Layers: " + std::to_string(m_editRoomData.layers.size()), 20, y, yellow);
    y += 15;
    for (size_t i = 0; i < m_editRoomData.layers.size(); i++) {
        const auto& layer = m_editRoomData.layers[i];
        std::string layerText = "  [" + std::to_string(i) + "] " + layer.image + 
                               " (z:" + std::to_string(layer.zIndex) + ")";
        FontManager::instance().renderText(renderer, "default", layerText, 20, y, white);
        y += 12;
    }
    y += 10;
    
    // Hotspots
    FontManager::instance().renderText(renderer, "default",
        "Hotspots: " + std::to_string(m_editRoomData.hotspots.size()), 20, y, yellow);
    y += 15;
    for (size_t i = 0; i < m_editRoomData.hotspots.size() && y < 350; i++) {
        const auto& hs = m_editRoomData.hotspots[i];
        std::string hsText = "  [" + std::to_string(i) + "] " + hs.name + " (" + hs.type + ")";
        FontManager::instance().renderText(renderer, "default", hsText, 20, y, white);
        y += 12;
    }
    
    // Instruktioner
    std::string instructions = m_editingText ? 
        "Type to edit | ENTER: Save | ESC: Cancel" :
        "UP/DOWN: Navigate | ENTER: Edit | L: Add Layer | H: Add Hotspot";
    FontManager::instance().renderText(renderer, "default", instructions, 10, 365, green);
}

void EditorState::saveRoomChanges() {
    namespace fs = std::filesystem;
    
    // Läs befintlig rooms.json
    std::string roomsPath = "assets/data/rooms.json";
    std::ifstream inFile(roomsPath);
    if (!inFile.is_open()) {
        m_statusMessage = "Error: Could not open rooms.json";
        m_statusTimer = 3.0f;
        return;
    }
    
    json data;
    try {
        data = json::parse(inFile);
        inFile.close();
    } catch (const json::exception& e) {
        m_statusMessage = "Error parsing JSON: " + std::string(e.what());
        m_statusTimer = 3.0f;
        return;
    }
    
    // Hitta och uppdatera rummet
    bool found = false;
    if (data.contains("rooms") && data["rooms"].is_array()) {
        for (auto& room : data["rooms"]) {
            if (room["id"] == m_editRoomData.id) {
                // Uppdatera room data
                room["name"] = m_editRoomData.name;
                room["background"] = m_editRoomData.background;
                
                // Uppdatera layers
                room["layers"] = json::array();
                for (const auto& layer : m_editRoomData.layers) {
                    json layerJson;
                    layerJson["image"] = layer.image;
                    layerJson["zIndex"] = layer.zIndex;
                    layerJson["parallaxX"] = layer.parallaxX;
                    layerJson["parallaxY"] = layer.parallaxY;
                    layerJson["opacity"] = layer.opacity;
                    room["layers"].push_back(layerJson);
                }
                
                // Uppdatera walk area
                room["walkArea"]["minX"] = m_editRoomData.walkArea.minX;
                room["walkArea"]["maxX"] = m_editRoomData.walkArea.maxX;
                room["walkArea"]["minY"] = m_editRoomData.walkArea.minY;
                room["walkArea"]["maxY"] = m_editRoomData.walkArea.maxY;
                
                // Hotspots uppdateras inte här (för komplex)
                
                found = true;
                break;
            }
        }
    }
    
    if (!found) {
        m_statusMessage = "Error: Room not found in JSON";
        m_statusTimer = 3.0f;
        return;
    }
    
    // Skriv tillbaka till fil
    std::ofstream outFile(roomsPath);
    if (!outFile.is_open()) {
        m_statusMessage = "Error: Could not write to rooms.json";
        m_statusTimer = 3.0f;
        return;
    }
    
    outFile << data.dump(2);
    outFile.close();
    
    m_statusMessage = "Saved changes to " + m_editRoomData.id;
    m_statusTimer = 2.0f;
    m_editingRoom = false;
    
    // Reload data
    DataLoader::instance().loadAll();
    
    LOG_INFO("Saved room: " + m_editRoomData.id);
}

void EditorState::renderTextField(SDL_Renderer* renderer, const std::string& label, 
                                  const std::string& value, int x, int y, int w, bool selected) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    
    // Label
    FontManager::instance().renderText(renderer, "default", label, x, y, cyan);
    
    // Value box
    SDL_Rect box = {x + 100, y - 2, w, 16};
    
    if (selected) {
        SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
    }
    SDL_RenderFillRect(renderer, &box);
    
    SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
    SDL_RenderDrawRect(renderer, &box);
    
    // Value text
    FontManager::instance().renderText(renderer, "default", value, x + 105, y, 
                                      selected ? yellow : white);
}

void EditorState::loadRoomPreview(SDL_Renderer* renderer) {
    // Ladda bakgrundstextur för preview
    if (m_roomPreviewTexture) {
        SDL_DestroyTexture(m_roomPreviewTexture);
        m_roomPreviewTexture = nullptr;
    }
    
    // Ladda första layer eller background
    std::string imagePath;
    if (!m_editRoomData.layers.empty()) {
        imagePath = m_editRoomData.layers[0].image;
    } else if (!m_editRoomData.background.empty()) {
        imagePath = m_editRoomData.background;
    }
    
    if (!imagePath.empty()) {
        m_roomPreviewTexture = IMG_LoadTexture(renderer, imagePath.c_str());
        if (!m_roomPreviewTexture) {
            LOG_WARNING("Failed to load room preview: " + imagePath);
        }
    }
}

void EditorState::renderVisualEditor(SDL_Renderer* renderer) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color red = {255, 100, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", 
        "[Visual Editor: " + m_editRoomData.name + "]", 10, 65, cyan);
    
    // Knappar
    renderButton(renderer, "Save", 400, 65, 60, 18, false);
    renderButton(renderer, "Text", 470, 65, 60, 18, false);
    renderButton(renderer, "Cancel", 540, 65, 60, 18, false);
    
    // Rita bakgrund
    if (m_roomPreviewTexture) {
        SDL_Rect dstRect = {10, 90, 620, 260};
        SDL_RenderCopy(renderer, m_roomPreviewTexture, nullptr, &dstRect);
    } else {
        // Fallback
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        SDL_Rect bg = {10, 90, 620, 260};
        SDL_RenderFillRect(renderer, &bg);
    }
    
    // Rita hotspots
    for (size_t i = 0; i < m_editRoomData.hotspots.size(); i++) {
        const auto& hs = m_editRoomData.hotspots[i];
        bool selected = (static_cast<int>(i) == m_selectedHotspot);
        
        // Skala hotspot till preview-storlek (620x260 istället för 640x400)
        float scaleX = 620.0f / 640.0f;
        float scaleY = 260.0f / 400.0f;
        SDL_Rect rect = {
            10 + static_cast<int>(hs.x * scaleX),
            90 + static_cast<int>(hs.y * scaleY),
            static_cast<int>(hs.w * scaleX),
            static_cast<int>(hs.h * scaleY)
        };
        
        // Färg baserat på typ
        if (hs.type == "npc") {
            SDL_SetRenderDrawColor(renderer, 100, 255, 100, selected ? 200 : 100);
        } else if (hs.type == "item") {
            SDL_SetRenderDrawColor(renderer, 255, 255, 100, selected ? 200 : 100);
        } else if (hs.type == "exit") {
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, selected ? 200 : 100);
        } else {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, selected ? 200 : 100);
        }
        
        SDL_RenderFillRect(renderer, &rect);
        
        // Ram (tjockare för vald)
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
            // Rita dubbel ram för tydlighet
            SDL_Rect outerRect = {rect.x - 1, rect.y - 1, rect.w + 2, rect.h + 2};
            SDL_RenderDrawRect(renderer, &outerRect);
            
            // Rita resize handles (hörn)
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            int hs_size = 6;
            SDL_Rect handles[4] = {
                {rect.x - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},  // TL
                {rect.x + rect.w - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},  // TR
                {rect.x - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size},  // BL
                {rect.x + rect.w - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size}  // BR
            };
            for (int h = 0; h < 4; h++) {
                SDL_RenderFillRect(renderer, &handles[h]);
            }
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
        
        // Namn (visa alltid för vald, annars bara typ-ikon)
        if (selected) {
            FontManager::instance().renderText(renderer, "default", hs.name, rect.x + 2, rect.y + 2, white);
        }
    }
    
    // Rita walk area
    float scaleX = 620.0f / 640.0f;
    float scaleY = 260.0f / 400.0f;
    const auto& wa = m_editRoomData.walkArea;
    SDL_Rect walkRect = {
        10 + static_cast<int>(wa.minX * scaleX),
        90 + static_cast<int>(wa.minY * scaleY),
        static_cast<int>((wa.maxX - wa.minX) * scaleX),
        static_cast<int>((wa.maxY - wa.minY) * scaleY)
    };
    
    // Walk area outline (streckad effekt)
    SDL_SetRenderDrawColor(renderer, 0, 255, 200, m_editingWalkArea ? 200 : 100);
    SDL_RenderDrawRect(renderer, &walkRect);
    // Rita inre streckad linje
    SDL_Rect innerWalk = {walkRect.x + 1, walkRect.y + 1, walkRect.w - 2, walkRect.h - 2};
    SDL_RenderDrawRect(renderer, &innerWalk);
    
    // Walk area handles (mitt på varje sida)
    if (m_editingWalkArea || m_selectedHotspot < 0) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 200, 255);
        int wh_size = 8;
        SDL_Rect walkHandles[4] = {
            {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y - wh_size/2, wh_size, wh_size},  // Top
            {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y + walkRect.h - wh_size/2, wh_size, wh_size},  // Bottom
            {walkRect.x - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size},  // Left
            {walkRect.x + walkRect.w - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size}   // Right
        };
        for (int h = 0; h < 4; h++) {
            SDL_RenderFillRect(renderer, &walkHandles[h]);
        }
    }
    
    // Visa vald hotspot info
    if (m_selectedHotspot >= 0 && m_selectedHotspot < static_cast<int>(m_editRoomData.hotspots.size())) {
        const auto& hs = m_editRoomData.hotspots[m_selectedHotspot];
        std::string info = "Selected: " + hs.name + " [" + hs.type + "] " + 
                          std::to_string(hs.w) + "x" + std::to_string(hs.h) + " at (" + 
                          std::to_string(hs.x) + "," + std::to_string(hs.y) + ")";
        FontManager::instance().renderText(renderer, "default", info, 10, 355, yellow);
    } else {
        // Visa walk area info
        std::string walkInfo = "Walk Area: (" + std::to_string(wa.minX) + "," + std::to_string(wa.minY) + 
                              ") to (" + std::to_string(wa.maxX) + "," + std::to_string(wa.maxY) + ")";
        FontManager::instance().renderText(renderer, "default", walkInfo, 10, 355, cyan);
    }
    
    // Instruktioner
    std::string instructions = "Drag corners to resize | Drag cyan handles for walk area | Right-click: Add | DEL: Delete";
    FontManager::instance().renderText(renderer, "default", instructions, 10, 375, green);
}
