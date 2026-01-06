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
 * @brief Player character actor
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
 * @brief NPC character actor
 */
class NPCActor : public CharacterActor {
public:
    NPCActor(const std::string& name = "NPC");
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
