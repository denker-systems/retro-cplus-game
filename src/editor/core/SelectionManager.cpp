/**
 * @file SelectionManager.cpp
 * @brief Central selection management implementation
 */
#include "SelectionManager.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include <iostream>

SelectionManager::SelectionManager(EditorContext& context) 
    : m_context(context) {
}

void SelectionManager::selectRoom(const std::string& roomId) {
    m_context.selectedType = EditorContext::SelectionType::Room;
    m_context.selectedRoomId = roomId;
    m_context.selectedActorId.clear();
    m_context.selectedCollisionBoxIndex = -1;
    m_selectedActor = nullptr;
    
    notifySelectionChanged();
}

void SelectionManager::selectActor(const std::string& actorId, engine::Scene* scene) {
    m_context.selectedType = EditorContext::SelectionType::Actor;
    m_context.selectedActorId = actorId;
    m_context.selectedCollisionBoxIndex = -1;
    m_selectedScene = scene;
    
    // Find actor in scene
    if (scene) {
        for (auto& actor : scene->getActors()) {
            if (actor->getName() == actorId) {
                m_selectedActor = actor.get();
                break;
            }
        }
    } else {
        m_selectedActor = nullptr;
    }
    
    notifySelectionChanged();
}

void SelectionManager::selectCollisionBox(int boxIndex) {
    m_context.selectedType = EditorContext::SelectionType::CollisionBox;
    m_context.selectedCollisionBoxIndex = boxIndex;
    m_context.selectedActorId.clear();
    m_selectedActor = nullptr;
    
    notifySelectionChanged();
}

void SelectionManager::selectHotspot(const std::string& roomId, int hotspotIndex) {
    m_context.selectedType = EditorContext::SelectionType::Hotspot;
    m_context.selectedRoomId = roomId;
    m_context.selectedHotspotIndex = hotspotIndex;
    m_context.selectedActorId.clear();
    m_context.selectedCollisionBoxIndex = -1;
    m_selectedActor = nullptr;
    
    notifySelectionChanged();
}

void SelectionManager::clearSelection() {
    std::cout << "[SelectionManager] clearSelection() called" << std::endl;
    
    m_context.clearSelection();
    m_selectedActor = nullptr;
    m_selectedScene = nullptr;
    
    // Close Actor Details when selection is cleared
    if (m_openActorDetailsCallback) {
        std::cout << "[SelectionManager] Closing Actor Details (passing nullptr)" << std::endl;
        m_openActorDetailsCallback(nullptr);
    }
    
    notifySelectionChanged();
}

void SelectionManager::notifyViewportSelection(engine::ActorObjectExtended* actor, engine::Scene* scene) {
    if (actor) {
        selectActor(actor->getName(), scene);
    } else {
        clearSelection();
    }
}

void SelectionManager::notifyViewportCollisionBoxSelection(int boxIndex) {
    selectCollisionBox(boxIndex);
}

void SelectionManager::selectActor(engine::ActorObjectExtended* actor) {
    std::cout << "[SelectionManager] selectActor: " << (actor ? actor->getName() : "nullptr") << std::endl;
    if (actor) {
        m_context.selectedType = EditorContext::SelectionType::Actor;
        m_context.selectedActorId = actor->getName();
        m_context.selectedCollisionBoxIndex = -1;
        m_selectedActor = actor;
        m_selectedScene = m_activeScene;
    } else {
        m_selectedActor = nullptr;
    }
    
    notifySelectionChanged();
}

void SelectionManager::notifySelectionChanged() {
    std::cout << "[SelectionManager] notifySelectionChanged - " << m_selectionCallbacks.size() << " callbacks" << std::endl;
    for (auto& cb : m_selectionCallbacks) {
        cb();
    }
}

void SelectionManager::notifyNavigationChanged() {
    std::cout << "[SelectionManager] notifyNavigationChanged - Level: " 
              << (m_activeLevel ? m_activeLevel->getName() : "null")
              << ", Scene: " << (m_activeScene ? m_activeScene->getName() : "null")
              << ", " << m_navigationCallbacks.size() << " callbacks" << std::endl;
    for (auto& cb : m_navigationCallbacks) {
        cb();
    }
}

void SelectionManager::openActorDetails(engine::ActorObjectExtended* actor) {
    std::cout << "[SelectionManager] ========================================" << std::endl;
    std::cout << "[SelectionManager] openActorDetails() called" << std::endl;
    std::cout << "[SelectionManager] Actor: " << (actor ? actor->getName() : "nullptr") << std::endl;
    std::cout << "[SelectionManager] Callback registered: " << (m_openActorDetailsCallback ? "YES" : "NO") << std::endl;
    
    if (m_openActorDetailsCallback && actor) {
        std::cout << "[SelectionManager] Invoking callback..." << std::endl;
        m_openActorDetailsCallback(actor);
        std::cout << "[SelectionManager] Callback completed" << std::endl;
    } else {
        if (!m_openActorDetailsCallback) {
            std::cout << "[SelectionManager] ERROR: No callback registered!" << std::endl;
        }
        if (!actor) {
            std::cout << "[SelectionManager] ERROR: Actor is null!" << std::endl;
        }
    }
    
    std::cout << "[SelectionManager] ========================================" << std::endl;
}
