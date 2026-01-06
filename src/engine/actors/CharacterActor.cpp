/**
 * @file CharacterActor.cpp
 * @brief Implementation of character actors
 */
#include "CharacterActor.h"
#include "engine/core/ActorObject.h"
#include "engine/components/SpriteComponent.h"
#include "engine/components/MovementComponent.h"
#include "engine/components/DialogComponent.h"
#include "engine/components/InventoryComponent.h"
#include "engine/components/InteractionComponent.h"
#include <SDL.h>

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
// Legacy Movement Support (temporary during migration)
// ============================================================================

void CharacterActor::move(int dx, int dy, float deltaTime) {
    // Simple legacy movement - will be replaced by MovementComponent
    Vec2 pos = getPosition();
    pos.x += dx * m_moveSpeed * deltaTime;
    pos.y += dy * m_moveSpeed * deltaTime;
    setPosition(pos.x, pos.y);
}

void CharacterActor::setTarget(float x, float y) {
    // Legacy setTarget - will be replaced by MovementComponent
    setPosition(x, y);
}

float CharacterActor::getX() const {
    return getPosition().x;
}

float CharacterActor::getY() const {
    return getPosition().y;
}

void CharacterActor::renderScaled(SDL_Renderer* renderer, float scale) {
    // Legacy renderScaled - will be removed when using proper components
    Vec2 pos = getPosition();
    SDL_Rect rect = {
        static_cast<int>(pos.x * scale),
        static_cast<int>(pos.y * scale),
        static_cast<int>(32 * scale),  // Assuming 32x32 character
        static_cast<int>(48 * scale)
    };
    SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);  // Magenta for placeholder
    SDL_RenderFillRect(renderer, &rect);
}

// ============================================================================
// PlayerActor
// ============================================================================

PlayerActor::PlayerActor()
    : CharacterActor("Player")
{
    m_moveSpeed = 120.0f;
    
    // Add MovementComponent
    auto movement = addComponent<MovementComponent>();
    movement->setMaxSpeed(m_moveSpeed);
    
    // Add SpriteComponent
    auto sprite = addComponent<engine::SpriteComponent>();
    sprite->loadTextureCached("assets/sprites/player.png");
    
    // Add InventoryComponent
    auto inventory = addComponent<engine::InventoryComponent>();
    inventory->setMaxSlots(20); // Standard inventory storlek
}

void PlayerActor::update(float deltaTime) {
    CharacterActor::update(deltaTime);
}

void PlayerActor::moveWithInput(int dx, int dy, float deltaTime) {
    auto movement = getComponent<MovementComponent>();
    if (movement) {
        // Apply input as force
        Vec2 force(dx * m_moveSpeed * 10, dy * m_moveSpeed * 10);
        movement->addForce(force);
    }
}

void PlayerActor::setMoveTarget(float x, float y) {
    m_targetPosition = Vec2(x, y);
    m_isMovingToTarget = true;
    
    // Simple move to target (will be improved with pathfinding)
    auto movement = getComponent<MovementComponent>();
    if (movement) {
        Vec2 currentPos = getPosition();
        Vec2 direction = m_targetPosition - currentPos;
        
        // Check if we're close enough
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance < 5.0f) {
            m_isMovingToTarget = false;
            movement->stop();
        } else {
            // Normalize and apply force
            direction.x /= distance;
            direction.y /= distance;
            movement->addForce(direction * m_moveSpeed * 10);
        }
    }
}

// ============================================================================
// NPCActor
// ============================================================================

NPCActor::NPCActor(const std::string& name)
    : CharacterActor(name)
{
    m_moveSpeed = 80.0f;
    
    // Add SpriteComponent
    auto sprite = addComponent<engine::SpriteComponent>();
    // TODO: Load NPC sprite based on name/type
    // sprite->loadTextureCached("assets/sprites/npc.png");
    
    // Add DialogComponent
    auto dialog = addComponent<engine::DialogComponent>();
    // TODO: Set dialog ID from data
    
    // Add InteractionComponent
    auto interaction = addComponent<engine::InteractionComponent>();
    interaction->setInteractionText("Prata med " + name);
    interaction->setInteractionRange(60.0f);
    // TODO: Set interaction callback for dialog
}

void NPCActor::update(float deltaTime) {
    CharacterActor::update(deltaTime);
}

void NPCActor::setDialogId(const std::string& dialogId) {
    auto dialog = getComponent<engine::DialogComponent>();
    if (dialog) {
        dialog->setDialogId(dialogId);
    }
}

const std::string& NPCActor::getDialogId() const {
    auto dialog = const_cast<NPCActor*>(this)->getComponent<engine::DialogComponent>();
    if (dialog) {
        return dialog->getDialogId();
    }
    static std::string empty;
    return empty;
}

bool NPCActor::canTalk() const {
    auto dialog = const_cast<NPCActor*>(this)->getComponent<engine::DialogComponent>();
    if (dialog) {
        return dialog->canTalk();
    }
    return false;
}

// ============================================================================
// PLAYER ACTOR INVENTORY METHODS
// ============================================================================

bool PlayerActor::addItem(const std::string& itemId, int quantity) {
    auto inventory = getComponent<engine::InventoryComponent>();
    if (inventory) {
        return inventory->addItem(itemId, quantity);
    }
    return false;
}

bool PlayerActor::removeItem(const std::string& itemId, int quantity) {
    auto inventory = getComponent<engine::InventoryComponent>();
    if (inventory) {
        return inventory->removeItem(itemId, quantity);
    }
    return false;
}

int PlayerActor::getItemCount(const std::string& itemId) const {
    auto inventory = const_cast<PlayerActor*>(this)->getComponent<engine::InventoryComponent>();
    if (inventory) {
        return inventory->getItemCount(itemId);
    }
    return 0;
}

bool PlayerActor::hasItem(const std::string& itemId) const {
    return getItemCount(itemId) > 0;
}

// ============================================================================
// NPC ACTOR INTERACTION METHODS
// ============================================================================

void NPCActor::setInteractionText(const std::string& text) {
    auto interaction = getComponent<engine::InteractionComponent>();
    if (interaction) {
        interaction->setInteractionText(text);
    }
}

const std::string& NPCActor::getInteractionText() const {
    auto interaction = const_cast<NPCActor*>(this)->getComponent<engine::InteractionComponent>();
    if (interaction) {
        return interaction->getInteractionText();
    }
    static std::string empty;
    return empty;
}

void NPCActor::setInteractionRange(float range) {
    auto interaction = getComponent<engine::InteractionComponent>();
    if (interaction) {
        interaction->setInteractionRange(range);
    }
}

float NPCActor::getInteractionRange() const {
    auto interaction = const_cast<NPCActor*>(this)->getComponent<engine::InteractionComponent>();
    if (interaction) {
        return interaction->getInteractionRange();
    }
    return 0.0f;
}

void NPCActor::interact() {
    auto interaction = getComponent<engine::InteractionComponent>();
    if (interaction) {
        interaction->interact();
    }
}

} // namespace engine
