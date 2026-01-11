/**
 * @file EditorWorldManager.h
 * @brief Manages World/Level/Scene hierarchy for the editor
 * 
 * Hierarchy: World → Level → Scene
 * Data flow: world.json → WorldData → engine::World
 *            scenes.json → RoomData → engine::Scene
 */
#pragma once

#include "editor/data/EditorDataTypes.h"
#include <memory>
#include <string>

// Forward declarations
namespace engine {
    class World;
    class Level;
    class Scene;
    class LayerManager;
    class WorldContainer;
}

class EditorContext;
class Game;

/**
 * @brief Manages World/Level/Scene hierarchy for the editor
 * 
 * Responsibilities:
 * - Load world.json and scenes.json
 * - Create engine::World/Level/Scene hierarchy
 * - Sync changes back to data files on save
 */
class EditorWorldManager {
public:
    explicit EditorWorldManager(EditorContext& context);
    ~EditorWorldManager() = default;
    
    // Global accessor for AI tools
    static EditorWorldManager* s_instance;
    static EditorWorldManager* getInstance() { return s_instance; }
    
    void initialize(Game* game);
    void shutdown();
    
    // World access
    engine::World* getWorld() const { return m_world.get(); }
    engine::LayerManager* getLayerManager() const { return m_layerManager.get(); }
    
    // Data access
    editor::WorldData& getWorldData() { return m_worldData; }
    const editor::WorldData& getWorldData() const { return m_worldData; }
    
    // Sync operations (call before saving)
    void syncScenesToRoomData();   // Scene gridPos → RoomData (scenes.json)
    void syncLevelsToWorldData();  // Level gridPos → WorldData (world.json)
    
    // Save operations
    void saveWorldData();  // Saves world.json
    
    // Scene operations for AI tools
    bool addSceneToLevel(const std::string& sceneId, const std::string& levelId = "");
    std::string getActiveLevelId() const;
    void refreshViewport();  // Force viewport to update
    
    // Access to EditorContext for AI tools
    EditorContext& getContext() { return m_context; }
    
    // Find scene by ID for direct actor access
    engine::Scene* findSceneById(const std::string& sceneId);
    
private:
    void loadWorldData();
    void createDefaultWorldData();
    void createWorldFromWorldData(Game* game);
    
    // Actor sync helpers (shared for World/Level/Scene)
    void syncActorsFromContainer(engine::WorldContainer* container, 
                                 std::vector<editor::ActorData>& actorDataList);
    void loadActorsToContainer(engine::WorldContainer* container,
                               const std::vector<editor::ActorData>& actorDataList);
    
    EditorContext& m_context;
    editor::WorldData m_worldData;
    std::unique_ptr<engine::World> m_world;
    std::unique_ptr<engine::LayerManager> m_layerManager;
};
