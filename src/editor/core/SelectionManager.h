/**
 * @file SelectionManager.h
 * @brief Central selection management for editor panels
 */
#pragma once

#include "EditorContext.h"
#include <functional>

namespace engine {
    class Scene;
    class ActorObjectExtended;
}

/**
 * @brief Manages selection synchronization across panels
 */
class SelectionManager {
public:
    using SelectionCallback = std::function<void()>;
    
    explicit SelectionManager(EditorContext& context);
    
    // Selection management
    void selectRoom(const std::string& roomId);
    void selectActor(const std::string& actorId, engine::Scene* scene);
    void selectCollisionBox(int boxIndex);
    void selectHotspot(const std::string& roomId, int hotspotIndex);
    void clearSelection();
    
    // Get selected items
    engine::ActorObjectExtended* getSelectedActor() const { return m_selectedActor; }
    engine::Scene* getSelectedScene() const { return m_selectedScene; }
    
    // Registration for panel callbacks
    void registerSelectionChangedCallback(SelectionCallback cb) {
        m_callbacks.push_back(cb);
    }
    
    // Update from viewport (called when viewport selects something)
    void notifyViewportSelection(engine::ActorObjectExtended* actor, engine::Scene* scene);
    void notifyViewportCollisionBoxSelection(int boxIndex);
    
private:
    EditorContext& m_context;
    engine::ActorObjectExtended* m_selectedActor = nullptr;
    engine::Scene* m_selectedScene = nullptr;
    
    std::vector<SelectionCallback> m_callbacks;
    
    void notifySelectionChanged();
};
