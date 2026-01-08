/**
 * @file SelectionManager.cpp
 * @brief Central selection management implementation
 */
#include "SelectionManager.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"

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
    m_context.clearSelection();
    m_selectedActor = nullptr;
    m_selectedScene = nullptr;
    
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

void SelectionManager::notifySelectionChanged() {
    for (auto& cb : m_callbacks) {
        cb();
    }
}
