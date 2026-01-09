/**
 * @file SceneLoader.h
 * @brief Load scenes from JSON files
 */
#pragma once

#include "Scene.h"
#include <memory>
#include <string>
#include <SDL.h>

namespace engine {

/**
 * @brief Loads scenes from JSON format
 * 
 * JSON Format:
 * {
 *   "name": "level1",
 *   "camera": { "zoom": 1.0, "bounds": [0, 0, 1280, 800] },
 *   "actors": [
 *     { "type": "StaticMeshActor", "name": "Cube1", "position": [0, 5, 0], ... },
 *     { "type": "CameraActor", "name": "MainCamera", ... },
 *     { "type": "LightActor", "name": "Sun", "lightType": "directional", ... }
 *   ]
 * }
 */
class SceneLoader {
public:
    /** @brief Load scene from JSON file */
    static std::unique_ptr<Scene> loadFromFile(const std::string& filepath, SDL_Renderer* renderer);
    
    /** @brief Load scene from JSON string */
    static std::unique_ptr<Scene> loadFromJSON(const std::string& jsonStr, SDL_Renderer* renderer);
    
    /** @brief Save scene to JSON file */
    static bool saveToFile(const Scene* scene, const std::string& filepath);
    
    /** @brief Convert scene to JSON string */
    static std::string toJSON(const Scene* scene);
    
protected:
    static void loadLayers(Scene* scene, const std::string& jsonStr, SDL_Renderer* renderer);
    static void loadCamera(Scene* scene, const std::string& jsonStr);
    static void loadActors(Scene* scene, const std::string& jsonStr);
    static void saveActors(const Scene* scene, nlohmann::json& data);
};

} // namespace engine
