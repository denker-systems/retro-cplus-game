/**
 * @file EditorWorldManager.cpp
 * @brief World manager implementation - handles World/Level/Scene hierarchy
 */
#include "EditorWorldManager.h"
#include "game/Game.h"
#include "editor/core/EditorContext.h"
#include "editor/data/EditorDataTypes.h"
#include "editor/data/EditorDataManager.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/world/LayerManager.h"
#include "engine/data/DataLoader.h"
#include "engine/world/RoomToSceneConverter.h"
#include "engine/graphics/TextureManager.h"
#include "engine/components/SpriteComponent.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/utils/Logger.h"
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Static instance for global access
EditorWorldManager* EditorWorldManager::s_instance = nullptr;

EditorWorldManager::EditorWorldManager(EditorContext& context)
    : m_context(context) {
    s_instance = this;
}

void EditorWorldManager::initialize(Game* game) {
    // Initialize TextureManager
    auto& texMgr = TextureManager::instance();
    texMgr.init(game->getRenderer());
    
    // Load all game data (scenes, items, etc.)
    auto& loader = DataLoader::instance();
    if (loader.getRooms().empty()) {
        loader.loadAll("assets/data/");
    }
    
    // Load world.json and create hierarchy
    loadWorldData();
    createWorldFromWorldData(game);
    
    LOG_INFO("EditorWorldManager: World '" + m_worldData.name + "' created with " + 
             std::to_string(m_worldData.levels.size()) + " levels, " +
             std::to_string(loader.getRooms().size()) + " scenes");
}

void EditorWorldManager::loadWorldData() {
    std::ifstream file("assets/data/world.json");
    if (file.is_open()) {
        try {
            json j = json::parse(file);
            m_worldData = j.get<editor::WorldData>();
            LOG_INFO("Loaded world.json: " + m_worldData.name + " with " + 
                     std::to_string(m_worldData.levels.size()) + " levels");
        } catch (const json::exception& e) {
            LOG_ERROR("Failed to parse world.json: " + std::string(e.what()));
            createDefaultWorldData();
        }
    } else {
        LOG_WARNING("world.json not found, creating default");
        createDefaultWorldData();
    }
}

void EditorWorldManager::createDefaultWorldData() {
    m_worldData.name = "Retro Adventure";
    m_worldData.version = "1.0";
    m_worldData.startLevelId = "main_game";
    m_worldData.startSceneId = "tavern";
    
    // Create default level with all scenes
    editor::LevelData defaultLevel;
    defaultLevel.id = "main_game";
    defaultLevel.name = "Main Game";
    defaultLevel.description = "The main game world";
    defaultLevel.gridPosition = {0, 0, 800, 600};
    
    // Add all room IDs to the default level
    const auto& rooms = DataLoader::instance().getRooms();
    for (const auto& room : rooms) {
        defaultLevel.sceneIds.push_back(room.id);
    }
    
    m_worldData.levels.push_back(defaultLevel);
}

