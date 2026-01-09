/**
 * @file SceneLoader.cpp
 * @brief Implementation of SceneLoader
 */
#include "SceneLoader.h"
#include "LayerManager.h"
#include "engine/actors/StaticMeshActor.h"
#include "engine/actors/CameraActor.h"
#include "engine/actors/LightActor.h"
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
        
        // Load layers (legacy)
        if (data.contains("layers")) {
            loadLayers(scene.get(), data["layers"].dump(), renderer);
        }
        
        // Load 3D actors
        if (data.contains("actors")) {
            loadActors(scene.get(), data["actors"].dump());
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
    }
    
    // Save 3D actors
    saveActors(scene, data);
    
    return data.dump(2);
}

void SceneLoader::loadActors(Scene* scene, const std::string& jsonStr) {
    try {
        json actorsData = json::parse(jsonStr);
        
        for (const auto& actorData : actorsData) {
            std::string type = actorData.value("type", "");
            std::string name = actorData.value("name", "Actor");
            
            if (type == "StaticMeshActor") {
                auto actor = std::make_unique<StaticMeshActor>(name);
                
                if (actorData.contains("position") && actorData["position"].size() == 3) {
                    actor->setPosition3D(glm::vec3(
                        actorData["position"][0].get<float>(),
                        actorData["position"][1].get<float>(),
                        actorData["position"][2].get<float>()
                    ));
                }
                
                if (actorData.contains("scale")) {
                    actor->setMeshScale(actorData["scale"].get<float>());
                }
                
                if (actorData.contains("primitive")) {
                    std::string prim = actorData["primitive"].get<std::string>();
                    if (prim == "cube") actor->setPrimitive(MeshPrimitive::Cube);
                    else if (prim == "sphere") actor->setPrimitive(MeshPrimitive::Sphere);
                    else if (prim == "plane") actor->setPrimitive(MeshPrimitive::Plane);
                    else if (prim == "cylinder") actor->setPrimitive(MeshPrimitive::Cylinder);
                }
                
                if (actorData.contains("color") && actorData["color"].size() == 3) {
                    actor->setColor(glm::vec3(
                        actorData["color"][0].get<float>(),
                        actorData["color"][1].get<float>(),
                        actorData["color"][2].get<float>()
                    ));
                }
                
                if (actorData.contains("physicsEnabled")) {
                    actor->setPhysicsEnabled(actorData["physicsEnabled"].get<bool>());
                }
                
                if (actorData.contains("bodyType")) {
                    std::string bt = actorData["bodyType"].get<std::string>();
                    if (bt == "static") actor->setBodyType(PhysicsBodyType::Static);
                    else if (bt == "dynamic") actor->setBodyType(PhysicsBodyType::Dynamic);
                    else if (bt == "kinematic") actor->setBodyType(PhysicsBodyType::Kinematic);
                }
                
                scene->addActor(std::move(actor));
                
            } else if (type == "CameraActor") {
                auto actor = std::make_unique<CameraActor>(name);
                
                if (actorData.contains("position") && actorData["position"].size() == 3) {
                    actor->setPosition3D(glm::vec3(
                        actorData["position"][0].get<float>(),
                        actorData["position"][1].get<float>(),
                        actorData["position"][2].get<float>()
                    ));
                }
                
                if (actorData.contains("target") && actorData["target"].size() == 3) {
                    actor->setTarget(glm::vec3(
                        actorData["target"][0].get<float>(),
                        actorData["target"][1].get<float>(),
                        actorData["target"][2].get<float>()
                    ));
                }
                
                if (actorData.contains("fov")) {
                    actor->setFOV(actorData["fov"].get<float>());
                }
                
                scene->addActor(std::move(actor));
                
            } else if (type == "LightActor") {
                std::string lightTypeStr = actorData.value("lightType", "point");
                LightType lightType = LightType::Point;
                if (lightTypeStr == "directional") lightType = LightType::Directional;
                else if (lightTypeStr == "spot") lightType = LightType::Spot;
                
                auto actor = std::make_unique<LightActor>(name, lightType);
                
                if (actorData.contains("position") && actorData["position"].size() == 3) {
                    actor->setPosition3D(glm::vec3(
                        actorData["position"][0].get<float>(),
                        actorData["position"][1].get<float>(),
                        actorData["position"][2].get<float>()
                    ));
                }
                
                if (actorData.contains("direction") && actorData["direction"].size() == 3) {
                    actor->setDirection(glm::vec3(
                        actorData["direction"][0].get<float>(),
                        actorData["direction"][1].get<float>(),
                        actorData["direction"][2].get<float>()
                    ));
                }
                
                if (actorData.contains("color") && actorData["color"].size() == 3) {
                    actor->setColor(glm::vec3(
                        actorData["color"][0].get<float>(),
                        actorData["color"][1].get<float>(),
                        actorData["color"][2].get<float>()
                    ));
                }
                
                if (actorData.contains("intensity")) {
                    actor->setIntensity(actorData["intensity"].get<float>());
                }
                
                if (actorData.contains("radius")) {
                    actor->setRadius(actorData["radius"].get<float>());
                }
                
                scene->addActor(std::move(actor));
            }
        }
        
    } catch (const json::exception& e) {
        std::cerr << "Actors JSON parse error: " << e.what() << std::endl;
    }
}

