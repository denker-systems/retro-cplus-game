/**
 * @file PlayerStartActor.cpp
 * @brief Implementation of PlayerStartActor
 */
#include "PlayerStartActor.h"

namespace engine {

PlayerStartActor::PlayerStartActor(const std::string& name)
    : ActorObjectExtended(name) {
}

glm::vec3 PlayerStartActor::getSpawnPosition() const {
    // Convert from 2D pixel coordinates to 3D world coordinates
    auto pos2D = getPosition();
    float z = getZ();
    return glm::vec3(pos2D.x / 100.0f, z / 100.0f, pos2D.y / 100.0f);
}

void PlayerStartActor::setSpawnPosition(const glm::vec3& position) {
    // Convert from 3D world coordinates to 2D pixel coordinates
    setPosition(position.x * 100.0f, position.z * 100.0f);
    setZ(position.y * 100.0f);
}

bool PlayerStartActor::isPlayerStart(const ActorObjectExtended* actor) {
    return dynamic_cast<const PlayerStartActor*>(actor) != nullptr;
}

} // namespace engine
