/**
 * @file EditorPlayMode.cpp
 * @brief Editor Play Mode implementation
 */
#include "EditorPlayMode.h"
#include "engine/world/World.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/RigidBody3DComponent.h"
#include "engine/components/RigidBody2DComponent.h"
#include "engine/actors/StaticMeshActor.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/Character3DActor.h"
#include "engine/actors/NPC3DActor.h"
#include "engine/utils/Logger.h"
#include <PxPhysicsAPI.h>
#include <iostream>

namespace editor {

EditorPlayMode::EditorPlayMode() = default;

EditorPlayMode::~EditorPlayMode() {
    shutdown();
}

void EditorPlayMode::initialize() {
    LOG_INFO("[EditorPlayMode] Initializing...");
    
    m_physicsManager.initializeBoth();
    
    if (m_physicsManager.isGPUAccelerationAvailable()) {
        LOG_INFO("[EditorPlayMode] PhysX GPU acceleration available");
    } else {
        LOG_INFO("[EditorPlayMode] PhysX running on CPU");
    }
    
    LOG_INFO("[EditorPlayMode] Initialized successfully");
}

void EditorPlayMode::shutdown() {
    if (m_state != PlayState::Stopped) {
        stop();
    }
    m_physicsManager.shutdown();
}

void EditorPlayMode::play() {
    if (m_state == PlayState::Playing) return;
    
    if (m_state == PlayState::Stopped) {
        LOG_INFO("[EditorPlayMode] ========================================");
        LOG_INFO("[EditorPlayMode] STARTING PLAY MODE");
        LOG_INFO("[EditorPlayMode] ========================================");
        LOG_INFO("[EditorPlayMode] World: " + std::string(m_world ? m_world->getName() : "NULL"));
        LOG_INFO("[EditorPlayMode] Scene: " + std::string(m_activeScene ? m_activeScene->getName() : "NULL"));
        LOG_INFO("[EditorPlayMode] Physics 2D: " + std::string(m_physicsManager.is2DInitialized() ? "YES" : "NO"));
        LOG_INFO("[EditorPlayMode] Physics 3D: " + std::string(m_physicsManager.is3DInitialized() ? "YES" : "NO"));
        LOG_INFO("[EditorPlayMode] GPU Acceleration: " + std::string(m_physicsManager.isGPUAccelerationAvailable() ? "YES" : "NO"));
        
        saveSceneState();
        initializePhysicsBodies();
        spawnPlayerAtStart();
        
        m_playTime = 0.0f;
        m_frameCount = 0;
        
        if (onPlayStarted) onPlayStarted();
    } else {
        LOG_INFO("[EditorPlayMode] Resuming play mode...");
    }
    
    m_state = PlayState::Playing;
    if (onStateChanged) onStateChanged(m_state);
}

void EditorPlayMode::pause() {
    if (m_state != PlayState::Playing) return;
    
    LOG_INFO("[EditorPlayMode] Pausing play mode...");
    m_state = PlayState::Paused;
    if (onStateChanged) onStateChanged(m_state);
}

void EditorPlayMode::stop() {
    if (m_state == PlayState::Stopped) return;
    
    LOG_INFO("[EditorPlayMode] Stopping play mode...");
    
    // Cleanup player
    if (m_player) {
        m_player->shutdownController();
        m_player.reset();
        LOG_INFO("[EditorPlayMode] Player destroyed");
    }
    
    cleanupPhysicsBodies();
    restoreSceneState();
    
    m_state = PlayState::Stopped;
    
    if (onPlayStopped) onPlayStopped();
    if (onStateChanged) onStateChanged(m_state);
    
    LOG_INFO("[EditorPlayMode] Play session: " + std::to_string(m_frameCount) + 
             " frames, " + std::to_string(m_playTime) + "s");
}

void EditorPlayMode::step() {
    if (m_state != PlayState::Paused) return;
    
    update(1.0f / 60.0f);
}

void EditorPlayMode::update(float deltaTime) {
    if (m_state != PlayState::Playing) return;
    
    float scaledDt = deltaTime * m_timeScale;
    scaledDt = std::min(scaledDt, m_maxDeltaTime);
    
    m_physicsManager.step(scaledDt);
    
    // Update player
    updatePlayer(scaledDt);
    
    // Log test body position
    if (m_testBody && m_physicsManager.getWorld3D()) {
        auto* actor = static_cast<physx::PxRigidActor*>(m_testBody);
        glm::vec3 pos = m_physicsManager.getWorld3D()->getBodyPosition(actor);
        
        // Log every 30 frames
        if (m_frameCount % 30 == 0) {
            LOG_INFO("[EditorPlayMode] Test body Y=" + std::to_string(pos.y) + 
                     " (falling from 10.0)");
        }
    }
    
    if (m_activeScene) {
        for (const auto& actor : m_activeScene->getActors()) {
            // Update NPC3DActor (no physics, just update)
            if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
                npc->update(scaledDt);
                
                // Log NPC position every 120 frames
                if (m_frameCount % 120 == 0) {
                    glm::vec3 pos = npc->getPosition3D();
                    LOG_DEBUG("[EditorPlayMode] NPC '" + npc->getName() + 
                              "' at (" + std::to_string(pos.x) + ", " + 
                              std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")");
                }
            }
            // Use StaticMeshActor's syncFromPhysics for correct coordinate mapping
            else if (auto* meshActor = dynamic_cast<engine::StaticMeshActor*>(actor.get())) {
                if (meshActor->isPhysicsEnabled()) {
                    meshActor->syncFromPhysics();
                    
                    // Log position every 60 frames
                    if (m_frameCount % 60 == 0) {
                        glm::vec3 pos = meshActor->getPosition3D();
                        LOG_DEBUG("[EditorPlayMode] Actor '" + actor->getName() + 
                                  "' pos: (" + std::to_string(pos.x) + ", " + 
                                  std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")");
                    }
                }
            }
            else if (auto* rb3d = actor->getComponent<engine::RigidBody3DComponent>()) {
                if (rb3d->isInitialized()) {
                    glm::vec3 pos = rb3d->getPosition();
                    
                    // Log position every 60 frames
                    if (m_frameCount % 60 == 0) {
                        LOG_DEBUG("[EditorPlayMode] Actor '" + actor->getName() + 
                                  "' pos: (" + std::to_string(pos.x) + ", " + 
                                  std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")");
                    }
                    
                    actor->setPosition(pos.x * 100.0f, pos.z * 100.0f);
                    actor->setZ(pos.y * 100.0f);
                }
            }
        }
    }
    
