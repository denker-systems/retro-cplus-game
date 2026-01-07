/**
 * @file StateManager.h
 * @brief Hanterar game states med en stack
 */
#pragma once

#include <stack>
#include <memory>
#include <SDL.h>

class IState;

/**
 * @brief Hanterar game states med push/pop/change
 * 
 * Använder en stack för att tillåta states ovanpå varandra
 * (t.ex. PauseState ovanpå PlayState).
 * State-ändringar är deferred för att undvika problem under handleEvent.
 */
class StateManager {
public:
    StateManager() = default;
    ~StateManager() = default;
    
    /** @brief Lägg till state ovanpå nuvarande (för overlays) */
    void pushState(std::unique_ptr<IState> state);
    
    /** @brief Ta bort översta staten och återgå till föregående */
    void popState();
    
    /** @brief Ersätt alla states med ny (för state-byte) */
    void changeState(std::unique_ptr<IState> state);
    
    /** @brief Processa pending state-ändringar (anropas av Game) */
    void processPendingChanges();
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& event);
    
    bool isEmpty() const { return m_states.empty() && !m_pendingState; }
    IState* getCurrentState() const;

private:
    void doChangeState(std::unique_ptr<IState> state);
    
    std::stack<std::unique_ptr<IState>> m_states;
    std::unique_ptr<IState> m_pendingState;
    bool m_pendingChange = false;
    bool m_pendingPop = false;
};
