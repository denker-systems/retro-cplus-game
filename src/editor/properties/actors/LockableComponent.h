/**
 * @file LockableComponent.h
 * @brief Component for locking actors to prevent editing
 */
#pragma once

#include "engine/core/ActorComponent.h"

namespace engine {

/**
 * @brief Component that makes an actor lockable in the editor
 */
class LockableComponent : public ActorComponent {
public:
    LockableComponent();
    explicit LockableComponent(bool locked);
    
    // Lock/unlock functionality
    void setLocked(bool locked) { m_locked = locked; }
    bool isLocked() const { return m_locked; }
    
    // Toggle lock state
    void toggleLock() { m_locked = !m_locked; }
    
    // Component interface
    void update(float deltaTime) override {}
    void render(SDL_Renderer* renderer) override {}
    
private:
    bool m_locked = false;
};

} // namespace engine
