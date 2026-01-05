/**
 * @file PlayState.h
 * @brief Gameplay state - huvudsaklig spel-logik
 */
#pragma once

#include "IState.h"
#include <memory>

class PlayerCharacter;
class Room;
class Input;

/**
 * @brief Gameplay state där spelaren interagerar med världen
 * 
 * Hanterar spelare, rum och input under gameplay.
 * ESC återgår till MenuState.
 */
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
    std::unique_ptr<PlayerCharacter> m_player;
    std::unique_ptr<Room> m_room;
    std::unique_ptr<Input> m_input;
};
