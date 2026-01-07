/**
 * @file EditorWorldManager.cpp
 * @brief World manager implementation
 */
#include "EditorWorldManager.h"
#include "game/Game.h"
#include "editor/core/EditorContext.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/world/LayerManager.h"
#include "engine/data/DataLoader.h"
#include "engine/graphics/TextureManager.h"
#include "engine/components/SpriteComponent.h"
#include "engine/utils/Logger.h"

EditorWorldManager::EditorWorldManager(EditorContext& context)
    : m_context(context) {
}

void EditorWorldManager::initialize(Game* game) {
    // Initialize TextureManager
    auto& texMgr = TextureManager::instance();
    texMgr.init(game->getRenderer());
    
    // Load all game data
    auto& loader = DataLoader::instance();
    if (loader.getRooms().empty()) {
        loader.loadAll("assets/data/");
    }
    
    // Create World and convert rooms to scenes
    createWorldFromRoomData(game);
    
    LOG_INFO("EditorWorldManager: World created with " + 
             std::to_string(loader.getRooms().size()) + " scenes");
}

void EditorWorldManager::createWorldFromRoomData(Game* game) {
    m_world = std::make_unique<engine::World>();
    
    // Create a default level for all rooms
    auto defaultLevel = std::make_unique<engine::Level>("main_game");
    defaultLevel->setName("Main Game");
    defaultLevel->setDescription("All game scenes");
    
    // Convert each RoomData to Scene
    auto& loader = DataLoader::instance();
    const auto& rooms = loader.getRooms();
    int gridX = 0;
    int gridY = 0;
    
    for (const auto& roomData : rooms) {
        auto scene = engine::Scene::createFromData(roomData);
        
        // Load background if available
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
        
        // Assign grid position if not explicitly set
        if (!roomData.gridPosition) {
            scene->setGridPosition(gridX, gridY, 640, 400);
            gridX++;
            if (gridX >= 4) {  // 4 columns
                gridX = 0;
                gridY++;
            }
        }
        
        defaultLevel->addScene(std::move(scene));
    }
    
    m_world->addLevel(std::move(defaultLevel));
    m_world->setActiveLevel("main_game");
}

void EditorWorldManager::syncScenesToRoomData() {
    // TODO: Implement sync from Scene actors back to RoomData
    LOG_DEBUG("EditorWorldManager: syncScenesToRoomData() called");
}

void EditorWorldManager::shutdown() {
    m_world.reset();
    m_layerManager.reset();
}
