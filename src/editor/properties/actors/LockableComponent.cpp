/**
 * @file LockableComponent.cpp
 * @brief Component for locking actors implementation
 */
#include "LockableComponent.h"

namespace engine {

LockableComponent::LockableComponent() = default;

LockableComponent::LockableComponent(bool locked)
    : m_locked(locked) {
}

} // namespace engine
