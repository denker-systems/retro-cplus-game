/**
 * @file EditorState.cpp
 * @brief Refaktorerad implementation av editor state
 */
#include "EditorState.h"
#include "../Game.h"
#include "../graphics/FontManager.h"
#include "../data/DataLoader.h"
#include "../editor/RoomDataManager.h"
#include "../editor/EditorTabRenderer.h"
#include "../editor/TiledIntegration.h"
#include "../audio/AudioManager.h"
#include "../utils/Logger.h"
#include <SDL_image.h>
#include <iostream>
#include <algorithm>

EditorState::EditorState() 
    : m_visualRoomEditor(std::make_unique<VisualRoomEditor>()) {
    LOG_INFO("EditorState created");
}

void EditorState::enter() {
    LOG_INFO("EditorState::enter()");
    m_currentTab = EditorTab::Overview;
    m_scrollY = 0;
    
    // Ladda all game data
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
    if (m_roomPreviewTexture) {
        SDL_DestroyTexture(m_roomPreviewTexture);
        m_roomPreviewTexture = nullptr;
    }
}

void EditorState::update(float deltaTime) {
    if (m_statusTimer > 0) {
        m_statusTimer -= deltaTime;
        if (m_statusTimer <= 0) {
            m_statusMessage.clear();
        }
    }
}

void EditorState::render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_RenderClear(renderer);
    
    SDL_Color titleColor = {255, 200, 100, 255};
    FontManager::instance().renderText(renderer, "default", 
        "=== RETRO ADVENTURE EDITOR ===", 180, 10, titleColor);
    
    renderTabs(renderer);
    
    switch (m_currentTab) {
        case EditorTab::Overview:   
            EditorTabRenderer::renderOverviewTab(renderer, m_roomNodes, m_selectedRoom); 
            break;
        case EditorTab::Rooms:      
            renderRoomsTab(renderer); 
            break;
        case EditorTab::Dialogs:    
            EditorTabRenderer::renderDialogsTab(renderer, m_scrollY, m_selectedDialog); 
            break;
        case EditorTab::Quests:     
            EditorTabRenderer::renderQuestsTab(renderer, m_scrollY, m_selectedQuest); 
            break;
        case EditorTab::Items:      
            EditorTabRenderer::renderItemsTab(renderer, m_scrollY, m_selectedItem); 
            break;
        case EditorTab::WorldState: 
            EditorTabRenderer::renderWorldStateTab(renderer, m_scrollY); 
            break;
        case EditorTab::Audio:      
            EditorTabRenderer::renderAudioTab(renderer); 
            break;
    }
    
    if (!m_statusMessage.empty()) {
        SDL_Color statusColor = {100, 255, 100, 255};
        FontManager::instance().renderText(renderer, "default", m_statusMessage, 10, 365, statusColor);
    }
    
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

void EditorState::renderRoomsTab(SDL_Renderer* renderer) {
    if (m_editingRoom) {
        renderRoomEditor(renderer);
        return;
    }
    
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Rooms]", 10, 65, cyan);
    
    std::string workflowText = m_useTiledWorkflow ? "Mode: Tiled" : "Mode: Manual";
    FontManager::instance().renderText(renderer, "default", workflowText, 350, 68, 
                                      m_useTiledWorkflow ? green : yellow);
    
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
        
        SDL_Rect clickArea = {20, y, 300, 15};
        if (m_mouseX >= clickArea.x && m_mouseX <= clickArea.x + clickArea.w &&
            m_mouseY >= clickArea.y && m_mouseY <= clickArea.y + clickArea.h) {
            SDL_SetRenderDrawColor(renderer, 60, 60, 80, 100);
            SDL_RenderFillRect(renderer, &clickArea);
        }
        
        FontManager::instance().renderText(renderer, "default", text, 20, y, color);
        y += 15;
        
        if (selected) {
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

void EditorState::renderRoomEditor(SDL_Renderer* renderer) {
    if (m_visualEditor) {
        renderVisualEditor(renderer);
        return;
    }
    
    // Text editor rendering (simplified)
    SDL_Color cyan = {100, 255, 255, 255};
    FontManager::instance().renderText(renderer, "default", 
        "[Text Editor: " + m_editRoomData.name + "]", 10, 65, cyan);
    
    renderButton(renderer, "Save", 400, 65, 60, 18, false);
    renderButton(renderer, "Visual", 470, 65, 60, 18, false);
    renderButton(renderer, "Cancel", 540, 65, 60, 18, false);
}

void EditorState::renderVisualEditor(SDL_Renderer* renderer) {
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", 
        "[Visual Editor: " + m_editRoomData.name + "]", 10, 65, cyan);
    
    renderButton(renderer, "Save", 400, 65, 60, 18, false);
    renderButton(renderer, "Text", 470, 65, 60, 18, false);
    renderButton(renderer, "Cancel", 540, 65, 60, 18, false);
    
    // Använd VisualRoomEditor för rendering
    m_visualRoomEditor->render(renderer, m_editRoomData, m_roomPreviewTexture);
    
    std::string instructions = "[+/-] Adjust depth scale | Drag magenta spawn | Drag cyan handles for walk area | Right-click: Add";
    FontManager::instance().renderText(renderer, "default", instructions, 10, 375, green);
}

