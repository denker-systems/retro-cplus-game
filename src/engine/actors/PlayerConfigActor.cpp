/**
 * @file PlayerConfigActor.cpp
 * @brief Implementation of PlayerConfigActor
 */
#include "PlayerConfigActor.h"
#include "engine/components/CameraComponent.h"
#include "engine/utils/Logger.h"

namespace engine {

PlayerConfigActor::PlayerConfigActor(const std::string& name)
    : ActorObjectExtended(name) {
    // Add camera component automatically
    m_camera = addComponent<CameraComponent>();
    
    // Set default camera settings
    if (m_camera) {
        m_camera->setZoom(1.0f);
        m_camera->setFollowSmoothing(0.1f);
        m_camera->setViewportSize(640, 400);
    }
    
    LOG_INFO("[PlayerConfigActor] Created: " + name);
}

void PlayerConfigActor::update(float deltaTime) {
    ActorObjectExtended::update(deltaTime);
    
    // Camera component updates automatically via component system
}

} // namespace engine