void EditorWorldManager::createWorldFromWorldData(Game* game) {
    m_world = std::make_unique<engine::World>(m_worldData.name);
    
    // Load World-level actors
    loadActorsToContainer(m_world.get(), m_worldData.actors);
    if (!m_worldData.actors.empty()) {
        LOG_DEBUG("Loaded " + std::to_string(m_worldData.actors.size()) + " World-level actors");
    }
    
    const auto& rooms = DataLoader::instance().getRooms();
    
    // Create Levels from WorldData
    for (const auto& levelData : m_worldData.levels) {
        auto level = std::make_unique<engine::Level>(levelData.id, levelData.name);
        level->setDescription(levelData.description);
        level->setGridPosition(levelData.gridPosition);
        
        if (!levelData.musicTrack.empty()) {
            level->setMusic(levelData.musicTrack);
        }
        
        LOG_DEBUG("Creating Level '" + levelData.name + "' at grid (" + 
                 std::to_string(levelData.gridPosition.gridX) + "," + 
                 std::to_string(levelData.gridPosition.gridY) + ")");
        
        // Load Level-level actors
        loadActorsToContainer(level.get(), levelData.actors);
        if (!levelData.actors.empty()) {
            LOG_DEBUG("  Loaded " + std::to_string(levelData.actors.size()) + " Level-level actors");
        }
        
        // Add Scenes to Level based on sceneIds
        int defaultGridX = 0;
        int defaultGridY = 0;
        
        for (const auto& sceneId : levelData.sceneIds) {
            // Find RoomData by ID
            auto roomIt = std::find_if(rooms.begin(), rooms.end(),
                [&sceneId](const RoomData& r) { return r.id == sceneId; });
            
            if (roomIt != rooms.end()) {
                const auto& roomData = *roomIt;
                auto scene = engine::Scene::createFromData(roomData);
                
                // Load background
                if (!roomData.background.empty()) {
                    auto* bgActor = scene->findActor("Background");
                    if (bgActor) {
                        auto* spriteComp = bgActor->getComponent<engine::SpriteComponent>();
                        if (spriteComp) {
                            SDL_Texture* texture = TextureManager::instance().load(roomData.background);
                            if (texture) {
                                spriteComp->setTexture(texture);
                                spriteComp->setSize(640, 400);
                                spriteComp->setSourceRect({0, 0, 640, 400});
                            }
                        }
                    }
                }
                
                // Set grid position from RoomData (scenes.json) or default
                if (roomData.gridPosition) {
                    scene->setGridPosition(
                        roomData.gridPosition->gridX,
                        roomData.gridPosition->gridY,
                        roomData.gridPosition->pixelWidth,
                        roomData.gridPosition->pixelHeight
                    );
                    LOG_DEBUG("  Scene '" + roomData.name + "' at grid (" + 
                             std::to_string(roomData.gridPosition->gridX) + "," + 
                             std::to_string(roomData.gridPosition->gridY) + ")");
                } else {
                    scene->setGridPosition(defaultGridX, defaultGridY, 640, 400);
                    defaultGridX++;
                    if (defaultGridX >= 4) {
                        defaultGridX = 0;
                        defaultGridY++;
                    }
                }
                
                // Add default PlayerStart to Tavern scene for quick testing
                if (sceneId == "tavern") {
                    auto playerStart = std::make_unique<engine::PlayerStartActor>();
                    playerStart->setSpawnPosition(glm::vec3(0.0f, 2.5f, 4.0f));
                    scene->addActor(std::move(playerStart));
                    LOG_INFO("[EditorWorldManager] Added default PlayerStart to Tavern scene");
                }
                
                level->addScene(std::move(scene));
            } else {
                LOG_WARNING("Scene ID '" + sceneId + "' not found in rooms data");
            }
        }
        
        m_world->addLevel(std::move(level));
    }
    
    // Set active level
    if (!m_worldData.startLevelId.empty()) {
        m_world->setActiveLevel(m_worldData.startLevelId);
    }
}

void EditorWorldManager::syncScenesToRoomData() {
    LOG_INFO("=== syncScenesToRoomData() START ===");
    
    if (!m_world) {
        LOG_ERROR("syncScenesToRoomData: m_world is null!");
        return;
    }
    
    LOG_INFO("World has " + std::to_string(m_world->getLevels().size()) + " levels");
    
    // Get reference to room data in EditorContext
    auto& rooms = m_context.rooms;
    LOG_INFO("EditorContext has " + std::to_string(rooms.size()) + " rooms");
    
    // Sync each scene's grid position back to RoomData
    int syncedCount = 0;
    for (const auto& level : m_world->getLevels()) {
        LOG_INFO("Level '" + level->getName() + "' has " + std::to_string(level->getScenes().size()) + " scenes");
        
        for (const auto& scene : level->getScenes()) {
            const auto& gridPos = scene->getGridPosition();
            
            LOG_DEBUG("Scene '" + scene->getName() + "' (ID) gridPos: (" + 
                     std::to_string(gridPos.gridX) + "," + std::to_string(gridPos.gridY) + 
                     ") size: " + std::to_string(gridPos.pixelWidth) + "x" + std::to_string(gridPos.pixelHeight));
            
            // Find corresponding room by ID (not name)
            auto roomIt = std::find_if(rooms.begin(), rooms.end(),
                [&scene](const RoomData& room) {
                    return room.id == scene->getName();
                });
            
            if (roomIt != rooms.end()) {
                // Log old position
                if (roomIt->gridPosition) {
                    LOG_DEBUG("  Old RoomData gridPosition: (" + 
                             std::to_string(roomIt->gridPosition->gridX) + "," + 
                             std::to_string(roomIt->gridPosition->gridY) + ")");
                } else {
                    LOG_DEBUG("  Old RoomData gridPosition: null");
                }
                
                // Update grid position
                roomIt->gridPosition = engine::GridPosition{
                    gridPos.gridX, gridPos.gridY, gridPos.pixelWidth, gridPos.pixelHeight
                };
                
                LOG_DEBUG("  Updated RoomData gridPosition: (" + 
                         std::to_string(roomIt->gridPosition->gridX) + "," + 
                         std::to_string(roomIt->gridPosition->gridY) + ")");
                
                syncedCount++;
            } else {
                LOG_WARNING("No matching RoomData found for scene ID '" + scene->getName() + "'");
                
                // Debug: Show available room IDs
                LOG_DEBUG("Available room IDs:");
                for (const auto& room : rooms) {
                    LOG_DEBUG("  Room ID: '" + room.id + "' Name: '" + room.name + "'");
                }
            }
        }
    }
    
    LOG_INFO("EditorWorldManager: Synced " + std::to_string(syncedCount) + " scenes to RoomData");
    LOG_INFO("=== syncScenesToRoomData() END ===");
}

