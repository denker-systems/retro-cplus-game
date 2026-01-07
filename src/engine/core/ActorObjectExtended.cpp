/**
 * @file ActorObjectExtended.cpp
 * @brief ActorObject with Component Support Implementation
 */
#include "ActorObjectExtended.h"

namespace engine {

ActorObjectExtended::ActorObjectExtended(const std::string& name)
    : ActorObject(name)
{
}

void ActorObjectExtended::update(float deltaTime) {
    // Call base class update
    ActorObject::update(deltaTime);
    
    // Update all enabled components
    for (auto& comp : m_components) {
        if (comp->isEnabled()) {
            comp->update(deltaTime);
        }
    }
}

void ActorObjectExtended::render(SDL_Renderer* renderer) {
    // Call base class render
    ActorObject::render(renderer);
    
    // Render all enabled components
    for (auto& comp : m_components) {
        if (comp->isEnabled()) {
            comp->render(renderer);
        }
    }
}

} // namespace engine
