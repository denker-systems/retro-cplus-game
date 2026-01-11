/**
 * @file SceneSerializer.cpp
 * @brief Implementation of SceneSerializer
 */
#include "SceneSerializer.h"
#include "engine/utils/Logger.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/NPC3DActor.h"
#include "engine/actors/Character3DActor.h"
#include <fstream>
#include <algorithm>

namespace editor {

SceneSerializer::SceneSerializer() {
}

std::string SceneSerializer::getFilePath() const {
    return "data/scenes.json";
}

std::string SceneSerializer::getDataTypeName() const {
    return "Scenes";
}

SerializationResult SceneSerializer::load() {
    std::string content = readJsonFromFile(m_filePath);
    
    if (content.empty()) {
        LOG_WARNING("scenes.json not found or empty");
        return SerializationResult::Failure("File not found");
    }
    
    try {
        json j = json::parse(content);
        m_scenes = j["scenes"].get<std::vector<SceneData>>();
        m_dirty = false;
        
        LOG_INFO("Loaded scenes.json: " + std::to_string(m_scenes.size()) + " scenes");
        return SerializationResult::Success(static_cast<int>(m_scenes.size()));
        
    } catch (const json::exception& e) {
        LOG_ERROR("Failed to parse scenes.json: " + std::string(e.what()));
        return SerializationResult::Failure(e.what());
    }
}

SerializationResult SceneSerializer::save() {
    try {
        json j;
        j["scenes"] = m_scenes;
        std::string content = j.dump(2);
        
        if (writeJsonToFile(m_filePath, content)) {
            m_dirty = false;
            LOG_INFO("Saved scenes.json: " + std::to_string(m_scenes.size()) + " scenes");
            return SerializationResult::Success(static_cast<int>(m_scenes.size()));
        } else {
            return SerializationResult::Failure("Failed to write file");
        }
        
    } catch (const json::exception& e) {
        LOG_ERROR("Failed to serialize scene data: " + std::string(e.what()));
        return SerializationResult::Failure(e.what());
    }
}

void SceneSerializer::syncFromEngine(engine::Scene* scene) {
    if (!scene) return;
    
    // Find matching scene by name
    SceneData* sceneData = findSceneByName(scene->getName());
    if (!sceneData) {
        sceneData = findScene(scene->getName());  // Try by ID
    }
    
    if (!sceneData) {
        LOG_WARNING("No matching scene found for: " + scene->getName());
        return;
    }
    
    // Sync grid position
    sceneData->gridPosition = scene->getGridPosition();
    
    // Sync camera config
    sceneData->camera = scene->getCameraConfig();
    
    // Clear and rebuild actors array with ALL actors from engine
    sceneData->actors.clear();
    
    for (const auto& actor : scene->getActors()) {
        std::string actorName = actor->getName();
        
        // Skip Background actor (handled separately)
        if (actorName == "Background") continue;
        
        // Create SceneActorData for this actor
        SceneActorData actorData;
        actorData.id = actorName;
        actorData.name = actorName;
        
        // Determine actor type and extract 3D position
        if (auto* playerStart = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
            actorData.type = "PlayerStart";
            glm::vec3 pos = playerStart->getSpawnPosition();
            actorData.x = pos.x;
            actorData.y = pos.y;
            actorData.z = pos.z;
            
            // Also update legacy spawn position
            sceneData->playerSpawnX = pos.x * 100.0f;  // Convert back to pixels
            sceneData->playerSpawnY = pos.z * 100.0f;
        } else if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
            actorData.type = "NPC3D";
            glm::vec3 pos = npc->getPosition3D();
            actorData.x = pos.x;
            actorData.y = pos.y;
            actorData.z = pos.z;
            actorData.sprite = npc->getSpriteName();
            actorData.dialogId = npc->getDialogId();
            
            // Save interaction volume
            glm::vec3 vol = npc->getInteractionVolume();
            actorData.properties["interactVolumeX"] = std::to_string(vol.x);
            actorData.properties["interactVolumeY"] = std::to_string(vol.y);
            actorData.properties["interactVolumeZ"] = std::to_string(vol.z);
        } else if (auto* char3d = dynamic_cast<engine::Character3DActor*>(actor.get())) {
            actorData.type = "Character3D";
            glm::vec3 pos = char3d->getPosition3D();
            actorData.x = pos.x;
            actorData.y = pos.y;
            actorData.z = pos.z;
            actorData.rotationY = char3d->getYaw();
        } else {
            // Generic actor - use 2D position converted
            actorData.type = "Generic";
            engine::Vec2 pos = actor->getPosition();
            actorData.x = pos.x / 100.0f;
            actorData.y = 0.0f;
            actorData.z = pos.y / 100.0f;
        }
        
        sceneData->actors.push_back(actorData);
        LOG_DEBUG("Synced actor '" + actorName + "' type=" + actorData.type);
    }
    
    LOG_INFO("Synced " + std::to_string(sceneData->actors.size()) + " actors to scene '" + scene->getName() + "'");
    m_dirty = true;
}

void SceneSerializer::syncToEngine(engine::Scene* scene) {
    if (!scene) return;
    
    // Find matching scene
    const SceneData* sceneData = findSceneByName(scene->getName());
    if (!sceneData) {
        sceneData = findScene(scene->getName());
    }
    
    if (!sceneData) return;
    
    // Sync grid position
    if (sceneData->gridPosition) {
        scene->setGridPosition(*sceneData->gridPosition);
    }
    
    // Sync camera config
    if (sceneData->camera) {
        scene->setCameraConfig(*sceneData->camera);
    }
    
    // Note: Actor positions are set during scene creation from SceneData
}

void SceneSerializer::syncAllFromEngine(const std::vector<engine::Scene*>& scenes) {
    for (auto* scene : scenes) {
        syncFromEngine(scene);
    }
}

void SceneSerializer::syncAllToEngine(std::vector<engine::Scene*>& scenes) {
    for (auto* scene : scenes) {
        syncToEngine(scene);
    }
}

SceneData* SceneSerializer::findScene(const std::string& sceneId) {
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&sceneId](const SceneData& s) { return s.id == sceneId; });
    return (it != m_scenes.end()) ? &(*it) : nullptr;
}

SceneData* SceneSerializer::findSceneByName(const std::string& name) {
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&name](const SceneData& s) { return s.name == name; });
    return (it != m_scenes.end()) ? &(*it) : nullptr;
}

const SceneData* SceneSerializer::findScene(const std::string& sceneId) const {
    auto it = std::find_if(m_scenes.begin(), m_scenes.end(),
        [&sceneId](const SceneData& s) { return s.id == sceneId; });
    return (it != m_scenes.end()) ? &(*it) : nullptr;
}

void SceneSerializer::addScene(const SceneData& scene) {
    m_scenes.push_back(scene);
    m_dirty = true;
}

void SceneSerializer::removeScene(const std::string& sceneId) {
    m_scenes.erase(
        std::remove_if(m_scenes.begin(), m_scenes.end(),
            [&sceneId](const SceneData& s) { return s.id == sceneId; }),
        m_scenes.end()
    );
    m_dirty = true;
}

} // namespace editor
