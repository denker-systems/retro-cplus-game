/**
 * @file SpriteActor.h
 * @brief Actor with sprite rendering via SpriteComponent
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"

namespace engine {

// Forward declaration
class SpriteComponent;
class AnimationComponent;

/**
 * @brief Actor with sprite rendering
 * 
 * Uses SpriteComponent for rendering instead of direct SDL calls.
 * This is the modern replacement for the legacy Sprite node.
 */
class SpriteActor : public ActorObjectExtended {
public:
    SpriteActor(const std::string& name = "SpriteActor");
    virtual ~SpriteActor() = default;
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    /**
     * @brief Initialize with sprite component
     * @param texturePath Path to texture (will be loaded via SpriteComponent)
     */
    void initializeSprite(const std::string& texturePath);
    
    /**
     * @brief Initialize with animation component
     */
    void initializeAnimation();
    
    // ========================================================================
    // COMPONENT ACCESS
    // ========================================================================
    
    SpriteComponent* getSpriteComponent() { return m_spriteComponent; }
    AnimationComponent* getAnimationComponent() { return m_animationComponent; }
    
protected:
    SpriteComponent* m_spriteComponent = nullptr;
    AnimationComponent* m_animationComponent = nullptr;
};

} // namespace engine
