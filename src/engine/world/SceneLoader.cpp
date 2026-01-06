/**
 * @file SceneLoader.cpp
 * @brief Implementation of SceneLoader
 */
#include "SceneLoader.h"
#include "LayerManager.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

namespace engine {

std::unique_ptr<Scene> SceneLoader::loadFromFile(const std::string& filepath, SDL_Renderer* renderer) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << filepath << std::endl;
        return nullptr;
    }
    
    std::string jsonStr((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    return loadFromJSON(jsonStr, renderer);
}

std::unique_ptr<Scene> SceneLoader::loadFromJSON(const std::string& jsonStr, SDL_Renderer* renderer) {
    try {
        json data = json::parse(jsonStr);
        
        std::string name = data.value("name", "Scene");
        auto scene = std::make_unique<Scene>(name);
        
        // Load camera settings
        if (data.contains("camera")) {
            loadCamera(scene.get(), data["camera"].dump());
        } else {
            // Create default camera
            scene->createDefaultCamera();
        }
        
        // Load layers
        if (data.contains("layers")) {
            loadLayers(scene.get(), data["layers"].dump(), renderer);
        }
        
        return scene;
        
    } catch (const json::exception& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return nullptr;
    }
}

void SceneLoader::loadCamera(Scene* scene, const std::string& jsonStr) {
    try {
        json data = json::parse(jsonStr);
        
        CameraComponent* camera = scene->createDefaultCamera();
        
        if (data.contains("zoom")) {
            camera->setZoom(data["zoom"].get<float>());
        }
        
        if (data.contains("bounds") && data["bounds"].is_array() && data["bounds"].size() == 4) {
            camera->setBounds(
                data["bounds"][0].get<float>(),
                data["bounds"][1].get<float>(),
                data["bounds"][2].get<float>(),
                data["bounds"][3].get<float>()
            );
        }
        
    } catch (const json::exception& e) {
        std::cerr << "Camera JSON parse error: " << e.what() << std::endl;
    }
}

void SceneLoader::loadLayers(Scene* scene, const std::string& jsonStr, SDL_Renderer* renderer) {
    try {
        json layersData = json::parse(jsonStr);
        
        for (const auto& layerData : layersData) {
            std::string name = layerData.value("name", "Layer");
            std::string typeStr = layerData.value("type", "main");
            int zIndex = layerData.value("zIndex", 0);
            
            // Convert type string to enum
            LayerType type = LayerType::Main;
            if (typeStr == "background") type = LayerType::Background;
            else if (typeStr == "foreground") type = LayerType::Foreground;
            else if (typeStr == "ui") type = LayerType::UI;
            
            // Create layer as a child node
            auto layer = std::make_unique<Layer>(name, type);
            // Note: Layer doesn't have setZIndex anymore (Actor-based)
            
            // TODO: Convert Layer to Actor-based system
            // scene->addActor(std::move(layer));
        }
        
    } catch (const json::exception& e) {
        std::cerr << "Layers JSON parse error: " << e.what() << std::endl;
    }
}

bool SceneLoader::saveToFile(const Scene* scene, const std::string& filepath) {
    if (!scene) return false;
    
    std::string jsonStr = toJSON(scene);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << jsonStr;
    file.close();
    
    return true;
}

std::string SceneLoader::toJSON(const Scene* scene) {
    if (!scene) return "{}";
    
    json data;
    data["name"] = scene->getName();
    
    // Save camera settings
    if (scene->getActiveCamera()) {
        CameraComponent* camera = scene->getActiveCamera();
        data["camera"]["zoom"] = camera->getZoom();
        
        // TODO: Save camera bounds if needed
    }
    
    // Save layers
    // TODO: Iterate through scene children and save Layer nodes
    
    return data.dump(2);  // Pretty print with 2-space indent
}

} // namespace engine