    m_playTime += scaledDt;
    m_frameCount++;
}

void EditorPlayMode::saveSceneState() {
    if (!m_activeScene) return;
    
    m_sceneStateBackup.clear();
    
    for (const auto& actor : m_activeScene->getActors()) {
        m_sceneStateBackup += actor->getName() + ":" + 
                              std::to_string(actor->getX()) + "," + 
                              std::to_string(actor->getY()) + ";";
    }
    
    m_hasBackup = true;
    LOG_DEBUG("[EditorPlayMode] Scene state saved");
}

void EditorPlayMode::restoreSceneState() {
    if (!m_hasBackup || !m_activeScene) return;
    
    size_t pos = 0;
    for (const auto& actor : m_activeScene->getActors()) {
        size_t colonPos = m_sceneStateBackup.find(':', pos);
        size_t commaPos = m_sceneStateBackup.find(',', colonPos);
        size_t semiPos = m_sceneStateBackup.find(';', commaPos);
        
        if (colonPos != std::string::npos && commaPos != std::string::npos && semiPos != std::string::npos) {
            float x = std::stof(m_sceneStateBackup.substr(colonPos + 1, commaPos - colonPos - 1));
            float y = std::stof(m_sceneStateBackup.substr(commaPos + 1, semiPos - commaPos - 1));
            actor->setPosition(x, y);
            pos = semiPos + 1;
        }
    }
    
    m_hasBackup = false;
    LOG_DEBUG("[EditorPlayMode] Scene state restored");
}

