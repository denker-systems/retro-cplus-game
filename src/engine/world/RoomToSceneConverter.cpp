/**
 * @file RoomToSceneConverter.cpp
 * @brief Converts legacy RoomData to Scene hierarchy with SpriteActors
 */
#include "RoomToSceneConverter.h"
#include "Scene.h"
#include "engine/actors/SpriteActor.h"
#include "engine/actors/EnvironmentActor.h"
#include "engine/actors/InteractiveActor.h"
#include "engine/graphics/TextureManager.h"

namespace engine {

std::unique_ptr<Scene> RoomToSceneConverter::convert(const RoomData& roomData, SDL_Renderer* renderer) {
    auto scene = std::make_unique<Scene>(roomData.id);
    
    // Convert background to SpriteActor
    if (!roomData.background.empty()) {
        auto bgActor = std::make_unique<SpriteActor>("Background");
        bgActor->setPosition(0, 0);
        
        // Load background texture
        std::string path = roomData.background;  // Background already contains full path
        bgActor->initializeSprite(path);
        
        scene->addActor(std::move(bgActor));
    }
    
    // Convert hotspots to InteractiveActors
    for (const auto& hsData : roomData.hotspots) {
        auto hotspotActor = std::make_unique<InteractiveActor>(hsData.name);
        hotspotActor->setPosition(hsData.x, hsData.y);
        
        // Store interaction data
        hotspotActor->setInteractionText(hsData.name);
        
        scene->addActor(std::move(hotspotActor));
    }
    
    // Add player spawn marker (simple prop for now)
    auto spawnActor = std::make_unique<PropActor>("PlayerSpawn");
    spawnActor->setPosition(roomData.playerSpawnX, roomData.playerSpawnY);
    scene->addActor(std::move(spawnActor));
    
    // Add walk area marker
    auto walkAreaActor = std::make_unique<PropActor>("WalkArea");
    scene->addActor(std::move(walkAreaActor));
    
    return scene;
}

} // namespace engine
