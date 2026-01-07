/**
 * @file EditorWorldManager.h
 * @brief Manages World/Level/Scene hierarchy for the editor
 */
#pragma once

#include <memory>
#include <string>

// Forward declarations
namespace engine {
    class World;
    class Level;
    class Scene;
    class LayerManager;
}

class EditorContext;
class Game;

/**
 * @brief Manages World/Level/Scene setup and conversion from RoomData
 */
class EditorWorldManager {
public:
    explicit EditorWorldManager(EditorContext& context);
    ~EditorWorldManager() = default;
    
    void initialize(Game* game);
    void shutdown();
    
    // World access
    engine::World* getWorld() const { return m_world.get(); }
    engine::LayerManager* getLayerManager() const { return m_layerManager.get(); }
    
    // Sync operations
    void syncScenesToRoomData();
    
private:
    void createWorldFromRoomData(Game* game);
    void loadSceneBackgrounds(Game* game);
    
    EditorContext& m_context;
    std::unique_ptr<engine::World> m_world;
    std::unique_ptr<engine::LayerManager> m_layerManager;
};
