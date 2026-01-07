/**
 * @file TriggerComponent.h
 * @brief Component for handling physics trigger events (onEnter/onExit/onStay)
 * 
 * Works with Collider2DComponent set to trigger mode.
 * Provides callbacks for when other actors enter, exit, or stay in the trigger.
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <functional>
#include <unordered_set>
#include <string>

namespace engine {

class ActorObjectExtended;

/**
 * @brief Handles trigger collision callbacks
 * 
 * Usage:
 *   auto* trigger = actor->addComponent<TriggerComponent>();
 *   trigger->setOnEnter([](ActorObjectExtended* other) {
 *       // Handle enter
 *   });
 *   trigger->setOnExit([](ActorObjectExtended* other) {
 *       // Handle exit
 *   });
 */
class TriggerComponent : public ActorComponent {
public:
    using TriggerCallback = std::function<void(ActorObjectExtended* other)>;
    
    TriggerComponent(const std::string& name = "TriggerComponent");
    virtual ~TriggerComponent() = default;
    
    // ========================================================================
    // ACTORCOMPONENT LIFECYCLE
    // ========================================================================
    
    void initialize() override;
    void shutdown() override;
    void update(float deltaTime) override;
    
    // ========================================================================
    // CALLBACK SETUP
    // ========================================================================
    
    void setOnEnter(TriggerCallback callback) { m_onEnter = callback; }
    void setOnExit(TriggerCallback callback) { m_onExit = callback; }
    void setOnStay(TriggerCallback callback) { m_onStay = callback; }
    
    // ========================================================================
    // TRIGGER EVENTS (called by physics system)
    // ========================================================================
    
    void onTriggerEnter(ActorObjectExtended* other);
    void onTriggerExit(ActorObjectExtended* other);
    void onTriggerStay(ActorObjectExtended* other);
    
    // ========================================================================
    // STATE QUERIES
    // ========================================================================
    
    bool isOverlapping(ActorObjectExtended* actor) const;
    const std::unordered_set<ActorObjectExtended*>& getOverlappingActors() const { return m_overlappingActors; }
    size_t getOverlapCount() const { return m_overlappingActors.size(); }
    
    // ========================================================================
    // FILTER SETTINGS
    // ========================================================================
    
    void setFilterTag(const std::string& tag) { m_filterTag = tag; }
    const std::string& getFilterTag() const { return m_filterTag; }
    bool hasFilterTag() const { return !m_filterTag.empty(); }
    
private:
    TriggerCallback m_onEnter;
    TriggerCallback m_onExit;
    TriggerCallback m_onStay;
    
    std::unordered_set<ActorObjectExtended*> m_overlappingActors;
    std::string m_filterTag;  // Only trigger for actors with this tag
};

} // namespace engine
