/**
 * @file RuntimeWorld.cpp
 * @brief Implementation av RuntimeWorld
 */
#include "RuntimeWorld.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/StaticMeshActor.h"
#include "engine/physics/PhysicsManager.h"
#include "engine/physics/box2d/PhysicsWorld2D.h"
#include "engine/physics/physx/PhysicsWorld3D.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <PxPhysicsAPI.h>

RuntimeWorld::RuntimeWorld() = default;

RuntimeWorld::~RuntimeWorld() = default;

bool RuntimeWorld::loadFromJSON(engine::physics::PhysicsManager* physicsManager) {
    LOG_INFO("[RuntimeWorld] Loading world from JSON...");
    
    // Create world
    m_world = std::make_unique<engine::World>();
    m_world->setName("RuntimeWorld");
    
    // Load scenes from JSON
    LOG_INFO("[RuntimeWorld] Loading scenes...");
    if (!loadScenes()) {
        LOG_ERROR("[RuntimeWorld] Failed to load scenes");
        return false;
    }
    LOG_INFO("[RuntimeWorld] Scenes loaded, creating ground plane...");
    
    // Create ground plane for physics
    if (!createGroundPlane(physicsManager)) {
        LOG_ERROR("[RuntimeWorld] Failed to create ground plane");
        return false;
    }
    LOG_INFO("[RuntimeWorld] Ground plane created successfully");
    
    LOG_INFO("[RuntimeWorld] World loaded successfully");
    return true;
}

bool RuntimeWorld::loadScenes() {
    LOG_INFO("[RuntimeWorld] Loading scenes from assets/data/scenes.json...");
    
    // Try to load scenes.json
    std::ifstream file("assets/data/scenes.json");
    if (!file.is_open()) {
        LOG_WARNING("[RuntimeWorld] scenes.json not found, creating default scene");
        
        // Create default test scene
        auto level = std::make_unique<engine::Level>();
        level->setName("DefaultLevel");
        
        auto scene = std::make_unique<engine::Scene>();
        scene->setName("TestScene");
        
        // Add PlayerStart
        auto playerStart = std::make_unique<engine::PlayerStartActor>();
        playerStart->setSpawnPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        scene->addActor(std::move(playerStart));
        
        m_activeScene = scene.get();
        level->addScene(std::move(scene));
        m_world->addLevel(std::move(level));
        
        LOG_INFO("[RuntimeWorld] Default scene created");
        return true;
    }
    
    // Parse JSON
    nlohmann::json j;
    try {
        file >> j;
    } catch (const std::exception& e) {
        LOG_ERROR("[RuntimeWorld] Failed to parse scenes.json: " + std::string(e.what()));
        return false;
    }
    
    // Create levels and scenes from JSON
    if (j.contains("levels")) {
        for (const auto& levelData : j["levels"]) {
            auto level = std::make_unique<engine::Level>();
            
            if (levelData.contains("name")) {
                level->setName(levelData["name"]);
            }
            
            if (levelData.contains("scenes")) {
                for (const auto& sceneData : levelData["scenes"]) {
                    auto scene = createSceneFromJSON(&sceneData);
                    if (scene) {
                        // Set first scene as active
                        if (!m_activeScene) {
                            m_activeScene = scene;
                        }
                        level->addScene(std::unique_ptr<engine::Scene>(scene));
                    }
                }
            }
            
            m_world->addLevel(std::move(level));
        }
    }
    
    // Fallback: create default scene if no scenes loaded
    if (!m_activeScene) {
        LOG_WARNING("[RuntimeWorld] No scenes found in JSON, creating default");
        
        auto level = std::make_unique<engine::Level>();
        level->setName("DefaultLevel");
        
        auto scene = std::make_unique<engine::Scene>();
        scene->setName("DefaultScene");
        
        auto playerStart = std::make_unique<engine::PlayerStartActor>();
        playerStart->setSpawnPosition(glm::vec3(0.0f, 2.0f, 0.0f));
        scene->addActor(std::move(playerStart));
        
        m_activeScene = scene.get();
        level->addScene(std::move(scene));
        m_world->addLevel(std::move(level));
    }
    
    LOG_INFO("[RuntimeWorld] Loaded " + std::to_string(m_world->getLevels().size()) + " levels");
    return true;
}

engine::Scene* RuntimeWorld::createSceneFromJSON(const void* data) {
    auto& sceneData = *const_cast<nlohmann::json*>(static_cast<const nlohmann::json*>(data));
    auto scene = new engine::Scene();
    
    if (sceneData.contains("name")) {
        scene->setName(sceneData["name"]);
    }
    
    // Add PlayerStart if specified
    if (sceneData.contains("playerSpawn")) {
        auto playerStart = std::make_unique<engine::PlayerStartActor>();
        
        glm::vec3 spawnPos(0.0f, 2.0f, 0.0f);
        try {
            if (sceneData.at("playerSpawn").contains("x")) {
                spawnPos.x = sceneData.at("playerSpawn").at("x");
            }
            if (sceneData.at("playerSpawn").contains("y")) {
                spawnPos.y = sceneData.at("playerSpawn").at("y");
            }
            if (sceneData.at("playerSpawn").contains("z")) {
                spawnPos.z = sceneData.at("playerSpawn").at("z");
            }
        } catch (...) {
            LOG_WARNING("[RuntimeWorld] Failed to parse playerSpawn, using default");
        }
        
        playerStart->setSpawnPosition(spawnPos);
        scene->addActor(std::move(playerStart));
        
        LOG_INFO("[RuntimeWorld] Added PlayerStart at (" + 
                 std::to_string(spawnPos.x) + ", " + 
                 std::to_string(spawnPos.y) + ", " + 
                 std::to_string(spawnPos.z) + ")");
    }
    
    // TODO: Load actors from JSON
    // TODO: Load static meshes from JSON
    
    return scene;
}

bool RuntimeWorld::createGroundPlane(engine::physics::PhysicsManager* physicsManager) {
    LOG_INFO("[RuntimeWorld] Creating ground plane...");
    
    auto* world3D = physicsManager->getWorld3D();
    if (!world3D || !world3D->isInitialized()) {
        LOG_ERROR("[RuntimeWorld] PhysicsWorld3D not initialized");
        return false;
    }
    
    // Create ground plane (same as EditorPlayMode)
    // Position box so its TOP surface is at Y=0 (center at Y=-0.5)
    auto* groundBody = world3D->createStaticBody(glm::vec3(0.0f, -0.5f, 0.0f));
    if (!groundBody) {
        LOG_ERROR("[RuntimeWorld] Failed to create ground body");
        return false;
    }
    
    // Add large flat box as ground (100x1x100 units)
    world3D->addBoxShape(groundBody, glm::vec3(50.0f, 0.5f, 50.0f));
    
    LOG_INFO("[RuntimeWorld] Ground plane created (top surface at Y=0, 100x1x100 units)");
    return true;
}
