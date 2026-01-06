/**
 * @file CharacterActor.cpp
 * @brief Implementation of character actors
 */
#include "CharacterActor.h"

namespace engine {

// ============================================================================
// CharacterActor
// ============================================================================

CharacterActor::CharacterActor(const std::string& name)
    : ActorObjectExtended(name)
{
}

void CharacterActor::update(float deltaTime) {
    // Base update (calls components)
    ActorObjectExtended::update(deltaTime);
}

void CharacterActor::render(SDL_Renderer* renderer) {
    // Base render (calls components)
    ActorObjectExtended::render(renderer);
}

// ============================================================================
// PlayerActor
// ============================================================================

PlayerActor::PlayerActor()
    : CharacterActor("Player")
{
    m_moveSpeed = 120.0f;
}

void PlayerActor::update(float deltaTime) {
    CharacterActor::update(deltaTime);
}

// ============================================================================
// NPCActor
// ============================================================================

NPCActor::NPCActor(const std::string& name)
    : CharacterActor(name)
{
    m_moveSpeed = 80.0f;
}

void NPCActor::update(float deltaTime) {
    CharacterActor::update(deltaTime);
}

} // namespace engine