void EditorPlayMode::initializePhysicsBodies() {
    if (!m_activeScene) {
        LOG_WARNING("[EditorPlayMode] No active scene - creating test physics actor");
        createTestPhysicsActor();
        return;
    }
    
    auto* world3D = m_physicsManager.getWorld3D();
    auto* world2D = m_physicsManager.getWorld2D();
    
    // Create ground plane at Y=0 for 3D physics
    // Position the box so its TOP surface is at Y=0 (center at Y=-0.5)
    if (world3D && !m_groundPlane) {
        auto* groundBody = world3D->createStaticBody(glm::vec3(0.0f, -0.5f, 0.0f));
        if (groundBody) {
            // Add a large flat box as ground (100x1x100 units)
            world3D->addBoxShape(groundBody, glm::vec3(50.0f, 0.5f, 50.0f));
            m_groundPlane = groundBody;
            LOG_INFO("[EditorPlayMode] Created ground plane (top surface at Y=0)");
        }
    }
    
    int bodyCount3D = 0;
    int bodyCount2D = 0;
    
    LOG_INFO("[EditorPlayMode] Scanning " + std::to_string(m_activeScene->getActors().size()) + " actors for physics components...");
    
    for (const auto& actor : m_activeScene->getActors()) {
        LOG_DEBUG("[EditorPlayMode] Checking actor: " + actor->getName());
        
        // Check for StaticMeshActor with physics enabled
        if (auto* meshActor = dynamic_cast<engine::StaticMeshActor*>(actor.get())) {
            if (meshActor->isPhysicsEnabled()) {
                LOG_DEBUG("[EditorPlayMode]   -> StaticMeshActor with physics");
                meshActor->initializePhysics(world3D);
                bodyCount3D++;
                LOG_INFO("[EditorPlayMode]   -> Initialized physics for StaticMeshActor: " + actor->getName());
            }
        }
        
        // Check for standalone RigidBody3DComponent
        if (auto* rb3d = actor->getComponent<engine::RigidBody3DComponent>()) {
            LOG_DEBUG("[EditorPlayMode]   -> Has RigidBody3DComponent");
            if (!rb3d->isInitialized() && world3D) {
                rb3d->initializeBody(world3D);
                bodyCount3D++;
                LOG_INFO("[EditorPlayMode]   -> Initialized 3D body for: " + actor->getName());
            }
        }
        
        if (auto* rb2d = actor->getComponent<engine::RigidBody2DComponent>()) {
            LOG_DEBUG("[EditorPlayMode]   -> Has RigidBody2DComponent");
            if (!rb2d->isInitialized() && world2D) {
                rb2d->initializeBody(world2D);
                bodyCount2D++;
                LOG_INFO("[EditorPlayMode]   -> Initialized 2D body for: " + actor->getName());
            }
        }
    }
    
    LOG_INFO("[EditorPlayMode] Physics bodies initialized: 3D=" + std::to_string(bodyCount3D) + ", 2D=" + std::to_string(bodyCount2D));
    
    // If no physics bodies found, create a test one
    if (bodyCount3D == 0 && bodyCount2D == 0) {
        LOG_INFO("[EditorPlayMode] No physics bodies found - creating test actor");
        createTestPhysicsActor();
    }
}

void EditorPlayMode::createTestPhysicsActor() {
    auto* world3D = m_physicsManager.getWorld3D();
    if (!world3D || !world3D->isInitialized()) {
        LOG_ERROR("[EditorPlayMode] Cannot create test actor - PhysX not initialized");
        return;
    }
    
    // Create ground plane if not already created
    if (!m_groundPlane) {
        auto* groundBody = world3D->createStaticBody(glm::vec3(0.0f, -0.5f, 0.0f));
        if (groundBody) {
            world3D->addBoxShape(groundBody, glm::vec3(50.0f, 0.5f, 50.0f));
            m_groundPlane = groundBody;
            LOG_INFO("[EditorPlayMode] Created ground plane (top surface at Y=0)");
        }
    }
    
    LOG_INFO("[EditorPlayMode] ----------------------------------------");
    LOG_INFO("[EditorPlayMode] CREATING TEST PHYSICS ACTOR");
    LOG_INFO("[EditorPlayMode] ----------------------------------------");
    
    // Create a falling box at position (0, 10, 0)
    engine::physics::PhysicsWorld3D::BodyDef3D def;
    def.type = engine::physics::BodyType::Dynamic;
    def.position = glm::vec3(0.0f, 10.0f, 0.0f);
    def.mass = 1.0f;
    def.useGravity = true;
    def.userData = nullptr;
    
    auto* body = world3D->createDynamicBody(def);
    if (body) {
        world3D->addBoxShape(body, glm::vec3(0.5f, 0.5f, 0.5f));
        LOG_INFO("[EditorPlayMode] Test box created at (0, 10, 0) - will fall due to gravity");
        m_testBody = body;
    } else {
        LOG_ERROR("[EditorPlayMode] Failed to create test body");
    }
}

