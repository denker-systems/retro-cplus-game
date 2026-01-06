/**
 * @file InteractionComponent.h
 * @brief Interaction Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <string>
#include <functional>

namespace engine {

/**
 * @brief Component for interaction functionality
 */
class InteractionComponent : public ActorComponent {
public:
    using InteractionCallback = std::function<void()>;
    
    InteractionComponent(const std::string& name = "InteractionComponent");
    virtual ~InteractionComponent() = default;
    
    void setInteractionText(const std::string& text) { m_interactionText = text; }
    const std::string& getInteractionText() const { return m_interactionText; }
    
    void setInteractionRange(float range) { m_interactionRange = range; }
    float getInteractionRange() const { return m_interactionRange; }
    
    void setOnInteract(InteractionCallback callback) { m_onInteract = callback; }
    
    void interact();
    bool canInteract() const { return m_enabled; }
    
private:
    std::string m_interactionText;
    float m_interactionRange = 50.0f;
    InteractionCallback m_onInteract;
};

} // namespace engine
