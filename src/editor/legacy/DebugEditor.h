/**
 * @file DebugEditor.h
 * @brief In-game debug editor med Dear ImGui
 * 
 * Visar flowchart/tree-view av:
 * - Rooms och hotspots
 * - Dialogs och nodes
 * - Quests och objectives
 * - WorldState (flags/counters)
 * - NPCs och AI
 */
#pragma once

#include <SDL.h>
#include <string>
#include <functional>

/**
 * @brief Debug editor overlay
 */
class DebugEditor {
public:
    static DebugEditor& instance();
    
    /** @brief Initiera ImGui med SDL */
    bool init(SDL_Window* window, SDL_Renderer* renderer);
    
    /** @brief Stäng av ImGui */
    void shutdown();
    
    /** @brief Hantera SDL events */
    void handleEvent(const SDL_Event& event);
    
    /** @brief Uppdatera ImGui frame */
    void beginFrame();
    
    /** @brief Rendera ImGui */
    void render();
    
    /** @brief Visa/dölj editor */
    void toggle() { m_visible = !m_visible; }
    bool isVisible() const { return m_visible; }
    void setVisible(bool visible) { m_visible = visible; }
    
    /** @brief Hotkey för att öppna editor (default: F12) */
    void setHotkey(SDL_Scancode key) { m_hotkey = key; }

private:
    DebugEditor() = default;
    ~DebugEditor() = default;
    DebugEditor(const DebugEditor&) = delete;
    DebugEditor& operator=(const DebugEditor&) = delete;
    
    // Panel-rendering
    void renderMainMenu();
    void renderWorldStatePanel();
    void renderRoomsPanel();
    void renderDialogsPanel();
    void renderQuestsPanel();
    void renderInventoryPanel();
    void renderAudioPanel();
    void renderEventsLog();
    
    // Flowchart helpers
    void renderRoomFlowchart();
    void renderDialogTree(const std::string& dialogId);
    void renderQuestTree(const std::string& questId);
    
    bool m_initialized = false;
    bool m_visible = false;
    SDL_Scancode m_hotkey = SDL_SCANCODE_F12;
    
    // Panel visibility
    bool m_showWorldState = true;
    bool m_showRooms = false;
    bool m_showDialogs = false;
    bool m_showQuests = false;
    bool m_showInventory = false;
    bool m_showAudio = false;
    bool m_showEvents = false;
    
    // Selected items
    std::string m_selectedRoom;
    std::string m_selectedDialog;
    std::string m_selectedQuest;
    
    // SDL references
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};