void EditorPlayMode::cleanupPhysicsBodies() {
    // Cleanup ground plane
    if (m_groundPlane && m_physicsManager.getWorld3D()) {
        m_physicsManager.getWorld3D()->destroyBody(static_cast<physx::PxRigidActor*>(m_groundPlane));
        m_groundPlane = nullptr;
    }
    
    if (!m_activeScene) return;
    
    for (const auto& actor : m_activeScene->getActors()) {
        if (auto* rb3d = actor->getComponent<engine::RigidBody3DComponent>()) {
            rb3d->shutdown();
        }
        if (auto* rb2d = actor->getComponent<engine::RigidBody2DComponent>()) {
            rb2d->shutdown();
        }
    }
    
    LOG_DEBUG("[EditorPlayMode] Physics bodies cleaned up");
}

void EditorPlayMode::handleMouseLook(float deltaX, float deltaY) {
    if (m_state == PlayState::Playing && m_player) {
        std::cout << "[EditorPlayMode] handleMouseLook - delta=(" 
                  << deltaX << "," << deltaY << ") yaw=" << m_player->getYaw() << std::endl;
        m_player->handleMouseLook(deltaX, deltaY);
    }
}

engine::PlayerStartActor* EditorPlayMode::findPlayerStart() const {
    // Search in active scene first
    if (m_activeScene) {
        for (const auto& actor : m_activeScene->getActors()) {
            if (auto* playerStart = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
                return playerStart;
            }
        }
    }
    
    // Search in active level
    if (m_world && m_world->getActiveLevel()) {
        auto* level = m_world->getActiveLevel();
        for (const auto& actor : level->getActors()) {
            if (auto* playerStart = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
                return playerStart;
            }
        }
        // Also search in level's scenes
        for (const auto& scene : level->getScenes()) {
            for (const auto& actor : scene->getActors()) {
                if (auto* playerStart = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
                    return playerStart;
                }
            }
        }
    }
    
    // Search in world actors
    if (m_world) {
        for (const auto& actor : m_world->getActors()) {
            if (auto* playerStart = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
                return playerStart;
            }
        }
    }
    
    return nullptr;
}

void EditorPlayMode::spawnPlayerAtStart() {
    auto* world3D = m_physicsManager.getWorld3D();
    if (!world3D || !world3D->isInitialized()) {
        LOG_WARNING("[EditorPlayMode] Cannot spawn player - PhysX not initialized");
        return;
    }
    
    // Find PlayerStart actor
    glm::vec3 spawnPos(0.0f, 2.0f, 0.0f);  // Default spawn position
    
    auto* playerStart = findPlayerStart();
    if (playerStart) {
        spawnPos = playerStart->getSpawnPosition();
        LOG_INFO("[EditorPlayMode] Found PlayerStart at (" + 
                 std::to_string(spawnPos.x) + ", " + 
                 std::to_string(spawnPos.y) + ", " + 
                 std::to_string(spawnPos.z) + ")");
    } else {
        LOG_INFO("[EditorPlayMode] No PlayerStart found - using default position");
    }
    
    // Create player
    m_player = std::make_unique<engine::Player3DActor>();
    m_player->setPosition3D(spawnPos);
    
    // Initialize character controller
    if (m_player->initializeController(world3D)) {
        LOG_INFO("[EditorPlayMode] Player spawned and controller initialized");
        LOG_INFO("[EditorPlayMode] Controls: WASD to move, SPACE to jump");
    } else {
        LOG_ERROR("[EditorPlayMode] Failed to initialize player controller");
        m_player.reset();
    }
}

void EditorPlayMode::updatePlayer(float deltaTime) {
    if (!m_player) return;
    
    // Update player (handles input and physics)
    m_player->update(deltaTime);
    
    // Log player position occasionally
    if (m_frameCount % 60 == 0) {
        glm::vec3 pos = m_player->getPosition3D();
        bool grounded = m_player->isGrounded();
        LOG_DEBUG("[EditorPlayMode] Player pos: (" + 
                  std::to_string(pos.x) + ", " + 
                  std::to_string(pos.y) + ", " + 
                  std::to_string(pos.z) + ") grounded: " + 
                  (grounded ? "YES" : "NO"));
    }
}

} // namespace editor
