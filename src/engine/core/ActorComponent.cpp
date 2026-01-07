/**
 * @file ActorComponent.cpp
 * @brief Component System Implementation
 */
#include "ActorComponent.h"

namespace engine {

// ============================================================================
// ACTOR COMPONENT
// ============================================================================

ActorComponent::ActorComponent(const std::string& name)
    : Object(name)
    , m_owner(nullptr)
    , m_enabled(true)
{
}

// ============================================================================
// SCENE COMPONENT
// ============================================================================

SceneComponent::SceneComponent(const std::string& name)
    : ActorComponent(name)
    , m_position(0, 0)
    , m_rotation(0.0f)
    , m_scale(1.0f, 1.0f)
    , m_parent(nullptr)
{
}

} // namespace engine
