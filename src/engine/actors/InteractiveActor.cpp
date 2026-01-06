/**
 * @file InteractiveActor.cpp
 * @brief Interactive Actor Implementation
 */
#include "InteractiveActor.h"

namespace engine {

InteractiveActor::InteractiveActor(const std::string& name)
    : ActorObject(name)
{
}

void InteractiveActor::interact() {
    if (canInteract() && m_onInteract) {
        m_onInteract();
    }
}

} // namespace engine
