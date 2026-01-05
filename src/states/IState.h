#pragma once

#include <SDL.h>

class Game;

class IState {
public:
    virtual ~IState() = default;
    
    virtual void enter() = 0;
    virtual void exit() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(const SDL_Event& event) = 0;
    
    void setGame(Game* game) { m_game = game; }
    Game* getGame() const { return m_game; }

protected:
    Game* m_game = nullptr;
};
