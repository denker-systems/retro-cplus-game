/**
 * @file InteractionComponent.cpp
 * @brief Interaction Component Implementation
 */
#include "InteractionComponent.h"

namespace engine {

InteractionComponent::InteractionComponent(const std::string& name)
    : ActorComponent(name)
{
}

void InteractionComponent::interact() {
    if (canInteract() && m_onInteract) {
        m_onInteract();
    }
}

} // namespace engine
