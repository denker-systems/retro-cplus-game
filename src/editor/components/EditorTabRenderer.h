/**
 * @file EditorTabRenderer.h
 * @brief Rendering av editor tabs
 */
#pragma once

#include <SDL.h>
#include <string>
#include <vector>

// Forward declarations
struct RoomNode {
    std::string id;
    int x, y;
    int width, height;
};

/**
 * @brief Hanterar rendering av olika editor tabs
 */
class EditorTabRenderer {
public:
    /** @brief Rendera Overview tab (room flowchart) */
    static void renderOverviewTab(SDL_Renderer* renderer, const std::vector<RoomNode>& roomNodes, const std::string& selectedRoom);
    
    /** @brief Rendera Rooms tab (room list) */
    static void renderRoomsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedRoom);
    
    /** @brief Rendera Dialogs tab */
    static void renderDialogsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedDialog);
    
    /** @brief Rendera Quests tab */
    static void renderQuestsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedQuest);
    
    /** @brief Rendera Items tab */
    static void renderItemsTab(SDL_Renderer* renderer, int scrollY, const std::string& selectedItem);
    
    /** @brief Rendera WorldState tab */
    static void renderWorldStateTab(SDL_Renderer* renderer, int scrollY);
    
    /** @brief Rendera Audio tab */
    static void renderAudioTab(SDL_Renderer* renderer);
    
    /** @brief Rendera room flowchart */
    static void renderRoomFlowchart(SDL_Renderer* renderer, const std::vector<RoomNode>& roomNodes, const std::string& selectedRoom);
};
