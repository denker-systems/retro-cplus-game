/**
 * @file RoomToSceneConverter.cpp
 * @brief Converts legacy RoomData to Scene hierarchy
 */
#include "RoomToSceneConverter.h"
#include "engine/nodes/Sprite.h"
#include "engine/nodes/InteractiveArea.h"
#include "engine/nodes/WalkArea.h"
#include "engine/nodes/Marker.h"
#include "engine/graphics/TextureManager.h"

namespace engine {

std::unique_ptr<Scene> RoomToSceneConverter::convert(const RoomData& roomData, SDL_Renderer* renderer) {
    auto scene = std::make_unique<Scene>(roomData.id);
    
    // Convert background to VisualActor
    if (!roomData.background.empty()) {
        auto bgActor = std::make_unique<ActorObjectExtended>("Background");
        // TODO: Add SpriteComponent with background texture
        scene->addActor(std::move(bgActor));
    }
    
    // Convert hotspots to InteractiveActors
    for (const auto& hsData : roomData.hotspots) {
        auto hotspotActor = std::make_unique<ActorObjectExtended>(hsData.name);
        hotspotActor->setPosition(hsData.x, hsData.y);
        // TODO: Add InteractionComponent
        scene->addActor(std::move(hotspotActor));
    }
    
    // Add player spawn marker
    auto spawnActor = std::make_unique<ActorObjectExtended>("PlayerSpawn");
    spawnActor->setPosition(roomData.playerSpawnX, roomData.playerSpawnY);
    scene->addActor(std::move(spawnActor));
    
    // Add walk area marker
    auto walkAreaActor = std::make_unique<ActorObjectExtended>("WalkArea");
    scene->addActor(std::move(walkAreaActor));
    
    return scene;
}

std::unique_ptr<Node> RoomToSceneConverter::createBackground(const RoomData& roomData, SDL_Renderer* renderer) {
    if (!renderer) return nullptr;
    
    auto& texMgr = TextureManager::instance();
    std::string path = "assets/backgrounds/" + roomData.background;
    SDL_Texture* tex = texMgr.load(path);
    
    if (!tex) return nullptr;
    
    auto sprite = std::make_unique<Sprite>("Background");
    sprite->setTexture(tex);
    sprite->setPosition(0, 0);
    sprite->setZIndex(-100);
    
    return sprite;
}

std::unique_ptr<Node> RoomToSceneConverter::createWalkArea(const RoomData& roomData) {
    auto walkArea = std::make_unique<WalkArea>("WalkArea");
    walkArea->setBounds(roomData.walkArea);
    walkArea->setZIndex(-50);
    
    return walkArea;
}

std::vector<std::unique_ptr<Node>> RoomToSceneConverter::createHotspots(const RoomData& roomData) {
    std::vector<std::unique_ptr<Node>> hotspots;
    
    for (const auto& hsData : roomData.hotspots) {
        auto hotspot = std::make_unique<InteractiveArea>(
            hsData.name,
            static_cast<float>(hsData.x),
            static_cast<float>(hsData.y),
            static_cast<float>(hsData.w),
            static_cast<float>(hsData.h)
        );
        
        // TODO: Map HotspotData fields to InteractiveArea when available
        // hotspot->setAction(hsData.action);
        // hotspot->setHoverText(hsData.hoverText);
        // hotspot->setCursor(hsData.cursor);
        hotspot->setZIndex(0);
        
        hotspots.push_back(std::move(hotspot));
    }
    
    return hotspots;
}

std::unique_ptr<Node> RoomToSceneConverter::createPlayerSpawn(const RoomData& roomData) {
    auto spawn = std::make_unique<Marker>("PlayerSpawn", Marker::MarkerType::PlayerSpawn);
    spawn->setPosition(roomData.playerSpawnX, roomData.playerSpawnY);
    spawn->setZIndex(100);
    
    return spawn;
}

std::unique_ptr<Node> RoomToSceneConverter::createContainer(const std::string& name) {
    return std::make_unique<Node>(name);
}

} // namespace engine
