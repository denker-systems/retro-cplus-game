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
    
    /**
     * @brief Initialize trigger component - setup callback tracking
     */
    void initialize() override;
    
    /**
     * @brief Cleanup trigger component - clear callbacks and tracking
     */
    void shutdown() override;
    
    /**
     * @brief Update trigger state - call onStay for overlapping actors
     */
    void update(float deltaTime) override;
    
    // ========================================================================
    // CALLBACK SETUP
    // ========================================================================
    
    /**
     * @brief Set callback for when an actor enters the trigger
     * @param callback Function to call on enter
     */
    void setOnEnter(TriggerCallback callback) { m_onEnter = callback; }
    
    /**
     * @brief Set callback for when an actor exits the trigger
     * @param callback Function to call on exit
     */
    void setOnExit(TriggerCallback callback) { m_onExit = callback; }
    
    /**
     * @brief Set callback for when an actor stays in the trigger
     * @param callback Function to call continuously while overlapping
     */
    void setOnStay(TriggerCallback callback) { m_onStay = callback; }
    
    // ========================================================================
    // TRIGGER EVENTS (called by physics system)
    // ========================================================================
    
    /**
     * @brief Called when an actor enters the trigger area
     * @param other The actor that entered
     */
    void onTriggerEnter(ActorObjectExtended* other);
    
    /**
     * @brief Called when an actor exits the trigger area
     * @param other The actor that exited
     */
    void onTriggerExit(ActorObjectExtended* other);
    
    /**
     * @brief Called when an actor stays in the trigger area
     * @param other The actor that is overlapping
     */
    void onTriggerStay(ActorObjectExtended* other);
    
    // ========================================================================
    // STATE QUERIES
    // ========================================================================
    
    /**
     * @brief Check if an actor is currently overlapping this trigger
     * @param actor The actor to check
     * @return true if overlapping
     */
    bool isOverlapping(ActorObjectExtended* actor) const;
    
    /**
     * @brief Get all currently overlapping actors
     * @return Set of overlapping actors
     */
    const std::unordered_set<ActorObjectExtended*>& getOverlappingActors() const { return m_overlappingActors; }
    
    /**
     * @brief Get the number of overlapping actors
     * @return Count of overlapping actors
     */
    size_t getOverlapCount() const { return m_overlappingActors.size(); }
    
    // ========================================================================
    // FILTER SETTINGS
    // ========================================================================
    
    /**
     * @brief Set tag filter - only trigger for actors with this tag
     * @param tag The tag to filter by
     */
    void setFilterTag(const std::string& tag) { m_filterTag = tag; }
    
    /**
     * @brief Get the current filter tag
     * @return The filter tag (empty if no filter)
     */
    const std::string& getFilterTag() const { return m_filterTag; }
    
    /**
     * @brief Check if a filter tag is set
     * @return true if filter tag is not empty
     */
    bool hasFilterTag() const { return !m_filterTag.empty(); }
    
private:
    TriggerCallback m_onEnter;
    TriggerCallback m_onExit;
    TriggerCallback m_onStay;
    
    std::unordered_set<ActorObjectExtended*> m_overlappingActors;
    std::string m_filterTag;  // Only trigger for actors with this tag
};

} // namespace engine
