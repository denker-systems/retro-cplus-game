/**
 * @file AIController.h
 * @brief AI Controller - Handles AI Logic
 */
#pragma once

#include "Controller.h"

namespace engine {

/**
 * @brief AI behavior types
 */
enum class AIBehaviorType {
    Idle,       ///< Stand still
    Wander,     ///< Wander around
    Follow,     ///< Follow a target
    Patrol,     ///< Patrol waypoints
    Flee        ///< Flee from target
};

/**
 * @brief Controller for AI logic (like AAIController in UE5)
 * 
 * Handles AI decision making and sends commands to possessed Pawn.
 */
class AIController : public Controller {
public:
    AIController();
    virtual ~AIController() = default;
    
    /**
     * @brief Set AI behavior
     * @param type Behavior type
     */
    void setBehavior(AIBehaviorType type) { m_currentBehavior = type; }
    
    /**
     * @brief Get current behavior
     * @return Current behavior type
     */
    AIBehaviorType getBehavior() const { return m_currentBehavior; }
    
    void update(float deltaTime) override;
    
private:
    AIBehaviorType m_currentBehavior = AIBehaviorType::Idle;
    float m_wanderTimer = 0.0f;
    Vec2 m_wanderDirection{0, 0};
    
    void updateIdleBehavior(float deltaTime);
    void updateWanderBehavior(float deltaTime);
    void updateFollowBehavior(float deltaTime);
};

} // namespace engine
