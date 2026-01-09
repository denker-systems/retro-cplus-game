/**
 * @file LightActor.cpp
 * @brief 3D Light Actor implementation
 */

#include "LightActor.h"

namespace engine {

LightActor::LightActor(const std::string& name, LightType type)
    : ActorObjectExtended(name)
    , m_lightType(type)
{
}

void LightActor::update(float deltaTime) {
    ActorObjectExtended::update(deltaTime);
}

void LightActor::render(SDL_Renderer* renderer) {
    ActorObjectExtended::render(renderer);
}

void LightActor::setPosition3D(const glm::vec3& pos) {
    m_position3D = pos;
    setPosition(pos.x * 100.0f, pos.z * 100.0f);
    setZ(pos.y * 100.0f);
}

} // namespace engine