void EditorState::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_ESCAPE:
                if (m_editingRoom) {
                    m_editingRoom = false;
                    m_visualEditor = false;
                    if (m_roomPreviewTexture) {
                        SDL_DestroyTexture(m_roomPreviewTexture);
                        m_roomPreviewTexture = nullptr;
                    }
                } else {
                    m_game->popState();
                }
                AudioManager::instance().playSound("select");
                return;
                
            case SDL_SCANCODE_TAB:
                handleTabSwitch(1);
                return;
                
            case SDL_SCANCODE_T:
                m_useTiledWorkflow = !m_useTiledWorkflow;
                m_statusMessage = m_useTiledWorkflow ? "Switched to Tiled workflow" : "Switched to Manual workflow";
                m_statusTimer = 2.0f;
                AudioManager::instance().playSound("select");
                return;
                
            case SDL_SCANCODE_M:
                AudioManager::instance().toggleMute();
                m_statusMessage = AudioManager::instance().isMuted() ? "Audio MUTED" : "Audio ON";
                m_statusTimer = 2.0f;
                return;
                
            default:
                break;
        }
    }
    
    if (event.type == SDL_MOUSEMOTION) {
        m_mouseX = event.motion.x;
        m_mouseY = event.motion.y;
        
        if (m_visualEditor && m_visualRoomEditor->isEditing()) {
            m_visualRoomEditor->handleMouseMotion(m_mouseX, m_mouseY, m_editRoomData);
        }
    }
    
    if (event.type == SDL_MOUSEBUTTONUP) {
        if (m_visualEditor && m_visualRoomEditor->isEditing()) {
            m_visualRoomEditor->handleMouseUp();
            m_statusMessage = "Updated";
            m_statusTimer = 1.5f;
        }
    }
    
    if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_DELETE) {
        if (m_visualEditor && m_visualRoomEditor->deleteSelectedHotspot(m_editRoomData)) {
            m_statusMessage = "Deleted hotspot";
            m_statusTimer = 2.0f;
            AudioManager::instance().playSound("select");
        }
    }
    
    // Depth scale adjustment
    if (event.type == SDL_KEYDOWN && m_visualEditor && m_visualRoomEditor->getSelectedHotspot() < 0) {
        bool shift = (event.key.keysym.mod & KMOD_SHIFT) != 0;
        
        if (event.key.keysym.scancode == SDL_SCANCODE_EQUALS || 
            event.key.keysym.scancode == SDL_SCANCODE_KP_PLUS) {
            m_visualRoomEditor->adjustDepthScale(true, shift, m_editRoomData);
            m_statusMessage = shift ? "Top scale increased" : "Bottom scale increased";
            m_statusTimer = 1.0f;
        }
        if (event.key.keysym.scancode == SDL_SCANCODE_MINUS || 
            event.key.keysym.scancode == SDL_SCANCODE_KP_MINUS) {
            m_visualRoomEditor->adjustDepthScale(false, shift, m_editRoomData);
            m_statusMessage = shift ? "Top scale decreased" : "Bottom scale decreased";
            m_statusTimer = 1.0f;
        }
    }
    
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            handleMouseClick(event.button.x, event.button.y);
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
    // Tab clicks
    if (y >= 35 && y <= 55) {
        int tabIndex = (x - 10) / 90;
        if (tabIndex >= 0 && tabIndex < 7) {
            m_currentTab = static_cast<EditorTab>(tabIndex);
            AudioManager::instance().playSound("select");
        }
    }
    
    // Room node clicks (Overview tab)
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
    
    // Room editor buttons
    if (m_currentTab == EditorTab::Rooms && m_editingRoom) {
        if (m_visualEditor) {
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
            
            // Visual editor clicks
            if (x >= 10 && x < 630 && y >= 90 && y < 350) {
                m_visualRoomEditor->handleMouseClick(x, y, m_editRoomData);
            }
        }
    }
    
    // Rooms tab buttons
    if (m_currentTab == EditorTab::Rooms && !m_editingRoom) {
        if (m_useTiledWorkflow) {
            if (isButtonClicked(450, 65, 90, 18, x, y)) {
                importFromTiled();
                AudioManager::instance().playSound("select");
            }
            if (isButtonClicked(550, 65, 80, 18, x, y)) {
                exportAllToTiled();
                AudioManager::instance().playSound("select");
            }
        } else {
            if (isButtonClicked(550, 65, 80, 18, x, y) && !m_selectedRoom.empty()) {
                openRoomEditor();
                AudioManager::instance().playSound("select");
            }
        }
        
        // Room list clicks
        auto& rooms = DataLoader::instance().getRooms();
        int listY = 90;
        for (const auto& room : rooms) {
            SDL_Rect clickArea = {20, listY, 300, 15};
            if (x >= clickArea.x && x <= clickArea.x + clickArea.w &&
                y >= clickArea.y && y <= clickArea.y + clickArea.h) {
                m_selectedRoom = room.id;
                AudioManager::instance().playSound("select");
                break;
            }
            listY += 15;
            if (m_selectedRoom == room.id) {
                listY += 12 + room.hotspots.size() * 12;
            }
        }
    }
}

