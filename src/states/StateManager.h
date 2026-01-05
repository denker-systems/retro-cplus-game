#pragma once

#include <stack>
#include <memory>
#include <SDL.h>

class IState;

class StateManager {
public:
    StateManager() = default;
    ~StateManager() = default;
    
    void pushState(std::unique_ptr<IState> state);
    void popState();
    void changeState(std::unique_ptr<IState> state);
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& event);
    
    bool isEmpty() const { return m_states.empty(); }
    IState* getCurrentState() const;

private:
    std::stack<std::unique_ptr<IState>> m_states;
};
