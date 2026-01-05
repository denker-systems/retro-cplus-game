/**
 * @file IState.h
 * @brief Interface för game states (State Pattern)
 */
#pragma once

#include <SDL.h>

class Game;

/**
 * @brief Abstrakt basklass för alla game states
 * 
 * Implementera denna för nya states (Menu, Play, Pause, etc.)
 * Alla states får tillgång till Game via m_game pekaren.
 */
class IState {
public:
    virtual ~IState() = default;
    
    /** @brief Anropas när state blir aktiv */
    virtual void enter() = 0;
    
    /** @brief Anropas när state lämnas */
    virtual void exit() = 0;
    
    /** @brief Uppdatera state logik */
    virtual void update(float deltaTime) = 0;
    
    /** @brief Rita state */
    virtual void render(SDL_Renderer* renderer) = 0;
    
    /** @brief Hantera input events */
    virtual void handleEvent(const SDL_Event& event) = 0;
    
    void setGame(Game* game) { m_game = game; }
    Game* getGame() const { return m_game; }

protected:
    Game* m_game = nullptr;  // Pekare till huvudspelet
};
