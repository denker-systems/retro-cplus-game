/**
 * @file NPC.cpp
 * @brief Implementation av NPC med Actor system
 */
#include "NPC.h"
#include "engine/components/SpriteComponent.h"

namespace engine {
namespace actors {

NPC::NPC(float x, float y, const std::string& name) {
    // Create the underlying actor
    m_actor = std::make_unique<engine::NPCActor>(name);
    m_actor->setPosition(x, y);
}

void NPC::update(float deltaTime) {
    if (m_actor) {
        m_actor->update(deltaTime);
    }
}

void NPC::render(SDL_Renderer* renderer) {
    if (m_actor) {
        m_actor->render(renderer);
    }
}

void NPC::setPosition(float x, float y) {
    if (m_actor) {
        m_actor->setPosition(x, y);
    }
}

void NPC::getPosition(float& x, float& y) const {
    if (m_actor) {
        engine::Vec2 pos = m_actor->getPosition();
        x = pos.x;
        y = pos.y;
    } else {
        x = y = 0.0f;
    }
}

const std::string& NPC::getName() const {
    static std::string empty = "";
    if (m_actor) {
        return m_actor->getName();
    }
    return empty;
}

void NPC::interact() {
    if (m_actor) {
        m_actor->interact();
    }
}

void NPC::setSpeed(float speed) {
    if (m_actor) {
        m_actor->setSpeed(speed);
    }
}

float NPC::getSpeed() const {
    if (m_actor) {
        return m_actor->getSpeed();
    }
    return 0.0f;
}

float NPC::getX() const {
    if (m_actor) {
        return m_actor->getPosition().x;
    }
    return 0.0f;
}

float NPC::getY() const {
    if (m_actor) {
        return m_actor->getPosition().y;
    }
    return 0.0f;
}

} // namespace actors
} // namespace engine