void EditorWorldManager::syncLevelsToWorldData() {
    LOG_INFO("=== syncLevelsToWorldData() START ===");
    
    if (!m_world) {
        LOG_ERROR("syncLevelsToWorldData: m_world is null!");
        return;
    }
    
    // Update WorldData from engine::World
    m_worldData.name = m_world->getName();
    
    // Sync World-level actors
    syncActorsFromContainer(m_world.get(), m_worldData.actors);
    
    // Update each level
    for (const auto& level : m_world->getLevels()) {
        // Find corresponding LevelData
        auto levelIt = std::find_if(m_worldData.levels.begin(), m_worldData.levels.end(),
            [&level](const editor::LevelData& ld) { return ld.id == level->getId(); });
        
        if (levelIt != m_worldData.levels.end()) {
            // Update existing level
            levelIt->name = level->getName();
            levelIt->description = level->getDescription();
            levelIt->gridPosition = level->getGridPosition();
            levelIt->musicTrack = level->getMusic();
            
            // Sync Level-level actors
            syncActorsFromContainer(level.get(), levelIt->actors);
            
            // Update sceneIds from actual scenes in level
            levelIt->sceneIds.clear();
            for (const auto& scene : level->getScenes()) {
                levelIt->sceneIds.push_back(scene->getName());
            }
            
            LOG_DEBUG("Updated Level '" + level->getName() + "' grid: (" +
                     std::to_string(level->getGridPosition().gridX) + "," +
                     std::to_string(level->getGridPosition().gridY) + ") actors: " +
                     std::to_string(levelIt->actors.size()));
        } else {
            // Create new LevelData
            editor::LevelData newLevel;
            newLevel.id = level->getId();
            newLevel.name = level->getName();
            newLevel.description = level->getDescription();
            newLevel.gridPosition = level->getGridPosition();
            newLevel.musicTrack = level->getMusic();
            
            // Sync actors
            syncActorsFromContainer(level.get(), newLevel.actors);
            
            for (const auto& scene : level->getScenes()) {
                newLevel.sceneIds.push_back(scene->getName());
            }
            
            m_worldData.levels.push_back(newLevel);
            LOG_DEBUG("Added new Level '" + level->getName() + "'");
        }
    }
    
    // Update active level/scene
    if (m_world->getActiveLevel()) {
        m_worldData.startLevelId = m_world->getActiveLevel()->getId();
        if (m_world->getActiveLevel()->getActiveScene()) {
            m_worldData.startSceneId = m_world->getActiveLevel()->getActiveScene()->getName();
        }
    }
    
    LOG_INFO("EditorWorldManager: Synced " + std::to_string(m_worldData.levels.size()) + " levels, " +
             std::to_string(m_worldData.actors.size()) + " world actors to WorldData");
    LOG_INFO("=== syncLevelsToWorldData() END ===");
}

void EditorWorldManager::saveWorldData() {
    LOG_INFO("=== saveWorldData() START ===");
    
    // First sync all data from engine
    syncLevelsToWorldData();
    
    // Write to file
    try {
        json j = m_worldData;
        std::ofstream file("assets/data/world.json");
        if (file.is_open()) {
            file << j.dump(2);
            file.close();
            LOG_INFO("Saved world.json: " + m_worldData.name + " with " + 
                     std::to_string(m_worldData.levels.size()) + " levels");
        } else {
            LOG_ERROR("Failed to open world.json for writing!");
        }
    } catch (const json::exception& e) {
        LOG_ERROR("Failed to serialize world data: " + std::string(e.what()));
    }
    
    LOG_INFO("=== saveWorldData() END ===");
}

void EditorWorldManager::syncActorsFromContainer(engine::WorldContainer* container, 
                                                  std::vector<editor::ActorData>& actorDataList) {
    actorDataList.clear();
    
    if (!container) return;
    
    for (const auto& actor : container->getActors()) {
        // Skip Background actors - they're handled separately
        if (actor->getName() == "Background") continue;
        
        editor::ActorData data;
        data.id = actor->getName();  // Use name as ID for now
        data.name = actor->getName();
        data.type = "sprite";  // Default type
        
        auto pos = actor->getPosition();
        data.x = pos.x;
        data.y = pos.y;
        data.width = 32.0f;   // Default
        data.height = 32.0f;  // Default
        
        actorDataList.push_back(data);
    }
}

