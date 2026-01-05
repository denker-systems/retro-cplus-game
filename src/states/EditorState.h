/**
 * @file EditorState.h
 * @brief Separat editor state för att visualisera och redigera game content
 * 
 * Öppnas från huvudmenyn, inte som overlay under gameplay.
 */
#pragma once

#include "IState.h"
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
    void renderButton(SDL_Renderer* renderer, const char* text, int x, int y, int w, int h, bool highlight = false);
    bool isButtonClicked(int btnX, int btnY, int btnW, int btnH, int clickX, int clickY);
    
    EditorTab m_currentTab = EditorTab::Overview;
    std::string m_statusMessage;
    float m_statusTimer = 0.0f;
    
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
};
