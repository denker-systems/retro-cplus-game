/**
 * @file ActorObject.cpp
 * @brief Implementation of ActorObject
 */
#include "ActorObject.h"

namespace engine {

ActorObject::ActorObject(const std::string& name)
    : Object(name)
{
}

void ActorObject::setPosition(float x, float y) {
    m_position.x = x;
    m_position.y = y;
}

void ActorObject::setPosition(const Vec2& pos) {
    m_position = pos;
}

void ActorObject::setScale(float sx, float sy) {
    m_scale.x = sx;
    m_scale.y = sy;
}

void ActorObject::setScale(const Vec2& scale) {
    m_scale = scale;
}

void ActorObject::update(float deltaTime) {
    // Base implementation does nothing
    // Subclasses override this
}

void ActorObject::render(SDL_Renderer* renderer) {
    // Base implementation does nothing
    // Subclasses with visual representation override this
}

} // namespace engine
