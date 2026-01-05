/**
 * @file EditorState.h
 * @brief Separat editor state för att visualisera och redigera game content
 * 
 * Öppnas från huvudmenyn, inte som overlay under gameplay.
 */
#pragma once

#include "IState.h"
#include "../data/GameData.h"
#include <SDL.h>
#include <string>
#include <vector>

/**
 * @brief Editor tabs/modes
 */
enum class EditorTab {
    Overview,     // Flowchart av alla rum
    Rooms,        // Redigera rum och hotspots
    Dialogs,      // Redigera dialogträd
    Quests,       // Redigera quests
    Items,        // Redigera items
    WorldState,   // Flags och counters
    Audio         // Musik och ljud
};

/**
 * @brief Separat editor state
 */
class EditorState : public IState {
public:
    EditorState();
    ~EditorState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    // Tab rendering
    void renderTabs(SDL_Renderer* renderer);
    void renderOverviewTab(SDL_Renderer* renderer);
    void renderRoomsTab(SDL_Renderer* renderer);
    void renderDialogsTab(SDL_Renderer* renderer);
    void renderQuestsTab(SDL_Renderer* renderer);
    void renderItemsTab(SDL_Renderer* renderer);
    void renderWorldStateTab(SDL_Renderer* renderer);
    void renderAudioTab(SDL_Renderer* renderer);
    
    // Flowchart rendering
    void renderRoomFlowchart(SDL_Renderer* renderer);
    void renderDialogTree(SDL_Renderer* renderer, const std::string& dialogId);
    
    // Input handling
    void handleTabSwitch(int direction);
    void handleMouseClick(int x, int y);
    
    // Tiled import/export
    void importFromTiled();
    void exportToTiled();
    void exportAllToTiled();
    
    // Manual room editing
    void openRoomEditor();
    void renderRoomEditor(SDL_Renderer* renderer);
    void renderVisualEditor(SDL_Renderer* renderer);
    void saveRoomChanges();
    void loadRoomPreview(SDL_Renderer* renderer);
    
    void renderButton(SDL_Renderer* renderer, const char* text, int x, int y, int w, int h, bool highlight = false);
    bool isButtonClicked(int btnX, int btnY, int btnW, int btnH, int clickX, int clickY);
    void renderTextField(SDL_Renderer* renderer, const std::string& label, const std::string& value, int x, int y, int w, bool selected);
    
    EditorTab m_currentTab = EditorTab::Overview;
    std::string m_statusMessage;
    float m_statusTimer = 0.0f;
    
    // Editor settings
    bool m_useTiledWorkflow = false;  // Toggle mellan Tiled och manuell redigering
    
    // Scroll och selection
    int m_scrollY = 0;
    std::string m_selectedRoom;
    std::string m_selectedDialog;
    std::string m_selectedQuest;
    std::string m_selectedItem;
    
    // Room flowchart positions (beräknade)
    struct RoomNode {
        std::string id;
        int x, y;
        int width, height;
    };
    std::vector<RoomNode> m_roomNodes;
    
    // Mouse state
    int m_mouseX = 0;
    int m_mouseY = 0;
    bool m_isDragging = false;
    int m_dragOffsetX = 0;
    int m_dragOffsetY = 0;
    
    // Room editor state
    bool m_editingRoom = false;
    bool m_visualEditor = false;  // Visuell editor mode
    int m_selectedField = 0;  // Vilket fält som är valt för redigering
    bool m_editingText = false;  // Om vi redigerar text just nu
    std::string m_textBuffer;  // Buffer för text-input
    
    // Visual editor state
    int m_selectedHotspot = -1;  // Index av vald hotspot
    bool m_draggingHotspot = false;
    bool m_resizingHotspot = false;
    int m_resizeHandle = -1;  // 0-3 = hörn (TL, TR, BL, BR)
    int m_dragStartX = 0;
    int m_dragStartY = 0;
    int m_hotspotOrigX = 0;  // Original position för drag
    int m_hotspotOrigY = 0;
    int m_hotspotOrigW = 0;  // Original storlek för resize
    int m_hotspotOrigH = 0;
    SDL_Texture* m_roomPreviewTexture = nullptr;  // Cached room texture
    
    // Walk area editing
    bool m_editingWalkArea = false;
    int m_walkAreaHandle = -1;  // 0-3 = sidor (top, bottom, left, right)
    
    // Edit room data (kopierad från DataLoader)
    struct EditRoomData {
        std::string id;
        std::string name;
        std::string background;
        std::vector<LayerData> layers;
        WalkAreaData walkArea;
        std::vector<HotspotData> hotspots;
    };
    EditRoomData m_editRoomData;
};
