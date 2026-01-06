/**
 * @file DebugEditor.cpp
 * @brief Implementation av in-game debug editor (stub tills ImGui SDL2 backend läggs till)
 * 
 * NOTE: Full ImGui-integration kräver manuell nedladdning av imgui_impl_sdl2.h/cpp
 * från https://github.com/ocornut/imgui/tree/master/backends
 * 
 * För nu: minimal stub som kan utökas senare
 */
#include "DebugEditor.h"
#include "engine/systems/WorldState.h"
#include "engine/systems/SceneManager.h"
#include "engine/systems/DialogSystem.h"
#include "engine/systems/QuestSystem.h"
#include "engine/systems/InventorySystem.h"
#include "engine/audio/AudioManager.h"
#include "engine/graphics/FontManager.h"
#include "engine/utils/Logger.h"

DebugEditor& DebugEditor::instance() {
    static DebugEditor inst;
    return inst;
}

bool DebugEditor::init(SDL_Window* window, SDL_Renderer* renderer) {
    m_window = window;
    m_renderer = renderer;
    m_initialized = true;
    LOG_INFO("DebugEditor initialized (simple mode - press F12 to toggle)");
    return true;
}

void DebugEditor::shutdown() {
    m_initialized = false;
    LOG_INFO("DebugEditor shutdown");
}

void DebugEditor::handleEvent(const SDL_Event& event) {
    if (!m_initialized) return;
    
    // Toggle editor with hotkey
    if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == m_hotkey) {
        toggle();
        LOG_INFO(m_visible ? "Debug editor opened" : "Debug editor closed");
    }
    
    // Panel hotkeys when visible
    if (m_visible && event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_F1: m_showWorldState = !m_showWorldState; break;
            case SDL_SCANCODE_F2: m_showRooms = !m_showRooms; break;
            case SDL_SCANCODE_F3: m_showDialogs = !m_showDialogs; break;
            case SDL_SCANCODE_F4: m_showQuests = !m_showQuests; break;
            case SDL_SCANCODE_F5: m_showInventory = !m_showInventory; break;
            case SDL_SCANCODE_F6: m_showAudio = !m_showAudio; break;
            default: break;
        }
    }
}

void DebugEditor::beginFrame() {
    // No-op in simple mode
}

void DebugEditor::render() {
    if (!m_initialized || !m_visible || !m_renderer) return;
    
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, 640, 400};
    SDL_RenderFillRect(m_renderer, &overlay);
    
    int y = 10;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    SDL_Color green = {100, 255, 100, 255};
    
    // Title
    FontManager::instance().renderText(m_renderer, "default", 
        "=== DEBUG EDITOR (F12 to close) ===", 10, y, yellow);
    y += 20;
    
    // Menu
    FontManager::instance().renderText(m_renderer, "default",
        "F1:WorldState F2:Rooms F3:Dialogs F4:Quests F5:Inventory F6:Audio", 10, y, white);
    y += 25;
    
    // WorldState panel
    if (m_showWorldState) {
        renderWorldStatePanel();
    }
    
    // Rooms panel  
    if (m_showRooms) {
        renderRoomsPanel();
    }
    
    // Dialogs panel
    if (m_showDialogs) {
        renderDialogsPanel();
    }
    
    // Quests panel
    if (m_showQuests) {
        renderQuestsPanel();
    }
    
    // Inventory panel
    if (m_showInventory) {
        renderInventoryPanel();
    }
    
    // Audio panel
    if (m_showAudio) {
        renderAudioPanel();
    }
}

void DebugEditor::renderMainMenu() {
    // Handled in render()
}

void DebugEditor::renderWorldStatePanel() {
    int x = 10, y = 60;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color red = {255, 100, 100, 255};
    
    FontManager::instance().renderText(m_renderer, "default", "[WorldState]", x, y, green);
    y += 15;
    
    auto& world = WorldState::instance();
    auto flags = world.getAllFlags();
    auto counters = world.getAllCounters();
    
    FontManager::instance().renderText(m_renderer, "default", 
        "Flags: " + std::to_string(flags.size()), x, y, white);
    y += 12;
    
    for (auto& [key, value] : flags) {
        std::string text = "  " + key + ": " + (value ? "true" : "false");
        FontManager::instance().renderText(m_renderer, "default", text, x, y, 
            value ? green : red);
        y += 12;
        if (y > 180) break; // Limit display
    }
    
    y += 5;
    FontManager::instance().renderText(m_renderer, "default",
        "Counters: " + std::to_string(counters.size()), x, y, white);
    y += 12;
    
    for (auto& [key, value] : counters) {
        std::string text = "  " + key + ": " + std::to_string(value);
        FontManager::instance().renderText(m_renderer, "default", text, x, y, white);
        y += 12;
        if (y > 250) break;
    }
}

