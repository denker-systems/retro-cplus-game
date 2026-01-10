/**
 * @file CharacterActor.h
 * @brief DEPRECATED: Legacy 2D character actors
 * 
 * USE INSTEAD: Character3DActor (3D base class)
 * 
 * This file is kept for backward compatibility with legacy game states.
 * All new code should use Character3DActor and its subclasses:
 * - Player3DActor (replaces PlayerActor)
 * - NPC3DActor (replaces NPCActor)
 * 
 * Migration plan: See docs/architecture/2D-TO-3D-MIGRATION.md
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"

namespace engine {

/**
 * @brief DEPRECATED: Legacy 2D character base class
 * 
 * USE INSTEAD: Character3DActor (3D base class)
 * 
 * Features:
 * - Movement speed
 * - Animation support (via AnimationComponent)
 * - Movement (via MovementComponent)
 * - Dialog (via DialogComponent)
 * 
 * @deprecated Use Character3DActor for all new code
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
    
    // Legacy movement support (temporary during migration)
    void move(int dx, int dy, float deltaTime);
    void setTarget(float x, float y);
    float getX() const;
    float getY() const;
    void renderScaled(SDL_Renderer* renderer, float scale);
    
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
 * @brief DEPRECATED: Legacy 2D player actor
 * 
 * USE INSTEAD: Player3DActor (3D player)
 * 
 * @deprecated Use Player3DActor for all new code
 */
class PlayerActor : public CharacterActor {
public:
    PlayerActor();
    virtual ~PlayerActor() = default;
    
    void update(float deltaTime) override;
    
    // Movement methods
    void moveWithInput(int dx, int dy, float deltaTime);
    void setMoveTarget(float x, float y);
    bool isMovingToTarget() const { return m_isMovingToTarget; }
    
    // Inventory methods
    bool addItem(const std::string& itemId, int quantity = 1);
    bool removeItem(const std::string& itemId, int quantity = 1);
    int getItemCount(const std::string& itemId) const;
    bool hasItem(const std::string& itemId) const;
    
private:
    bool m_isMovingToTarget = false;
    Vec2 m_targetPosition{0, 0};
};

/**
 * @brief DEPRECATED: Legacy 2D NPC actor
 * 
 * USE INSTEAD: NPC3DActor (3D NPC)
 * 
 * @deprecated Use NPC3DActor for all new code
 */
class NPCActor : public CharacterActor {
public:
    NPCActor(const std::string& name = "NPC", const std::string& spriteName = "");
    virtual ~NPCActor() = default;
    
    void update(float deltaTime) override;
    
    // Dialog methods
    void setDialogId(const std::string& dialogId);
    const std::string& getDialogId() const;
    bool canTalk() const;
    
    // Interaction methods
    void setInteractionText(const std::string& text);
    const std::string& getInteractionText() const;
    void setInteractionRange(float range);
    float getInteractionRange() const;
    void interact();
    
    // Speed methods
    void setSpeed(float speed) { m_moveSpeed = speed; }
    float getSpeed() const { return m_moveSpeed; }
    
private:
    float m_moveSpeed = 80.0f;
};

} // namespace engine
