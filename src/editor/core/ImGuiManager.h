/**
 * @file ImGuiManager.h
 * @brief ImGui integration för editor
 */
#pragma once

#ifdef HAS_IMGUI

#include <SDL.h>

/**
 * @brief ImGui integration manager for the editor
 * 
 * Handles ImGui initialization, frame management, and SDL event processing.
 * Provides a singleton interface for ImGui operations throughout the editor.
 */
class ImGuiManager {
public:
    /**
     * @brief Get singleton instance
     * @return Reference to ImGuiManager instance
     */
    static ImGuiManager& instance();
    
    /**
     * @brief Initialize ImGui with SDL window and renderer
     * @param window SDL window handle
     * @param renderer SDL renderer handle
     * @return true if successful
     */
    bool init(SDL_Window* window, SDL_Renderer* renderer);
    
    /**
     * @brief Shutdown ImGui and clean up resources
     */
    void shutdown();
    
    /**
     * @brief Start a new ImGui frame
     * Call this before rendering any ImGui widgets
     */
    void beginFrame();
    
    /**
     * @brief End the current ImGui frame and render
     * Call this after all ImGui widgets have been rendered
     */
    void endFrame();
    
    /**
     * @brief Process SDL events and forward them to ImGui
     * @param event SDL event to process
     */
    void processEvent(const SDL_Event& event);
    
    /**
     * @brief Check if ImGui is initialized
     * @return true if ImGui is ready for use
     */
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
