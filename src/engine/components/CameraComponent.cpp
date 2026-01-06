/**
 * @file CameraComponent.cpp
 * @brief Camera Component Implementation
 */
#include "CameraComponent.h"
#include "engine/core/CoreRedirects.h"
#include <algorithm>

namespace engine {

CameraComponent::CameraComponent(const std::string& name)
    : SceneComponent(name)
{
}

void CameraComponent::update(float deltaTime) {
    if (!m_followTarget || !m_owner) return;
    
    Vec2 targetPos = m_followTarget->getPosition();
    Vec2 currentPos = m_owner->getPosition();
    
    // Smooth follow
    Vec2 diff = targetPos - currentPos;
    Vec2 newPos = currentPos + diff * m_followSmoothing;
    
    // Apply bounds
    if (m_hasBounds) {
        float halfViewW = (m_viewportWidth / m_zoom) * 0.5f;
        float halfViewH = (m_viewportHeight / m_zoom) * 0.5f;
        
        newPos.x = std::clamp(newPos.x, m_minX + halfViewW, m_maxX - halfViewW);
        newPos.y = std::clamp(newPos.y, m_minY + halfViewH, m_maxY - halfViewH);
    }
    
    m_owner->setPosition(newPos);
}

Vec2 CameraComponent::worldToScreen(Vec2 worldPos) const {
    if (!m_owner) return worldPos;
    
    Vec2 camPos = m_owner->getPosition();
    Vec2 offset = (worldPos - camPos) * m_zoom;
    
    return Vec2(
        offset.x + m_viewportWidth * 0.5f,
        offset.y + m_viewportHeight * 0.5f
    );
}

Vec2 CameraComponent::screenToWorld(Vec2 screenPos) const {
    if (!m_owner) return screenPos;
    
    Vec2 camPos = m_owner->getPosition();
    Vec2 centered = Vec2(
        screenPos.x - m_viewportWidth * 0.5f,
        screenPos.y - m_viewportHeight * 0.5f
    );
    
    return camPos + (centered / m_zoom);
}

} // namespace engine
