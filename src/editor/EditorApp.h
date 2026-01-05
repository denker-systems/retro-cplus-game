/**
 * @file EditorApp.h
 * @brief Standalone editor application
 */
#pragma once

#include <SDL.h>
#include <string>
#include <memory>

class EditorApp {
public:
    EditorApp();
    ~EditorApp();
    
    bool init(const std::string& title, int width, int height);
    void run();
    void shutdown();
    
    SDL_Renderer* getRenderer() const { return m_renderer; }
    
private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    bool m_running = false;
    
    Uint32 m_lastTime = 0;
};
