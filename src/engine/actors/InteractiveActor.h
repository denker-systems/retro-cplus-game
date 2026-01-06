/**
 * @file InteractiveActor.h
 * @brief Base class for interactive actors
 */
#pragma once

#include "engine/core/CoreRedirects.h"
#include <string>
#include <functional>

namespace engine {

/**
 * @brief Base class for interactive objects
 * 
 * This is the base for InteractiveArea, WalkArea, Marker, etc.
 * All interactive objects inherit from this.
 */
class InteractiveActor : public ActorObject {
public:
    using InteractionCallback = std::function<void()>;
    
    InteractiveActor(const std::string& name);
    virtual ~InteractiveActor() = default;
    
    void setInteractionText(const std::string& text) { m_interactionText = text; }
    const std::string& getInteractionText() const { return m_interactionText; }
    
    void setOnInteract(InteractionCallback callback) { m_onInteract = callback; }
    
    virtual void interact();
    virtual bool canInteract() const { return true; }
    
protected:
    std::string m_interactionText;
    InteractionCallback m_onInteract;
};

} // namespace engine
