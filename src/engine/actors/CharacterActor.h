/**
 * @file CharacterActor.h
 * @brief Base class for all character actors (Player, NPCs)
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"

namespace engine {

/**
 * @brief Base class for characters (Player, NPCs)
 * 
 * Features:
 * - Movement speed
 * - Animation support (via AnimationComponent)
 * - Movement (via MovementComponent)
 * - Dialog (via DialogComponent)
 * 
 * Similar to ACharacter in Unreal Engine.
 */
class CharacterActor : public ActorObjectExtended {
public:
    CharacterActor(const std::string& name);
    virtual ~CharacterActor() = default;
    
    // ========================================================================
    // MOVEMENT
    // ========================================================================
    
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    float getMoveSpeed() const { return m_moveSpeed; }
    
    // ========================================================================
    // CHARACTER STATE
    // ========================================================================
    
    bool isMoving() const { return m_isMoving; }
    
    void setFacingDirection(float angle) { m_facingDirection = angle; }
    float getFacingDirection() const { return m_facingDirection; }
    
    // ========================================================================
    // LIFECYCLE
    // ========================================================================
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
protected:
    float m_moveSpeed = 100.0f;
    float m_facingDirection = 0.0f; // Radians
    bool m_isMoving = false;
};

/**
 * @brief Player character actor
 */
class PlayerActor : public CharacterActor {
public:
    PlayerActor();
    virtual ~PlayerActor() = default;
    
    void update(float deltaTime) override;
};

/**
 * @brief NPC character actor
 */
class NPCActor : public CharacterActor {
public:
    NPCActor(const std::string& name);
    virtual ~NPCActor() = default;
    
    void setDialogId(const std::string& dialogId) { m_dialogId = dialogId; }
    const std::string& getDialogId() const { return m_dialogId; }
    
    void update(float deltaTime) override;
    
protected:
    std::string m_dialogId;
};

} // namespace engine
