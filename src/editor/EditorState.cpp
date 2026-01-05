/**
 * @file EditorState.cpp
 * @brief Refaktorerad implementation av editor state
 */
#include "EditorState.h"
#include "game/Game.h"
#include "engine/graphics/FontManager.h"
#include "engine/data/DataLoader.h"
#include "editor/components/RoomDataManager.h"
#include "editor/components/EditorTabRenderer.h"
#include "editor/components/TiledIntegration.h"
#include "engine/audio/AudioManager.h"
#include "engine/utils/Logger.h"
#include <SDL_image.h>
#include <iostream>
#include <algorithm>

#ifdef HAS_IMGUI
#include "editor/ImGuiManager.h"
#include "editor/panels/HierarchyPanel.h"
#include "editor/panels/PropertiesPanel.h"
#include "editor/panels/ViewportPanel.h"
#include "editor/panels/AssetBrowserPanel.h"
#include "editor/panels/ConsolePanel.h"
#include <imgui.h>
#include <imgui_internal.h>
#endif

EditorState::EditorState() 
    : m_visualRoomEditor(std::make_unique<VisualRoomEditor>()) {
    LOG_INFO("EditorState created");
}

void EditorState::enter() {
    LOG_INFO("EditorState::enter()");
    m_currentTab = EditorTab::Overview;
    m_scrollY = 0;
    
#ifdef HAS_IMGUI
    // Initiera ImGui
    if (m_game) {
        ImGuiManager::instance().init(m_game->getWindow(), m_game->getRenderer());
    }
    
    // Skapa paneler
    m_hierarchyPanel = std::make_unique<HierarchyPanel>(m_editorContext);
    m_propertiesPanel = std::make_unique<PropertiesPanel>(m_editorContext);
    m_viewportPanel = std::make_unique<ViewportPanel>(m_editorContext);
    m_viewportPanel->setRenderer(m_game->getRenderer());
    m_assetBrowserPanel = std::make_unique<AssetBrowserPanel>(m_editorContext);
    m_consolePanel = std::make_unique<ConsolePanel>(m_editorContext);
    
    m_consolePanel->log("Editor initialized with panel architecture");
#endif
    
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
    // Hämta skärmupplösning för dynamisk layout
    int screenW = m_game ? m_game->getScreenWidth() : 1920;
    int screenH = m_game ? m_game->getScreenHeight() : 1080;
    
    SDL_SetRenderDrawColor(renderer, 30, 30, 40, 255);
    SDL_RenderClear(renderer);
    
    // Skala baserat på skärmbredd (referens: 1920px)
    float scale = screenW / 1920.0f;
    int fontSize = static_cast<int>(16 * scale);
    
    SDL_Color titleColor = {255, 200, 100, 255};
    FontManager::instance().renderText(renderer, "default", 
        "=== RETRO ADVENTURE EDITOR ===", static_cast<int>(screenW * 0.35f), static_cast<int>(20 * scale), titleColor);
    
    renderTabs(renderer, screenW, screenH, scale);
    
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
        FontManager::instance().renderText(renderer, "default", m_statusMessage, 
            static_cast<int>(20 * scale), screenH - static_cast<int>(60 * scale), statusColor);
    }
    
    SDL_Color footerColor = {150, 150, 150, 255};
    std::string footerText = "ESC: Exit | TAB: Switch tabs | T: Toggle Tiled/Manual";
    FontManager::instance().renderText(renderer, "default", footerText, 
        static_cast<int>(20 * scale), screenH - static_cast<int>(30 * scale), footerColor);
    
#ifdef HAS_IMGUI
    // Render ImGui overlay (ingen logisk skalning behövs längre)
    ImGuiManager::instance().beginFrame();
    renderImGui();
    ImGuiManager::instance().endFrame();
#endif
}

