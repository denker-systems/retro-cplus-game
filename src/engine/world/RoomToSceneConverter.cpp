/**
 * @file RoomToSceneConverter.cpp
 * @brief Converts SceneData to Scene hierarchy with SpriteActors
 */
#include "RoomToSceneConverter.h"
#include "Scene.h"
#include "engine/actors/SpriteActor.h"
#include "engine/actors/EnvironmentActor.h"
#include "engine/actors/InteractiveActor.h"
#include "engine/graphics/TextureManager.h"

namespace engine {

std::unique_ptr<Scene> RoomToSceneConverter::convert(const SceneData& sceneData, SDL_Renderer* renderer) {
    auto scene = std::make_unique<Scene>(sceneData.id);
    scene->setName(sceneData.name);
    
    // Copy grid position if available
    if (sceneData.gridPosition) {
        scene->setGridPosition(*sceneData.gridPosition);
    } else {
        // Default grid position based on scene index (will be overwritten later if needed)
        scene->setGridPosition(0, 0, 640, 400);
    }
    
    // Copy camera config if available
    if (sceneData.camera) {
        scene->setCameraConfig(*sceneData.camera);
    }
    
    // Convert background to SpriteActor
    if (!sceneData.background.empty()) {
        auto bgActor = std::make_unique<SpriteActor>("Background");
        bgActor->setPosition(0, 0);
        
        // Load background texture
        std::string path = sceneData.background;  // Background already contains full path
        bgActor->initializeSprite(path);
        
        scene->addActor(std::move(bgActor));
    }
    
    // Convert hotspots to InteractiveActors
    for (const auto& hsData : sceneData.hotspots) {
        auto hotspotActor = std::make_unique<InteractiveActor>(hsData.name);
        hotspotActor->setPosition(hsData.x, hsData.y);
        
        // Store interaction data
        hotspotActor->setInteractionText(hsData.name);
        
        scene->addActor(std::move(hotspotActor));
    }
    
    // Add player spawn marker (simple prop for now)
    auto spawnActor = std::make_unique<PropActor>("PlayerSpawn");
    spawnActor->setPosition(sceneData.playerSpawnX, sceneData.playerSpawnY);
    scene->addActor(std::move(spawnActor));
    
    // Add walk area marker
    auto walkAreaActor = std::make_unique<PropActor>("WalkArea");
    scene->addActor(std::move(walkAreaActor));
    
    return scene;
}

} // namespace engine
