/**
 * @file PlayState.h
 * @brief Gameplay state - huvudsaklig spel-logik
 */
#pragma once

#include "game/states/IState.h"
#include "engine/actors/NPC.h"
#include "engine/world/Scene.h"
#include "engine/input/Input.h"
#include <memory>
#include <string>
#include "engine/actors/CharacterActor.h"

class Input;

/**
 * @brief Gameplay state där spelaren interagerar med världen
 * 
 * Hanterar spelare, scener och input under gameplay.
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
    void onRoomChange(const std::string& roomId);
    void interactWithHotspot(struct Hotspot* hotspot);
    struct Hotspot* getNearbyHotspot(float maxDistance = 60.0f);
    void interactWithNPC(engine::actors::NPC* npc);
    engine::actors::NPC* getNPCAt(int x, int y);
    
    std::unique_ptr<engine::PlayerActor> m_player;
    std::unique_ptr<Input> m_input;
    
    std::string m_hoveredHotspot;    // Namn på hotspot under musen
    struct Hotspot* m_nearbyHotspot = nullptr;  // Närmaste hotspot för E-interaktion
    static constexpr float INTERACT_DISTANCE = 60.0f;
    bool m_initialized = false;  // Undvik att ladda om data vid resume
    bool m_playerPhysicsInitialized = false;  // Physics initialized for player
};
