/**
 * @file Scene.cpp
 * @brief Implementation of Scene (UE5-style)
 */
#include "Scene.h"
#include "engine/actors/CharacterActor.h"
#include "engine/data/GameData.h"
#include "engine/data/DataLoader.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/SpriteComponent.h"
#include <SDL_image.h>
#include <iostream>

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
            // Special handling for Background - fill entire viewport like legacy Room::render()
            if (actor->getName() == "Background") {
                auto* spriteComp = actor->getComponent<SpriteComponent>();
                if (spriteComp && spriteComp->getTexture()) {
                    // Use nullptr for dest to fill entire render target (like legacy Room)
                    SDL_RenderCopy(renderer, spriteComp->getTexture(), nullptr, nullptr);
                    continue;
                }
            }
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
    // Use data.id as scene name for lookups (e.g. "tavern")
    // data.name is display name (e.g. "The Rusty Anchor")
    auto scene = std::make_unique<Scene>(data.id);
    
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
        scene->m_backgroundPath = data.background;
        // Note: We can't load texture here without renderer
        // This will be handled by GameDataLoader
    }
    
    // Create actor for background FIRST so it renders behind everything
    auto bgActor = std::make_unique<ActorObjectExtended>("Background");
    bgActor->setPosition(0.0f, 0.0f);
    std::cout << "[DEBUG] Creating Background actor for scene: " << data.name << std::endl;
    // Add sprite component for background if we have a path
    if (!data.background.empty()) {
        auto spriteComp = bgActor->addComponent<SpriteComponent>();
        std::cout << "[DEBUG] Added SpriteComponent to Background actor" << std::endl;
        // Note: Texture will be loaded later when renderer is available
    }
    scene->addActor(std::move(bgActor));
    std::cout << "[DEBUG] Added Background actor to scene" << std::endl;
    
    // Create actors for hotspots (skip NPCs - they're created separately)
    for (const auto& hs : data.hotspots) {
        // Skip NPC hotspots - NPCActors are created from NPC data
        // Note: type in JSON is lowercase "npc"
        if (hs.type != "npc" && hs.type != "NPC") {
            auto actor = std::make_unique<ActorObjectExtended>(hs.name);
            actor->setPosition(static_cast<float>(hs.x), static_cast<float>(hs.y));
            scene->addActor(std::move(actor));
        }
    }
    
    // Create actor for player spawn
    auto spawnActor = std::make_unique<ActorObjectExtended>("PlayerSpawn");
    spawnActor->setPosition(static_cast<float>(data.playerSpawnX), static_cast<float>(data.playerSpawnY));
    scene->addActor(std::move(spawnActor));
    
    // Create NPCActors from NPC data
    const auto& npcs = DataLoader::instance().getNPCs();
    std::cout << "[DEBUG] Checking NPCs for scene ID: " << data.id << std::endl;
    for (const auto& npcData : npcs) {
        std::cout << "[DEBUG] NPC " << npcData.name << " is in room: " << npcData.room << std::endl;
        if (npcData.room == data.id) {  // Match against data.id (e.g. "tavern")
            std::cout << "[DEBUG] Creating NPCActor: " << npcData.name << std::endl;
            auto npcActor = std::make_unique<engine::NPCActor>(npcData.name, npcData.sprite);
            npcActor->setPosition(static_cast<float>(npcData.x), static_cast<float>(npcData.y));
            
            // Set speed
            npcActor->setSpeed(npcData.moveSpeed);
            
            // Set dialog
            if (!npcData.dialogId.empty()) {
                npcActor->setDialogId(npcData.dialogId);
            }
            
            // Set interaction text
            npcActor->setInteractionText("Prata med " + npcData.name);
            
            scene->addActor(std::move(npcActor));
        }
    }
    
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
    // Render all actors (including background and NPCs)
    renderActors(renderer);
}

bool Scene::loadLayer(SDL_Renderer* renderer, const std::string& imagePath, int zIndex, 
                     float parallaxX, float parallaxY, float opacity) {
    // TODO: Implement proper layer system with actors
    // For now, do nothing - layers should be created as actors
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
