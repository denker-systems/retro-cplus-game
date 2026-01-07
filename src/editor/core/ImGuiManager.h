/**
 * @file ImGuiManager.h
 * @brief ImGui integration för editor
 */
#pragma once

#ifdef HAS_IMGUI

#include <SDL.h>

class ImGuiManager {
public:
    static ImGuiManager& instance();
    
    bool init(SDL_Window* window, SDL_Renderer* renderer);
    void shutdown();
    
    void beginFrame();
    void endFrame();
    
    void processEvent(const SDL_Event& event);
    
    bool isInitialized() const { return m_initialized; }
    
private:
    ImGuiManager() = default;
    ~ImGuiManager() = default;
    ImGuiManager(const ImGuiManager&) = delete;
    ImGuiManager& operator=(const ImGuiManager&) = delete;
    
    bool m_initialized = false;
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};

#endif // HAS_IMGUI
