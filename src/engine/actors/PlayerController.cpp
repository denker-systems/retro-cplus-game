/**
 * @file PlayerController.cpp
 * @brief Player Controller Implementation
 */
#include "PlayerController.h"
#include "Pawn.h"

namespace engine {

PlayerController::PlayerController()
    : Controller("PlayerController")
{
}

void PlayerController::handleInput(const Input& input) {
    if (!m_possessedPawn) return;
    
    // WASD movement
    float horizontal = 0.0f;
    float vertical = 0.0f;
    
    if (input.isKeyDown(SDL_SCANCODE_W)) vertical -= 1.0f;
    if (input.isKeyDown(SDL_SCANCODE_S)) vertical += 1.0f;
    if (input.isKeyDown(SDL_SCANCODE_A)) horizontal -= 1.0f;
    if (input.isKeyDown(SDL_SCANCODE_D)) horizontal += 1.0f;
    
    m_possessedPawn->moveForward(vertical);
    m_possessedPawn->moveRight(horizontal);
    
    // E to interact
    if (input.isKeyPressed(SDL_SCANCODE_E)) {
        m_possessedPawn->interact();
    }
}

void PlayerController::update(float deltaTime) {
    // Player controller logic if needed
}

} // namespace engine
