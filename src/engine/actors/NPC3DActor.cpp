/**
 * @file NPC3DActor.cpp
 * @brief 3D NPC implementation
 */
#include "NPC3DActor.h"
#include "engine/utils/Logger.h"

namespace engine {

NPC3DActor::NPC3DActor(const std::string& name)
    : Character3DActor(name) {
}

void NPC3DActor::interact() {
    if (!m_dialogId.empty()) {
        LOG_INFO("[NPC3DActor] " + getName() + " interaction - dialog: " + m_dialogId);
        // TODO: Trigger dialog system
    } else if (!m_interactionText.empty()) {
        LOG_INFO("[NPC3DActor] " + getName() + " says: " + m_interactionText);
    }
}

void NPC3DActor::update(float deltaTime) {
    Character3DActor::update(deltaTime);
    
    // TODO: AI behavior (patrol, idle animations, etc.)
}

} // namespace engine
