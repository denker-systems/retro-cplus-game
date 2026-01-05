#pragma once

#include <SDL.h>
#include <memory>
#include <string>

class StateManager;
class IState;

class Game {
public:
    Game();
    ~Game();

    bool init(const std::string& title, int width, int height);
    void run();
    void quit();
    
    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);
    
    SDL_Renderer* getRenderer() const { return m_renderer; }

    static constexpr int SCREEN_WIDTH = 640;
    static constexpr int SCREEN_HEIGHT = 400;

private:
    void handleEvents();
    void update(float deltaTime);
    void render();

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    
    std::unique_ptr<StateManager> m_stateManager;

    bool m_running = false;
    Uint32 m_lastFrameTime = 0;
};