void DebugEditor::renderRoomsPanel() {
    int x = 220, y = 60;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    
    FontManager::instance().renderText(m_renderer, "default", "[Rooms]", x, y, cyan);
    y += 15;
    
    auto* scene = SceneManager::instance().getCurrentScene();
    if (scene) {
        FontManager::instance().renderText(m_renderer, "default",
            "Current: " + scene->getId(), x, y, white);
        y += 12;
        
        auto& hotspots = scene->getHotspots();
        FontManager::instance().renderText(m_renderer, "default",
            "Hotspots: " + std::to_string(hotspots.size()), x, y, white);
        y += 12;
        
        for (auto& hs : hotspots) {
            SDL_Color col = hs.active ? white : SDL_Color{100, 100, 100, 255};
            FontManager::instance().renderText(m_renderer, "default",
                "  " + hs.name + " [" + hs.id + "]", x, y, col);
            y += 12;
            if (y > 250) break;
        }
    } else {
        FontManager::instance().renderText(m_renderer, "default", "No room", x, y, white);
    }
}

void DebugEditor::renderDialogsPanel() {
    int x = 420, y = 60;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color magenta = {255, 100, 255, 255};
    
    FontManager::instance().renderText(m_renderer, "default", "[Dialogs]", x, y, magenta);
    y += 15;
    
    if (DialogSystem::instance().isActive()) {
        FontManager::instance().renderText(m_renderer, "default",
            "Speaker: " + DialogSystem::instance().getCurrentSpeaker(), x, y, white);
        y += 12;
        
        std::string text = DialogSystem::instance().getCurrentText();
        if (text.length() > 30) text = text.substr(0, 30) + "...";
        FontManager::instance().renderText(m_renderer, "default", text, x, y, white);
    } else {
        FontManager::instance().renderText(m_renderer, "default", "No active dialog", x, y, white);
    }
}

void DebugEditor::renderQuestsPanel() {
    int x = 10, y = 270;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color orange = {255, 180, 100, 255};
    
    FontManager::instance().renderText(m_renderer, "default", "[Quests]", x, y, orange);
    y += 15;
    
    auto& quests = QuestSystem::instance().getActiveQuests();
    FontManager::instance().renderText(m_renderer, "default",
        "Active: " + std::to_string(quests.size()), x, y, white);
    y += 12;
    
    for (auto& q : quests) {
        FontManager::instance().renderText(m_renderer, "default", "  " + q.title, x, y, white);
        y += 12;
        if (y > 380) break;
    }
}

void DebugEditor::renderInventoryPanel() {
    int x = 220, y = 270;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color blue = {100, 150, 255, 255};
    
    FontManager::instance().renderText(m_renderer, "default", "[Inventory]", x, y, blue);
    y += 15;
    
    auto items = InventorySystem::instance().getItems();
    FontManager::instance().renderText(m_renderer, "default",
        "Items: " + std::to_string(items.size()), x, y, white);
    y += 12;
    
    for (auto& item : items) {
        FontManager::instance().renderText(m_renderer, "default", "  " + item.name, x, y, white);
        y += 12;
        if (y > 380) break;
    }
}

void DebugEditor::renderAudioPanel() {
    int x = 420, y = 270;
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color purple = {200, 100, 255, 255};
    
    FontManager::instance().renderText(m_renderer, "default", "[Audio]", x, y, purple);
    y += 15;
    
    auto& audio = AudioManager::instance();
    
    std::string musicVol = "Music: " + std::to_string(static_cast<int>(audio.getMusicVolume() * 100)) + "%";
    FontManager::instance().renderText(m_renderer, "default", musicVol, x, y, white);
    y += 12;
    
    std::string sfxVol = "SFX: " + std::to_string(static_cast<int>(audio.getSFXVolume() * 100)) + "%";
    FontManager::instance().renderText(m_renderer, "default", sfxVol, x, y, white);
    y += 12;
    
    std::string state = "State: " + std::to_string(static_cast<int>(audio.getMusicState()));
    FontManager::instance().renderText(m_renderer, "default", state, x, y, white);
}

void DebugEditor::renderEventsLog() {
    // TODO: Implement event log
}

void DebugEditor::renderRoomFlowchart() {
    // TODO: Implement flowchart
}

void DebugEditor::renderDialogTree(const std::string& dialogId) {
    (void)dialogId;
}

void DebugEditor::renderQuestTree(const std::string& questId) {
    (void)questId;
}
