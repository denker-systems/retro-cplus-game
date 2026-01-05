/**
 * @file Game.h
 * @brief Huvudspelklass - initiering, spelloop och state-hantering
 */
#pragma once

#include <SDL.h>
#include <memory>
#include <string>

class StateManager;
class IState;

/**
 * @brief Central spelklass som koordinerar alla system
 * 
 * Hanterar SDL-initiering, spelloop och state management.
 * Alla game states får tillgång till Game via pekare.
 */
class Game {
public:
    Game();
    ~Game();

    /** @brief Initiera SDL, skapa fönster och renderer */
    bool init(const std::string& title, int width, int height);
    
    /** @brief Starta spelloop (blockerar tills spelet avslutas) */
    void run();
    
    /** @brief Frigör alla resurser och stäng SDL */
    void quit();
    
    /** @brief Lägg till state ovanpå nuvarande (för overlay) */
    void pushState(std::unique_ptr<IState> state);
    
    /** @brief Ta bort översta staten */
    void popState();
    
    /** @brief Ersätt alla states med ny */
    void changeState(std::unique_ptr<IState> state);
    
    SDL_Renderer* getRenderer() const { return m_renderer; }

    static constexpr int SCREEN_WIDTH = 640;
    static constexpr int SCREEN_HEIGHT = 400;

private:
    void handleEvents();           // Hantera SDL events
    void update(float deltaTime);  // Uppdatera aktiv state
    void render();                 // Rita aktiv state

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    std::unique_ptr<StateManager> m_stateManager;
    bool m_running = false;
    Uint32 m_lastFrameTime = 0;  // För delta time beräkning
};
