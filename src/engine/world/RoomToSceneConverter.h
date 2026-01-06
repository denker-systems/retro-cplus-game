/**
 * @file RoomToSceneConverter.h
 * @brief Converts legacy RoomData (now SceneData) to Scene hierarchy
 */
#pragma once

#include "Scene.h"
#include "engine/data/GameData.h"
#include <memory>
#include <SDL.h>

namespace engine {

/**
 * @brief Converts SceneData (JSON) to Scene (Actor hierarchy)
 * 
 * This is a transitional tool to convert scene data format
 * into the Scene-based architecture.
 * 
 * Usage:
 *   SceneData sceneData = loadSceneFromJSON("tavern.json");
 *   auto scene = RoomToSceneConverter::convert(sceneData, renderer);
 */
class RoomToSceneConverter {
public:
    /**
     * @brief Convert SceneData to Scene
     * @param sceneData Scene data structure
     * @param renderer SDL renderer for loading textures
     * @return Unique pointer to constructed Scene
     */
    static std::unique_ptr<Scene> convert(const SceneData& sceneData, SDL_Renderer* renderer);
    
private:
    /** @brief Create background sprite from scene data */
    static std::unique_ptr<ActorObject> createBackground(const SceneData& sceneData, SDL_Renderer* renderer);
    
    /** @brief Create walk area polygon from scene data */
    static std::unique_ptr<ActorObject> createWalkArea(const SceneData& sceneData);
    
    /** @brief Create hotspot interactive areas */
    static std::vector<std::unique_ptr<ActorObject>> createHotspots(const SceneData& sceneData);
    
    /** @brief Create player spawn marker */
    static std::unique_ptr<ActorObject> createPlayerSpawn(const SceneData& sceneData);
    
    /** @brief Create container actor for organization */
    static std::unique_ptr<ActorObject> createContainer(const std::string& name);
};

} // namespace engine
