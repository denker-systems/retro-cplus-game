/**
 * @file SelectionManager.h
 * @brief Central selection management for editor panels
 */
#pragma once

#include "EditorContext.h"
#include <functional>

namespace engine {
    class World;
    class Level;
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
    
    // Navigation state (World/Level/Scene)
    void setWorld(engine::World* world) { m_world = world; notifyNavigationChanged(); }
    void setActiveLevel(engine::Level* level) { m_activeLevel = level; notifyNavigationChanged(); }
    void setActiveScene(engine::Scene* scene) { m_activeScene = scene; notifyNavigationChanged(); }
    
    engine::World* getWorld() const { return m_world; }
    engine::Level* getActiveLevel() const { return m_activeLevel; }
    engine::Scene* getActiveScene() const { return m_activeScene; }
    
    // Selection management
    void selectRoom(const std::string& roomId);
    void selectActor(const std::string& actorId, engine::Scene* scene);
    void selectActor(engine::ActorObjectExtended* actor);  // Direct pointer version
    void selectCollisionBox(int boxIndex);
    void selectHotspot(const std::string& roomId, int hotspotIndex);
    void clearSelection();
    
    // Get selected items
    engine::ActorObjectExtended* getSelectedActor() const { return m_selectedActor; }
    engine::Scene* getSelectedScene() const { return m_selectedScene; }
    
    // Registration for panel callbacks
    void registerSelectionChangedCallback(SelectionCallback cb) {
        m_selectionCallbacks.push_back(cb);
    }
    void registerNavigationChangedCallback(SelectionCallback cb) {
        m_navigationCallbacks.push_back(cb);
    }
    
    // Update from viewport (called when viewport selects something)
    void notifyViewportSelection(engine::ActorObjectExtended* actor, engine::Scene* scene);
    void notifyViewportCollisionBoxSelection(int boxIndex);
    
private:
    EditorContext& m_context;
    
    // Navigation state
    engine::World* m_world = nullptr;
    engine::Level* m_activeLevel = nullptr;
    engine::Scene* m_activeScene = nullptr;
    
    // Selection state
    engine::ActorObjectExtended* m_selectedActor = nullptr;
    engine::Scene* m_selectedScene = nullptr;
    
    std::vector<SelectionCallback> m_selectionCallbacks;
    std::vector<SelectionCallback> m_navigationCallbacks;
    
    void notifySelectionChanged();
    void notifyNavigationChanged();
};
