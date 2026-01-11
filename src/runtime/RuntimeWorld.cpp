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
#include "engine/actors/NPC3DActor.h"
#include "engine/actors/Character3DActor.h"
#include "engine/actors/PlayerConfigActor.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/physics/PhysicsManager.h"
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
    
    m_physicsManager = physicsManager;
    
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
    
    // Create collision bodies for NPCs
    createNPCColliders(physicsManager);
    
    LOG_INFO("[RuntimeWorld] World loaded successfully");
    return true;
}

bool RuntimeWorld::loadScenes() {
    LOG_INFO("[RuntimeWorld] Loading world from assets/data/world.json...");
    
    // Try to load world.json first (contains level/scene hierarchy)
    std::ifstream worldFile("assets/data/world.json");
    if (!worldFile.is_open()) {
        LOG_WARNING("[RuntimeWorld] world.json not found, loading scenes.json directly...");
        
        // Load scenes.json directly
        std::ifstream scenesFile("assets/data/scenes.json");
        if (!scenesFile.is_open()) {
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
        
        // Parse scenes.json directly (no world.json)
        nlohmann::json scenesJson;
        try {
            scenesFile >> scenesJson;
        } catch (const std::exception& e) {
            LOG_ERROR("[RuntimeWorld] Failed to parse scenes.json: " + std::string(e.what()));
            return false;
        }
        
        // Create default level and load all scenes
        auto level = std::make_unique<engine::Level>();
        level->setName("Main Game");
        
        if (scenesJson.contains("scenes")) {
            for (const auto& sceneData : scenesJson["scenes"]) {
                auto scene = createSceneFromJSON(&sceneData);
                if (scene) {
                    if (!m_activeScene) {
                        m_activeScene = scene;
                    }
                    level->addScene(std::unique_ptr<engine::Scene>(scene));
                    
                    std::string sceneName = sceneData.contains("id") ? sceneData["id"].get<std::string>() : "unknown";
                    LOG_INFO("[RuntimeWorld] Loaded scene from scenes.json: " + sceneName);
                }
            }
        }
        
        m_world->addLevel(std::move(level));
        LOG_INFO("[RuntimeWorld] Loaded " + std::to_string(m_world->getLevels().size()) + " levels from scenes.json");
        return true;
    }
    
    // Parse world.json
    nlohmann::json worldJson;
    try {
        worldFile >> worldJson;
    } catch (const std::exception& e) {
        LOG_ERROR("[RuntimeWorld] Failed to parse world.json: " + std::string(e.what()));
        return false;
    }
    
    // Load scenes.json for scene data
    std::ifstream scenesFile("assets/data/scenes.json");
    nlohmann::json scenesJson;
    if (scenesFile.is_open()) {
        try {
            scenesFile >> scenesJson;
        } catch (const std::exception& e) {
            LOG_ERROR("[RuntimeWorld] Failed to parse scenes.json: " + std::string(e.what()));
            return false;
        }
    } else {
        LOG_WARNING("[RuntimeWorld] scenes.json not found");
    }
    
    // Create levels from world.json
    if (worldJson.contains("levels")) {
        for (const auto& levelData : worldJson["levels"]) {
            auto level = std::make_unique<engine::Level>();
            
            if (levelData.contains("name")) {
                level->setName(levelData["name"]);
            }
            
            // Load scenes for this level
            if (levelData.contains("sceneIds") && scenesJson.contains("scenes")) {
                for (const auto& sceneId : levelData["sceneIds"]) {
                    std::string id = sceneId;
                    
                    // Find scene data in scenes.json
                    for (const auto& sceneData : scenesJson["scenes"]) {
                        if (sceneData.contains("id") && sceneData["id"] == id) {
                            auto scene = createSceneFromJSON(&sceneData);
                            if (scene) {
                                // Set first scene as active (or start scene from world.json)
                                if (!m_activeScene) {
                                    m_activeScene = scene;
                                }
                                if (worldJson.contains("startSceneId") && worldJson["startSceneId"] == id) {
                                    m_activeScene = scene;
                                }
                                level->addScene(std::unique_ptr<engine::Scene>(scene));
                                LOG_INFO("[RuntimeWorld] Loaded scene: " + id);
                            }
                            break;
                        }
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
    
    std::string sceneId = sceneData.contains("id") ? sceneData["id"].get<std::string>() : "";
    
    // PRIORITY 1: Load actors from scenes.json "actors" array (editor-saved data)
    bool hasActorsArray = sceneData.contains("actors") && sceneData["actors"].is_array() && !sceneData["actors"].empty();
    
    if (hasActorsArray) {
        LOG_INFO("[RuntimeWorld] Loading actors from scenes.json actors array");
        
        for (const auto& actorData : sceneData["actors"]) {
            std::string type = actorData.value("type", "Generic");
            std::string name = actorData.value("name", "Unknown");
            float x = actorData.value("x", 0.0f);
            float y = actorData.value("y", 0.0f);
            float z = actorData.value("z", 0.0f);
            
            if (type == "PlayerStart") {
                auto playerStart = std::make_unique<engine::PlayerStartActor>();
                playerStart->setSpawnPosition(glm::vec3(x, y, z));
                scene->addActor(std::move(playerStart));
                LOG_INFO("[RuntimeWorld] Added PlayerStart at (" + 
                         std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
            }
            else if (type == "NPC3D") {
                auto npc = std::make_unique<engine::NPC3DActor>();
                npc->setName(name);
                npc->setPosition3D(glm::vec3(x, y, z));
                npc->setSpriteName(actorData.value("sprite", ""));
                npc->setDialogId(actorData.value("dialogId", ""));
                scene->addActor(std::move(npc));
                LOG_INFO("[RuntimeWorld] Added NPC3D: " + name + " at (" + 
                         std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
            }
            else if (type == "Character3D") {
                auto character = std::make_unique<engine::Character3DActor>();
                character->setName(name);
                character->setPosition3D(glm::vec3(x, y, z));
                character->setYaw(actorData.value("rotationY", 0.0f));
                scene->addActor(std::move(character));
                LOG_INFO("[RuntimeWorld] Added Character3D: " + name);
            }
            else if (type == "PlayerConfig") {
                // Load PlayerConfig for camera visualization
                auto playerConfig = std::make_unique<engine::PlayerConfigActor>();
                playerConfig->setName(name);
                scene->addActor(std::move(playerConfig));
                LOG_INFO("[RuntimeWorld] Added PlayerConfig");
            }
            else if (type == "Generic" || type == "Background") {
                // Create generic actor with position
                auto actor = std::make_unique<engine::ActorObjectExtended>(name);
                actor->setPosition(x * 100.0f, z * 100.0f);  // Convert 3D to 2D position
                actor->setZ(y * 100.0f);  // Y becomes Z height
                scene->addActor(std::move(actor));
                LOG_INFO("[RuntimeWorld] Added Generic: " + name + " at (" + 
                         std::to_string(x) + ", " + std::to_string(y) + ", " + std::to_string(z) + ")");
            }
            else {
                LOG_DEBUG("[RuntimeWorld] Skipping unknown actor type: " + type);
            }
        }
    }
    else {
        // FALLBACK: Legacy loading from playerSpawnX/Y and npcs.json
        LOG_INFO("[RuntimeWorld] No actors array - using legacy loading");
        
        // Add PlayerStart from playerSpawnX/Y (convert 2D to 3D)
        if (sceneData.contains("playerSpawnX") && sceneData.contains("playerSpawnY")) {
            auto playerStart = std::make_unique<engine::PlayerStartActor>();
            
            float spawnX = sceneData["playerSpawnX"];
            float spawnY = sceneData["playerSpawnY"];
            
            glm::vec3 spawnPos(spawnX / 100.0f, 2.0f, spawnY / 100.0f);
            playerStart->setSpawnPosition(spawnPos);
            scene->addActor(std::move(playerStart));
            
            LOG_INFO("[RuntimeWorld] Added PlayerStart at (" + 
                     std::to_string(spawnPos.x) + ", " + std::to_string(spawnPos.y) + ", " + std::to_string(spawnPos.z) + ")");
        }
        
        // Load NPCs from npcs.json
        std::ifstream npcFile("assets/data/npcs.json");
        if (npcFile.is_open()) {
            nlohmann::json npcJson;
            try {
                npcFile >> npcJson;
                
                if (npcJson.contains("npcs")) {
                    for (const auto& npcData : npcJson["npcs"]) {
                        if (npcData.contains("room") && npcData["room"] == sceneId) {
                            auto npc = std::make_unique<engine::NPC3DActor>();
                            
                            if (npcData.contains("name")) {
                                npc->setName(npcData["name"]);
                            }
                            
                            if (npcData.contains("x") && npcData.contains("y")) {
                                float nx = npcData["x"];
                                float ny = npcData["y"];
                                glm::vec3 pos3d(nx / 100.0f, 1.0f, ny / 100.0f);
                                npc->setPosition3D(pos3d);
                            }
                            
                            if (npcData.contains("dialogId")) {
                                npc->setDialogId(npcData["dialogId"]);
                            }
                            if (npcData.contains("sprite")) {
                                npc->setSpriteName(npcData["sprite"]);
                            }
                            
                            scene->addActor(std::move(npc));
                            LOG_INFO("[RuntimeWorld] Added NPC: " + npcData["name"].get<std::string>());
                        }
                    }
                }
            } catch (const std::exception& e) {
                LOG_WARNING("[RuntimeWorld] Failed to load NPCs: " + std::string(e.what()));
            }
        }
        
        // Load hotspots from scenes.json as actors
        if (sceneData.contains("hotspots")) {
            for (const auto& hotspot : sceneData["hotspots"]) {
                std::string hsType = hotspot.value("type", "");
                std::string hsName = hotspot.value("name", "Unknown");
                
                // Skip NPC hotspots - they're loaded from npcs.json
                if (hsType == "npc" || hsType == "NPC") continue;
                
                // Create actor for hotspot (exit, item, examine, etc.)
                auto actor = std::make_unique<engine::ActorObjectExtended>(hsName);
                
                // Convert 2D position to 3D
                float x = hotspot.value("x", 0);
                float y = hotspot.value("y", 0);
                glm::vec3 pos3d(x / 100.0f, 1.0f, y / 100.0f);
                actor->setPosition(pos3d.x * 100.0f, pos3d.z * 100.0f);  // Store as 2D for now
                
                scene->addActor(std::move(actor));
                LOG_INFO("[RuntimeWorld] Added hotspot: " + hsName + " (" + hsType + ")");
            }
        }
    }
    
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

void RuntimeWorld::createNPCColliders(engine::physics::PhysicsManager* physicsManager) {
    if (!physicsManager || !m_activeScene) return;
    
    auto* world3D = physicsManager->getWorld3D();
    if (!world3D || !world3D->isInitialized()) {
        LOG_WARNING("[RuntimeWorld] Cannot create NPC colliders - physics not ready");
        return;
    }
    
    int colliderCount = 0;
    for (const auto& actor : m_activeScene->getActors()) {
        if (!actor) continue;
        
        // Create collision for NPC3DActor
        if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
            glm::vec3 pos = npc->getPosition3D();
            
            // Create static body at NPC position (box collider)
            auto* body = world3D->createStaticBody(pos);
            if (body) {
                // NPC collision box: 0.5x1x0.5 (same as visual)
                world3D->addBoxShape(body, glm::vec3(0.25f, 0.5f, 0.25f));
                colliderCount++;
            }
        }
    }
    
    LOG_INFO("[RuntimeWorld] Created " + std::to_string(colliderCount) + " NPC colliders");
}
