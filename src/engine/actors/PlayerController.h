/**
 * @file PlayerController.h
 * @brief Player Controller - Handles Player Input
 */
#pragma once

#include "Controller.h"
#include "engine/input/Input.h"

namespace engine {

/**
 * @brief Controller for player input (like APlayerController in UE5)
 * 
 * Handles keyboard/mouse input and sends commands to possessed Pawn.
 */
class PlayerController : public Controller {
public:
    PlayerController();
    virtual ~PlayerController() = default;
    
    /**
     * @brief Handle input and send to pawn
     * @param input Input manager
     */
    void handleInput(const Input& input);
    
    void update(float deltaTime) override;
};

} // namespace engine
