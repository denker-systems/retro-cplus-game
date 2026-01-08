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
    SDL_Window* getWindow() const { return m_window; }
    
    /** @brief Hämta renderingsskala */
    float getScale() const { return m_scale; }
    
    /** @brief Hämta viewport offset */
    int getViewportX() const { return m_viewport.x; }
    int getViewportY() const { return m_viewport.y; }
    
    /** @brief Konvertera skärmkoordinater till spelkoordinater */
    void screenToGame(int screenX, int screenY, int& gameX, int& gameY) const;
    
    /** @brief Hämta skärmupplösning */
    int getScreenWidth() const;
    int getScreenHeight() const;

    // Bas-spelupplösning (används för bakåtkompatibilitet med spel-content)
    static constexpr int GAME_WIDTH = 640;
    static constexpr int GAME_HEIGHT = 400;

private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    void calculateViewport();

    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    std::unique_ptr<StateManager> m_stateManager;
    bool m_running = false;
    Uint32 m_lastFrameTime = 0;
    
    // Viewport för letterboxing
    SDL_Rect m_viewport = {0, 0, GAME_WIDTH, GAME_HEIGHT};
    float m_scale = 1.0f;
    bool m_useOpenGL = false;  // True when using OpenGL for rendering
};
