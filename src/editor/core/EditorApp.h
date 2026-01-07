/**
 * @file EditorApp.h
 * @brief Standalone editor application
 */
#pragma once

#include <SDL.h>
#include <string>
#include <memory>

// Forward declarations
class EditorState;

/**
 * @brief Standalone editor application
 * 
 * Manages the editor window, SDL initialization, and main loop.
 * Coordinates with EditorState for the actual editor functionality.
 */
class EditorApp {
public:
    EditorApp();
    ~EditorApp();
    
    /**
     * @brief Initialize SDL, create window and renderer
     * @param title Window title
     * @param width Window width
     * @param height Window height
     * @return true if successful
     */
    bool init(const std::string& title, int width, int height);
    
    /**
     * @brief Run the main editor loop
     */
    void run();
    
    /**
     * @brief Clean up resources and shutdown
     */
    void shutdown();
    
    /**
     * @brief Get SDL renderer for drawing operations
     * @return SDL renderer instance
     */
    SDL_Renderer* getRenderer() const { return m_renderer; }
    
private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    bool m_running = false;
    
    Uint32 m_lastTime = 0;
    
    // Editor state
    std::unique_ptr<EditorState> m_editorState;
    
public:
    // Dummy methods för EditorState-kompatibilitet
    void popState() { m_running = false; }
    SDL_Renderer* getRenderer() { return m_renderer; }
};