void EditorWorldManager::loadActorsToContainer(engine::WorldContainer* container,
                                               const std::vector<editor::ActorData>& actorDataList) {
    if (!container) return;
    
    for (const auto& actorData : actorDataList) {
        // Create actor from data
        auto actor = std::make_unique<engine::ActorObjectExtended>(actorData.name);
        actor->setPosition({actorData.x, actorData.y});
        
        // Add sprite component if sprite is specified
        if (!actorData.sprite.empty()) {
            auto* spriteComp = actor->addComponent<engine::SpriteComponent>();
            SDL_Texture* texture = TextureManager::instance().load(actorData.sprite);
            if (texture) {
                spriteComp->setTexture(texture);
                spriteComp->setSize(static_cast<int>(actorData.width), 
                                   static_cast<int>(actorData.height));
            }
        }
        
        container->addActor(std::move(actor));
        LOG_DEBUG("Loaded actor '" + actorData.name + "' at (" + 
                 std::to_string(actorData.x) + "," + std::to_string(actorData.y) + ")");
    }
}

void EditorWorldManager::shutdown() {
    m_world.reset();
    m_layerManager.reset();
}

std::string EditorWorldManager::getActiveLevelId() const {
    if (m_world && m_world->getActiveLevel()) {
        return m_world->getActiveLevel()->getId();
    }
    // Return first level if no active level
    if (!m_worldData.levels.empty()) {
        return m_worldData.levels[0].id;
    }
    return "main_game";  // Default fallback
}

bool EditorWorldManager::addSceneToLevel(const std::string& sceneId, const std::string& levelId) {
    std::string targetLevelId = levelId.empty() ? getActiveLevelId() : levelId;
    
    LOG_INFO("[EditorWorldManager] Adding scene '" + sceneId + "' to level '" + targetLevelId + "'");
    
    // 1. Add to WorldData (for saving to world.json)
    bool foundLevel = false;
    for (auto& level : m_worldData.levels) {
        if (level.id == targetLevelId) {
            // Check if already exists
            auto it = std::find(level.sceneIds.begin(), level.sceneIds.end(), sceneId);
            if (it == level.sceneIds.end()) {
                level.sceneIds.push_back(sceneId);
                LOG_INFO("[EditorWorldManager] Added scene to WorldData level");
            } else {
                LOG_WARNING("[EditorWorldManager] Scene already in level");
            }
            foundLevel = true;
            break;
        }
    }
    
    if (!foundLevel) {
        LOG_ERROR("[EditorWorldManager] Level not found: " + targetLevelId);
        return false;
    }
    
    // 2. Create Scene in engine::Level if room data exists
    if (m_world) {
        for (auto& level : m_world->getLevels()) {
            if (level->getId() == targetLevelId) {
                // Find room data for this scene
                const auto& rooms = DataLoader::instance().getRooms();
                const RoomData* roomData = nullptr;
                for (const auto& room : rooms) {
                    if (room.id == sceneId) {
                        roomData = &room;
                        break;
                    }
                }
                
                // Calculate grid position (place next to existing scenes)
                int maxGridX = 0;
                for (const auto& existingScene : level->getScenes()) {
                    maxGridX = std::max(maxGridX, existingScene->getGridPosition().gridX + 1);
                }
                
                if (roomData) {
                    // Create scene from room data
                    auto scene = engine::RoomToSceneConverter::convert(*roomData, nullptr);
                    if (scene) {
                        scene->setId(sceneId);  // Set ID for lookup
                        scene->setGridPosition({maxGridX, 0, 640, 400});
                        level->addScene(std::move(scene));
                        LOG_INFO("[EditorWorldManager] Created engine::Scene for '" + sceneId + "' at grid (" + std::to_string(maxGridX) + ", 0)");
                    }
                } else {
                    // Create empty scene for new scenes without RoomData
                    auto scene = std::make_unique<engine::Scene>(sceneId);
                    scene->setId(sceneId);  // Set ID for lookup
                    scene->setGridPosition({maxGridX, 0, 640, 400});
                    level->addScene(std::move(scene));
                    LOG_INFO("[EditorWorldManager] Created empty engine::Scene for '" + sceneId + "' at grid (" + std::to_string(maxGridX) + ", 0)");
                }
                break;
            }
        }
    }
    
    // 3. Save world.json
    saveWorldData();
    
    return true;
}

void EditorWorldManager::refreshViewport() {
    // Mark context as dirty to trigger viewport refresh
    m_context.isDirty = true;
    LOG_INFO("[EditorWorldManager] Viewport refresh requested");
}
