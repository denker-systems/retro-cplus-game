/**
 * @file RoomToSceneConverter.h
 * @brief Converts legacy RoomData to Scene hierarchy
 */
#pragma once

#include "Scene.h"
#include "engine/data/GameData.h"
#include <memory>
#include <SDL.h>

namespace engine {

/**
 * @brief Converts RoomData (JSON) to Scene (Node hierarchy)
 * 
 * This is a transitional tool to convert legacy room data format
 * into the new Scene-based architecture.
 * 
 * Usage:
 *   RoomData roomData = loadRoomFromJSON("tavern.json");
 *   auto scene = RoomToSceneConverter::convert(roomData, renderer);
 */
class RoomToSceneConverter {
public:
    /**
     * @brief Convert RoomData to Scene
     * @param roomData Legacy room data structure
     * @param renderer SDL renderer for loading textures
     * @return Unique pointer to constructed Scene
     */
    static std::unique_ptr<Scene> convert(const RoomData& roomData, SDL_Renderer* renderer);
    
private:
    /** @brief Create background sprite from room data */
    static std::unique_ptr<Node> createBackground(const RoomData& roomData, SDL_Renderer* renderer);
    
    /** @brief Create walk area polygon from room data */
    static std::unique_ptr<Node> createWalkArea(const RoomData& roomData);
    
    /** @brief Create hotspot interactive areas */
    static std::vector<std::unique_ptr<Node>> createHotspots(const RoomData& roomData);
    
    /** @brief Create player spawn marker */
    static std::unique_ptr<Node> createPlayerSpawn(const RoomData& roomData);
    
    /** @brief Create container node for organization */
    static std::unique_ptr<Node> createContainer(const std::string& name);
};

} // namespace engine
