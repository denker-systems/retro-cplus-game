/**
 * @file PathfindingComponent.cpp
 * @brief Pathfinding Component Implementation
 */
#include "PathfindingComponent.h"

namespace engine {

PathfindingComponent::PathfindingComponent(const std::string& name)
    : ActorComponent(name)
{
}

void PathfindingComponent::setTarget(Vec2 target) {
    m_target = target;
}

Vec2 PathfindingComponent::getCurrentWaypoint() const {
    if (m_currentWaypoint >= 0 && m_currentWaypoint < m_path.size()) {
        return m_path[m_currentWaypoint];
    }
    return m_target;
}

void PathfindingComponent::clearPath() {
    m_path.clear();
    m_currentWaypoint = 0;
}

void PathfindingComponent::update(float deltaTime) {
    if (!hasPath() || !m_owner) return;
    
    Vec2 currentPos = m_owner->getPosition();
    Vec2 waypoint = getCurrentWaypoint();
    
    float dx = waypoint.x - currentPos.x;
    float dy = waypoint.y - currentPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    // Reached waypoint
    if (distance < 5.0f) {
        m_currentWaypoint++;
        if (m_currentWaypoint >= m_path.size()) {
            clearPath();
        }
    }
}

} // namespace engine