void EditorState::renderTabs(SDL_Renderer* renderer, int screenW, int screenH, float scale) {
    const char* tabNames[] = {
        "Overview", "Rooms", "Dialogs", "Quests", "Items", "WorldState", "Audio"
    };
    
    int tabWidth = static_cast<int>(120 * scale);
    int tabHeight = static_cast<int>(30 * scale);
    int tabSpacing = static_cast<int>(10 * scale);
    int x = static_cast<int>(20 * scale);
    int y = static_cast<int>(60 * scale);
    
    for (int i = 0; i < 7; i++) {
        bool selected = (static_cast<int>(m_currentTab) == i);
        
        SDL_Rect tabRect = {x, y, tabWidth, tabHeight};
        
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 80, 80, 120, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 50, 50, 70, 255);
        }
        SDL_RenderFillRect(renderer, &tabRect);
        
        SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
        SDL_RenderDrawRect(renderer, &tabRect);
        
        SDL_Color textColor = selected ? SDL_Color{255, 255, 100, 255} : SDL_Color{200, 200, 200, 255};
        FontManager::instance().renderText(renderer, "default", tabNames[i], x + static_cast<int>(10 * scale), y + static_cast<int>(8 * scale), textColor);
        
        x += tabWidth + tabSpacing;
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
#ifdef HAS_IMGUI
    // Låt ImGui processa events först
    ImGuiManager::instance().processEvent(event);
    
    // Om ImGui vill ha input, skippa vår handling
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return;
    }
#endif
    
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

