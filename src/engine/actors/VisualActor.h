/**
 * @file VisualActor.h
 * @brief Base class for actors with visual representation
 * 
 * NOTE: This actor is 3D-compatible (uses ActorObjectExtended base)
 * Works with both 2D and 3D coordinates.
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"

namespace engine {

/**
 * @brief Base class for actors with visual representation
 * 
 * This is the base for Sprite, AnimatedSprite, Label, etc.
 * All visual objects inherit from this.
 */
class VisualActor : public ActorObjectExtended {
public:
    VisualActor(const std::string& name);
    virtual ~VisualActor() = default;
    
    void setOpacity(float opacity) { m_opacity = opacity; }
    float getOpacity() const { return m_opacity; }
    
    void setZIndex(int z) { m_zIndex = z; }
    int getZIndex() const { return m_zIndex; }
    
protected:
    float m_opacity = 1.0f;
    int m_zIndex = 0;
};

} // namespace engine
