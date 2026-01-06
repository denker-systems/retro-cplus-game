/**
 * @file Scene.cpp
 * @brief Implementation of Scene (UE5-style)
 */
#include "Scene.h"
#include "engine/data/GameData.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/SpriteComponent.h"
#include <SDL_image.h>

namespace engine {

// Constructors now in header

void Scene::update(float deltaTime) {
    if (m_isPaused) return;
    
    // Update camera actor
    if (m_cameraActor && m_cameraActor->isActive()) {
        m_cameraActor->update(deltaTime);
    }
    
    // Update all actors
    for (auto& actor : m_actors) {
        if (actor->isActive()) {
            actor->update(deltaTime);
        }
    }
}

void Scene::renderActors(SDL_Renderer* renderer) {
    if (!renderer) return;
    
    // Render all actors
    for (const auto& actor : m_actors) {
        if (actor && actor->isActive()) {
            actor->render(renderer);
        }
    }
}

CameraComponent* Scene::createDefaultCamera() {
    // Create camera actor
    m_cameraActor = std::make_unique<ActorObjectExtended>("CameraActor");
    
    // Create and attach camera component using template syntax
    m_activeCamera = m_cameraActor->addComponent<CameraComponent>("MainCamera");
    
    return m_activeCamera;
}

void Scene::onSceneEnter() {
    // Override in subclass
}

void Scene::onSceneExit() {
    // Override in subclass
}

void Scene::onScenePause() {
    m_isPaused = true;
}

void Scene::onSceneResume() {
    m_isPaused = false;
}

std::unique_ptr<Scene> Scene::createFromData(const SceneData& data) {
    auto scene = std::make_unique<Scene>(data.name);
    
    // Set grid position if available
    if (data.gridPosition) {
        scene->setGridPosition(*data.gridPosition);
    }
    
    // Set camera config if available
    if (data.camera) {
        scene->setCameraConfig(*data.camera);
    }
    
    // Load background if available
    if (!data.background.empty()) {
        // Note: We can't load texture here without renderer
        // This will be handled by GameDataLoader
    }
    
    // Create actors for hotspots
    for (const auto& hs : data.hotspots) {
        auto actor = std::make_unique<ActorObjectExtended>(hs.name);
        actor->setPosition(static_cast<float>(hs.x), static_cast<float>(hs.y));
        scene->addActor(std::move(actor));
    }
    
    // Create actor for player spawn
    auto spawnActor = std::make_unique<ActorObjectExtended>("PlayerSpawn");
    spawnActor->setPosition(static_cast<float>(data.playerSpawnX), static_cast<float>(data.playerSpawnY));
    scene->addActor(std::move(spawnActor));
    
    // Create actor for background
    auto bgActor = std::make_unique<ActorObjectExtended>("Background");
    bgActor->setPosition(0.0f, 0.0f);
    // Add sprite component for background if we have a path
    if (!data.background.empty()) {
        auto spriteComp = bgActor->addComponent<SpriteComponent>();
        // Note: Texture will be loaded later when renderer is available
    }
    scene->addActor(std::move(bgActor));
    
    // Also populate legacy data for game compatibility
    scene->m_legacyWalkArea = {data.walkArea.minX, data.walkArea.maxX, data.walkArea.minY, data.walkArea.maxY, data.walkArea.scaleTop, data.walkArea.scaleBottom};
    // Convert HotspotData to Hotspot for legacy compatibility
    for (const auto& hsData : data.hotspots) {
        Hotspot hs;
        hs.id = hsData.id;
        hs.name = hsData.name;
        hs.rect = {hsData.x, hsData.y, hsData.w, hsData.h};
        // Convert string type to enum
        if (hsData.type == "NPC") hs.type = HotspotType::NPC;
        else if (hsData.type == "Item") hs.type = HotspotType::Item;
        else if (hsData.type == "Exit") hs.type = HotspotType::Exit;
        else if (hsData.type == "Examine") hs.type = HotspotType::Examine;
        else hs.type = HotspotType::None;
        hs.dialogId = hsData.dialogId;
        hs.examineText = hsData.examineText;
        hs.funnyFails = hsData.funnyFails;
        hs.targetRoom = hsData.targetScene;  // Map targetScene to targetRoom
        scene->m_legacyHotspots.push_back(hs);
    }
    
    return scene;
}

// Legacy methods from old Scene - for game compatibility
void Scene::render(SDL_Renderer* renderer) {
    // Render background if loaded
    if (m_backgroundTexture) {
        SDL_Rect dest = {0, 0, 640, 400};
        SDL_RenderCopy(renderer, m_backgroundTexture, nullptr, &dest);
    }
    
    // Render all actors (including background actor)
    renderActors(renderer);
    
    // Render NPCs
    renderNPCs(renderer);
}

bool Scene::loadBackground(SDL_Renderer* renderer, const std::string& path) {
    if (m_backgroundTexture) {
        SDL_DestroyTexture(m_backgroundTexture);
    }
    m_backgroundTexture = IMG_LoadTexture(renderer, path.c_str());
    return m_backgroundTexture != nullptr;
}

bool Scene::loadLayer(SDL_Renderer* renderer, const std::string& imagePath, int zIndex, 
                     float parallaxX, float parallaxY, float opacity) {
    // For now, just load as background if zIndex is -1
    if (zIndex == -1) {
        return loadBackground(renderer, imagePath);
    }
    // TODO: Implement proper layer system
    return true;
}

void Scene::addHotspot(const std::string& id, const std::string& name, 
                      int x, int y, int w, int h, HotspotType type,
                      const std::string& dialogId,
                      const std::string& examineText,
                      const std::vector<std::string>& funnyFails) {
    Hotspot hs;
    hs.id = id;
    hs.name = name;
    hs.rect = {x, y, w, h};
    hs.type = type;
    hs.dialogId = dialogId;
    hs.examineText = examineText;
    hs.funnyFails = funnyFails;
    m_legacyHotspots.push_back(hs);
}

void Scene::addExit(const std::string& id, const std::string& name,
                   int x, int y, int w, int h, const std::string& targetRoom) {
    addHotspot(id, name, x, y, w, h, HotspotType::Exit, "", "", {});
    m_legacyHotspots.back().targetRoom = targetRoom;
}

void Scene::setWalkArea(int minX, int maxX, int minY, int maxY, float scaleTop, float scaleBottom) {
    m_legacyWalkArea.minX = minX;
    m_legacyWalkArea.maxX = maxX;
    m_legacyWalkArea.minY = minY;
    m_legacyWalkArea.maxY = maxY;
    m_legacyWalkArea.scaleTop = scaleTop;
    m_legacyWalkArea.scaleBottom = scaleBottom;
}

Hotspot* Scene::getHotspotAt(int x, int y) {
    for (auto& hs : m_legacyHotspots) {
        if (x >= hs.rect.x && x < hs.rect.x + hs.rect.w && 
            y >= hs.rect.y && y < hs.rect.y + hs.rect.h) {
            return &hs;
        }
    }
    return nullptr;
}

void Scene::addNPC(std::unique_ptr<engine::actors::NPC> npc) {
    m_npcs.push_back(std::move(npc));
}

void Scene::updateNPCs(float deltaTime) {
    for (auto& npc : m_npcs) {
        if (npc) {
            npc->update(deltaTime);
        }
    }
}

void Scene::renderNPCs(SDL_Renderer* renderer) {
    for (auto& npc : m_npcs) {
        if (npc) {
            npc->render(renderer);
        }
    }
}

void Scene::getPlayerSpawn(float& x, float& y) const {
    // Find PlayerSpawn actor
    for (const auto& actor : getActors()) {
        if (actor && actor->getName() == "PlayerSpawn") {
            auto pos = actor->getPosition();
            x = pos.x;
            y = pos.y;
            return;
        }
    }
    // Default fallback
    x = 320.0f;
    y = 240.0f;
}

void Scene::setPlayerSpawn(float x, float y) {
    // Find and update PlayerSpawn actor
    for (const auto& actor : getActors()) {
        if (actor && actor->getName() == "PlayerSpawn") {
            actor->setPosition(x, y);
            return;
        }
    }
    // Create if not found
    auto spawnActor = std::make_unique<ActorObjectExtended>("PlayerSpawn");
    spawnActor->setPosition(x, y);
    addActor(std::move(spawnActor));
}

} // namespace engine
