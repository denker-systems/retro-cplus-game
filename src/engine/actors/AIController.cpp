/**
 * @file AIController.cpp
 * @brief AI Controller Implementation
 */
#include "AIController.h"
#include "Pawn.h"
#include <cmath>
#include <cstdlib>

namespace engine {

AIController::AIController()
    : Controller("AIController")
    , m_currentBehavior(AIBehaviorType::Idle)
    , m_wanderTimer(0.0f)
{
}

void AIController::update(float deltaTime) {
    if (!m_possessedPawn) return;
    
    switch (m_currentBehavior) {
        case AIBehaviorType::Idle:
            updateIdleBehavior(deltaTime);
            break;
            
        case AIBehaviorType::Wander:
            updateWanderBehavior(deltaTime);
            break;
            
        case AIBehaviorType::Follow:
            updateFollowBehavior(deltaTime);
            break;
            
        default:
            break;
    }
}

void AIController::updateIdleBehavior(float deltaTime) {
    // Do nothing - just stand still
}

void AIController::updateWanderBehavior(float deltaTime) {
    m_wanderTimer -= deltaTime;
    
    if (m_wanderTimer <= 0.0f) {
        // Pick new random direction every 2-4 seconds
        m_wanderTimer = 2.0f + (rand() % 200) / 100.0f;
        
        float angle = (rand() % 360) * 3.14159f / 180.0f;
        m_wanderDirection = Vec2(std::cos(angle), std::sin(angle));
    }
    
    // Move in current wander direction
    m_possessedPawn->moveForward(m_wanderDirection.y * 0.3f);
    m_possessedPawn->moveRight(m_wanderDirection.x * 0.3f);
}

void AIController::updateFollowBehavior(float deltaTime) {
    // TODO: Implement follow behavior when we have target system
}

} // namespace engine