void SceneLoader::saveActors(const Scene* scene, json& data) {
    json actorsArray = json::array();
    
    for (const auto& actor : scene->getActors()) {
        json actorData;
        
        if (auto* mesh = dynamic_cast<StaticMeshActor*>(actor.get())) {
            actorData["type"] = "StaticMeshActor";
            actorData["name"] = mesh->getName();
            
            glm::vec3 pos = mesh->getPosition3D();
            actorData["position"] = {pos.x, pos.y, pos.z};
            actorData["scale"] = mesh->getMeshScale();
            
            switch (mesh->getPrimitive()) {
                case MeshPrimitive::Cube: actorData["primitive"] = "cube"; break;
                case MeshPrimitive::Sphere: actorData["primitive"] = "sphere"; break;
                case MeshPrimitive::Plane: actorData["primitive"] = "plane"; break;
                case MeshPrimitive::Cylinder: actorData["primitive"] = "cylinder"; break;
            }
            
            glm::vec3 col = mesh->getColor();
            actorData["color"] = {col.r, col.g, col.b};
            actorData["physicsEnabled"] = mesh->isPhysicsEnabled();
            
            switch (mesh->getBodyType()) {
                case PhysicsBodyType::Static: actorData["bodyType"] = "static"; break;
                case PhysicsBodyType::Dynamic: actorData["bodyType"] = "dynamic"; break;
                case PhysicsBodyType::Kinematic: actorData["bodyType"] = "kinematic"; break;
            }
            
        } else if (auto* cam = dynamic_cast<CameraActor*>(actor.get())) {
            actorData["type"] = "CameraActor";
            actorData["name"] = cam->getName();
            
            glm::vec3 pos = cam->getPosition3D();
            actorData["position"] = {pos.x, pos.y, pos.z};
            
            glm::vec3 target = cam->getTarget();
            actorData["target"] = {target.x, target.y, target.z};
            actorData["fov"] = cam->getFOV();
            
        } else if (auto* light = dynamic_cast<LightActor*>(actor.get())) {
            actorData["type"] = "LightActor";
            actorData["name"] = light->getName();
            
            switch (light->getLightType()) {
                case LightType::Directional: actorData["lightType"] = "directional"; break;
                case LightType::Point: actorData["lightType"] = "point"; break;
                case LightType::Spot: actorData["lightType"] = "spot"; break;
            }
            
            glm::vec3 pos = light->getPosition3D();
            actorData["position"] = {pos.x, pos.y, pos.z};
            
            glm::vec3 dir = light->getDirection();
            actorData["direction"] = {dir.x, dir.y, dir.z};
            
            glm::vec3 col = light->getColor();
            actorData["color"] = {col.r, col.g, col.b};
            actorData["intensity"] = light->getIntensity();
            actorData["radius"] = light->getRadius();
        }
        
        if (!actorData.empty()) {
            actorsArray.push_back(actorData);
        }
    }
    
    if (!actorsArray.empty()) {
        data["actors"] = actorsArray;
    }
}

} // namespace engine
