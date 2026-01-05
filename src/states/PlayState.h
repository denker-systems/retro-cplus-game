#pragma once

#include "IState.h"
#include <memory>

class Player;
class Room;
class Input;

class PlayState : public IState {
public:
    PlayState();
    ~PlayState() override;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    std::unique_ptr<Player> m_player;
    std::unique_ptr<Room> m_room;
    std::unique_ptr<Input> m_input;
};