void EditorState::openRoomEditor() {
    if (m_selectedRoom.empty()) {
        m_statusMessage = "Select a room first";
        m_statusTimer = 2.0f;
        return;
    }
    
    if (RoomDataManager::loadRoomForEditing(m_selectedRoom, m_editRoomData)) {
        m_editingRoom = true;
        m_visualEditor = true;
        m_selectedField = 0;
        m_statusMessage = "Editing: " + m_editRoomData.name;
        m_statusTimer = 2.0f;
        
        // Ladda preview texture
        if (m_roomPreviewTexture) {
            SDL_DestroyTexture(m_roomPreviewTexture);
        }
        m_roomPreviewTexture = RoomDataManager::loadRoomPreview(m_game->getRenderer(), m_editRoomData);
        
        LOG_INFO("Opening room editor for: " + m_selectedRoom);
    }
}

void EditorState::saveRoomChanges() {
    std::string message;
    if (RoomDataManager::saveRoomChanges(m_editRoomData, message)) {
        m_statusMessage = message;
        m_statusTimer = 3.0f;
        DataLoader::instance().loadAll();
        LOG_INFO("Saved room: " + m_editRoomData.id);
    } else {
        m_statusMessage = "Error: " + message;
        m_statusTimer = 3.0f;
    }
}

void EditorState::importFromTiled() {
    std::string message;
    if (TiledIntegration::importFromTiled(message)) {
        m_statusMessage = message;
        m_statusTimer = 3.0f;
    } else {
        m_statusMessage = "Error: " + message;
        m_statusTimer = 3.0f;
    }
}

void EditorState::exportAllToTiled() {
    std::string message;
    if (TiledIntegration::exportAllToTiled(message)) {
        m_statusMessage = message;
        m_statusTimer = 3.0f;
    } else {
        m_statusMessage = "Error: " + message;
        m_statusTimer = 3.0f;
    }
}

void EditorState::renderButton(SDL_Renderer* renderer, const char* text, int x, int y, int w, int h, bool highlight) {
    if (highlight) {
        SDL_SetRenderDrawColor(renderer, 80, 100, 80, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 60, 60, 80, 255);
    }
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    SDL_Color textColor = {255, 255, 255, 255};
    FontManager::instance().renderText(renderer, "default", text, x + 5, y + 3, textColor);
}

bool EditorState::isButtonClicked(int btnX, int btnY, int btnW, int btnH, int clickX, int clickY) {
    return clickX >= btnX && clickX <= btnX + btnW &&
           clickY >= btnY && clickY <= btnY + btnH;
}
