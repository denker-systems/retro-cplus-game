/**
 * @file TriggerComponent.cpp
 * @brief Implementation of TriggerComponent
 */
#include "TriggerComponent.h"
#include "engine/core/ActorObjectExtended.h"

namespace engine {

TriggerComponent::TriggerComponent(const std::string& name)
    : ActorComponent(name) {
}

void TriggerComponent::initialize() {
    // Initialize trigger component - setup callback tracking
}

void TriggerComponent::shutdown() {
    m_overlappingActors.clear();
    m_onEnter = nullptr;
    m_onExit = nullptr;
    m_onStay = nullptr;
}

void TriggerComponent::update(float deltaTime) {
    // Update trigger state - call onStay for all currently overlapping actors
    if (m_onStay) {
        for (auto* actor : m_overlappingActors) {
            m_onStay(actor);
        }
    }
}

void TriggerComponent::onTriggerEnter(ActorObjectExtended* other) {
    if (!other) return;
    
    // Check if already tracking this actor
    if (m_overlappingActors.find(other) != m_overlappingActors.end()) {
        return;
    }
    
    // Add to tracking set and fire enter callback
    m_overlappingActors.insert(other);
    if (m_onEnter) {
        m_onEnter(other);
    }
}

void TriggerComponent::onTriggerExit(ActorObjectExtended* other) {
    if (!other) return;
    
    // Check if currently tracking this actor
    auto it = m_overlappingActors.find(other);
    if (it == m_overlappingActors.end()) {
        return;
    }
    
    // Remove from tracking set and fire exit callback
    m_overlappingActors.erase(it);
    if (m_onExit) {
        m_onExit(other);
    }
}

void TriggerComponent::onTriggerStay(ActorObjectExtended* other) {
    if (!other) return;
    
    // Only fire stay callback if actor is currently overlapping
    if (m_overlappingActors.find(other) == m_overlappingActors.end()) {
        return;
    }
    
    // Fire stay callback (also called continuously in update)
    if (m_onStay) {
        m_onStay(other);
    }
}

bool TriggerComponent::isOverlapping(ActorObjectExtended* actor) const {
    return m_overlappingActors.find(actor) != m_overlappingActors.end();
}

} // namespace engine
