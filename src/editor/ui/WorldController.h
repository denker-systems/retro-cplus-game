/**
 * @file WorldController.h
 * @brief Handles World/Level/Scene initialization and management for the editor
 */
#pragma once

#include "engine/data/GameData.h"
#include <memory>
#include <string>
#include <vector>

struct SDL_Renderer;

namespace engine {
    class World;
    class Level;
    class Scene;
    class LayerManager;
}

/**
 * @brief Controller for World/Level/Scene hierarchy in the editor
 * 
 * Handles loading game data and converting it to the World→Level→Scene
 * hierarchy used by the editor viewport.
 */
class WorldController {
public:
    WorldController();
    ~WorldController();
    
    // Initialization
    void initialize(SDL_Renderer* renderer);
    
    // Accessors
    engine::World* getWorld() { return m_world.get(); }
    engine::Level* getActiveLevel();
    engine::Scene* getActiveScene();
    engine::LayerManager* getLayerManager() { return m_layerManager.get(); }
    
    // Statistics
    size_t getSceneCount() const;
    
private:
    void loadRoomsAsScenes(SDL_Renderer* renderer);
    void setupBackgroundTexture(engine::Scene* scene, const RoomData& roomData, SDL_Renderer* renderer);
    
    std::unique_ptr<engine::World> m_world;
    std::unique_ptr<engine::LayerManager> m_layerManager;
};
