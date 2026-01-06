/**
 * @file PerceptionComponent.cpp
 * @brief Perception Component Implementation
 */
#include "PerceptionComponent.h"
#include "engine/core/CoreRedirects.h"
#include <cmath>

namespace engine {

PerceptionComponent::PerceptionComponent(const std::string& name)
    : ActorComponent(name)
{
}

bool PerceptionComponent::canSee(ActorObject* target) const {
    if (!target || !m_owner) return false;
    
    Vec2 ownerPos = m_owner->getPosition();
    Vec2 targetPos = target->getPosition();
    
    float dx = targetPos.x - ownerPos.x;
    float dy = targetPos.y - ownerPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    return distance <= m_sightRange;
}

bool PerceptionComponent::canHear(Vec2 soundPosition) const {
    if (!m_owner) return false;
    
    Vec2 ownerPos = m_owner->getPosition();
    float dx = soundPosition.x - ownerPos.x;
    float dy = soundPosition.y - ownerPos.y;
    float distance = std::sqrt(dx * dx + dy * dy);
    
    return distance <= m_hearingRange;
}

void PerceptionComponent::update(float deltaTime) {
    // Update visible actors list
    m_visibleActors.clear();
    
    // TODO: Query scene for actors and check visibility
}

} // namespace engine
