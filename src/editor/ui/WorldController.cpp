/**
 * @file WorldController.cpp
 * @brief Implementation of World/Level/Scene management for editor
 */
#include "WorldController.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/world/LayerManager.h"
#include "engine/data/DataLoader.h"
#include "engine/graphics/TextureManager.h"
#include "engine/components/SpriteComponent.h"
#include "engine/utils/Logger.h"

WorldController::WorldController() = default;
WorldController::~WorldController() = default;

void WorldController::initialize(SDL_Renderer* renderer) {
    // Initialize TextureManager
    auto& texMgr = TextureManager::instance();
    texMgr.init(renderer);
    
    // Load all game data
    auto& loader = DataLoader::instance();
    if (loader.getRooms().empty()) {
        loader.loadAll("assets/data/");
    }
    
    // Create World and load rooms as scenes
    m_world = std::make_unique<engine::World>();
    loadRoomsAsScenes(renderer);
    
    LOG_INFO("WorldController initialized with " + std::to_string(getSceneCount()) + " scenes");
}

void WorldController::loadRoomsAsScenes(SDL_Renderer* renderer) {
    auto& loader = DataLoader::instance();
    const auto& rooms = loader.getRooms();
    
    // Create default level for all rooms
    auto defaultLevel = std::make_unique<engine::Level>("main_game");
    defaultLevel->setName("Main Game");
    defaultLevel->setDescription("All game scenes");
    
    int gridX = 0;
    int gridY = 0;
    
    for (const auto& roomData : rooms) {
        auto scene = engine::Scene::createFromData(roomData);
        
        // Setup background texture
        setupBackgroundTexture(scene.get(), roomData, renderer);
        
        // Assign grid position if not set
        if (!roomData.gridPosition) {
            scene->setGridPosition(gridX, gridY, 640, 400);
            gridX++;
            if (gridX >= 4) {
                gridX = 0;
                gridY++;
            }
        }
        
        defaultLevel->addScene(std::move(scene));
    }
    
    m_world->addLevel(std::move(defaultLevel));
    m_world->setActiveLevel("main_game");
}

void WorldController::setupBackgroundTexture(engine::Scene* scene, const RoomData& roomData, SDL_Renderer* renderer) {
    if (roomData.background.empty()) return;
    
    auto* bgActor = scene->findActor("Background");
    if (!bgActor) return;
    
    auto* spriteComp = bgActor->getComponent<engine::SpriteComponent>();
    if (!spriteComp) return;
    
    SDL_Texture* texture = TextureManager::instance().load(roomData.background);
    if (texture) {
        spriteComp->setTexture(texture);
        spriteComp->setSize(640, 400);
        spriteComp->setSourceRect({0, 0, 640, 400});
    }
}

engine::Level* WorldController::getActiveLevel() {
    return m_world ? m_world->getActiveLevel() : nullptr;
}

engine::Scene* WorldController::getActiveScene() {
    auto* level = getActiveLevel();
    return level ? level->getActiveScene() : nullptr;
}

size_t WorldController::getSceneCount() const {
    if (!m_world) return 0;
    
    size_t count = 0;
    for (const auto& level : m_world->getLevels()) {
        count += level->getScenes().size();
    }
    return count;
}