void EditorState::renderImGui() {
#ifdef HAS_IMGUI
    static bool showDemoWindow = false;
    static bool firstTime = true;
    
    // Hämta skärmstorlek
    ImGuiIO& io = ImGui::GetIO();
    float screenW = io.DisplaySize.x;
    float screenH = io.DisplaySize.y;
    
    // Synka EditorContext med gamla selection-variabler
    m_editorContext.selectedRoomId = m_selectedRoom;
    m_editorContext.selectedDialogId = m_selectedDialog;
    m_editorContext.selectedQuestId = m_selectedQuest;
    m_editorContext.selectedItemId = m_selectedItem;
    m_editorContext.statusMessage = m_statusMessage;
    
    // === HUVUDMENY ===
    float menuBarHeight = 0;
    if (ImGui::BeginMainMenuBar()) {
        menuBarHeight = ImGui::GetWindowSize().y;
        
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save All", "Ctrl+S")) {
                m_editorContext.saveToFiles();
                if (m_consolePanel) m_consolePanel->log("Saved all data");
            }
            if (ImGui::MenuItem("Import from Tiled...")) {
                importFromTiled();
            }
            if (ImGui::MenuItem("Export to Tiled...")) {
                exportAllToTiled();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "ESC")) {
                if (m_game) m_game->popState();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Edit")) {
            if (ImGui::MenuItem("Undo", "Ctrl+Z", false, false)) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {}
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("View")) {
            bool hierarchyVisible = m_hierarchyPanel && m_hierarchyPanel->isVisible();
            bool viewportVisible = m_viewportPanel && m_viewportPanel->isVisible();
            bool propertiesVisible = m_propertiesPanel && m_propertiesPanel->isVisible();
            bool assetBrowserVisible = m_assetBrowserPanel && m_assetBrowserPanel->isVisible();
            bool consoleVisible = m_consolePanel && m_consolePanel->isVisible();
            
            if (ImGui::MenuItem("Hierarchy", nullptr, &hierarchyVisible)) {
                if (m_hierarchyPanel) m_hierarchyPanel->setVisible(hierarchyVisible);
            }
            if (ImGui::MenuItem("Viewport", nullptr, &viewportVisible)) {
                if (m_viewportPanel) m_viewportPanel->setVisible(viewportVisible);
            }
            if (ImGui::MenuItem("Properties", nullptr, &propertiesVisible)) {
                if (m_propertiesPanel) m_propertiesPanel->setVisible(propertiesVisible);
            }
            if (ImGui::MenuItem("Asset Browser", nullptr, &assetBrowserVisible)) {
                if (m_assetBrowserPanel) m_assetBrowserPanel->setVisible(assetBrowserVisible);
            }
            if (ImGui::MenuItem("Console", nullptr, &consoleVisible)) {
                if (m_consolePanel) m_consolePanel->setVisible(consoleVisible);
            }
            ImGui::Separator();
            ImGui::MenuItem("ImGui Demo", nullptr, &showDemoWindow);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Tools")) {
            if (ImGui::MenuItem("Validate All Data")) {
                if (m_consolePanel) m_consolePanel->logWarning("Validation not implemented yet");
            }
            if (ImGui::MenuItem("Reload Data")) {
                DataLoader::instance().loadAll();
                if (m_consolePanel) m_consolePanel->log("Data reloaded");
            }
            if (ImGui::MenuItem("Refresh Assets")) {
                if (m_assetBrowserPanel) {
                    m_assetBrowserPanel->refreshAssets();
                    if (m_consolePanel) m_consolePanel->log("Assets refreshed");
                }
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
    
    // === DOCKSPACE ===
    ImGuiWindowFlags dockspaceFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    
    ImGui::SetNextWindowPos(ImVec2(0, menuBarHeight));
    ImGui::SetNextWindowSize(ImVec2(screenW, screenH - menuBarHeight));
    
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("DockSpaceWindow", nullptr, dockspaceFlags);
    ImGui::PopStyleVar();
    
    ImGuiID dockspaceId = ImGui::GetID("EditorDockspace");
    ImGui::DockSpace(dockspaceId, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    
    // Sätt upp default layout första gången
    if (firstTime) {
        firstTime = false;
        ImGui::DockBuilderRemoveNode(dockspaceId);
        ImGui::DockBuilderAddNode(dockspaceId, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, ImVec2(screenW, screenH - menuBarHeight));
        
        ImGuiID dockLeft, dockRight, dockBottom, dockCenter;
        ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.2f, &dockLeft, &dockCenter);
        ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, 0.3f, &dockRight, &dockCenter);
        ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, 0.25f, &dockBottom, &dockCenter);
        
        // Dela höger panel för Properties och Asset Browser
        ImGuiID dockRightTop, dockRightBottom;
        ImGui::DockBuilderSplitNode(dockRight, ImGuiDir_Down, 0.5f, &dockRightBottom, &dockRightTop);
        
        ImGui::DockBuilderDockWindow("Hierarchy", dockLeft);
        ImGui::DockBuilderDockWindow("Viewport", dockCenter);
        ImGui::DockBuilderDockWindow("Properties", dockRightTop);
        ImGui::DockBuilderDockWindow("Asset Browser", dockRightBottom);
        ImGui::DockBuilderDockWindow("Console", dockBottom);
        ImGui::DockBuilderFinish(dockspaceId);
    }
    
    ImGui::End();
    
    // === RENDERA PANELER ===
    if (m_hierarchyPanel) m_hierarchyPanel->render();
    if (m_viewportPanel) {
        m_viewportPanel->update(0.016f);
        m_viewportPanel->render();
    }
    if (m_propertiesPanel) m_propertiesPanel->render();
    if (m_assetBrowserPanel) m_assetBrowserPanel->render();
    if (m_consolePanel) m_consolePanel->render();
    
    // Demo window
    if (showDemoWindow) {
        ImGui::ShowDemoWindow(&showDemoWindow);
    }
    
    // Synka tillbaka selection
    m_selectedRoom = m_editorContext.selectedRoomId;
    m_selectedDialog = m_editorContext.selectedDialogId;
    m_selectedQuest = m_editorContext.selectedQuestId;
    m_selectedItem = m_editorContext.selectedItemId;
#endif
}

bool EditorState::isButtonClicked(int btnX, int btnY, int btnW, int btnH, int clickX, int clickY) {
    return clickX >= btnX && clickX <= btnX + btnW &&
           clickY >= btnY && clickY <= btnY + btnH;
}
