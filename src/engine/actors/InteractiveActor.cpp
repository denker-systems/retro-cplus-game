/**
 * @file InteractiveActor.cpp
 * @brief Interactive Actor Implementation
 */
#include "InteractiveActor.h"

namespace engine {

InteractiveActor::InteractiveActor(const std::string& name)
    : ActorObjectExtended(name)
{
}

void InteractiveActor::interact() {
    if (canInteract() && m_onInteract) {
        m_onInteract();
    }
}

} // namespace engine
