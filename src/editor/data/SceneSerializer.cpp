/**
 * @file SceneSerializer.cpp
 * @brief Implementation of SceneSerializer
 */
#include "SceneSerializer.h"
#include "engine/utils/Logger.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/actors/PlayerStartActor.h"
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
    
    // Sync actor positions
    for (const auto& actor : scene->getActors()) {
        std::string actorName = actor->getName();
        
        // Save PlayerStart position (3D position)
        if (actorName == "PlayerStart") {
            // Cast to PlayerStartActor to get spawn position
            if (auto* playerStart = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
                glm::vec3 spawnPos = playerStart->getSpawnPosition();
                sceneData->playerSpawnX = spawnPos.x;
                sceneData->playerSpawnY = spawnPos.z;  // Store Z as Y for 3D
                LOG_DEBUG("Synced PlayerStart spawn position: (" + 
                         std::to_string(spawnPos.x) + ", " + 
                         std::to_string(spawnPos.y) + ", " + 
                         std::to_string(spawnPos.z) + ")");
            }
            continue;
        }
        
        // Save PlayerConfig position
        if (actorName == "PlayerConfig") {
            engine::Vec2 pos = actor->getPosition();
            LOG_DEBUG("Synced PlayerConfig position: (" + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ")");
            continue;
        }
        
        // Find matching hotspot for other actors
        engine::Vec2 pos = actor->getPosition();
        for (auto& hs : sceneData->hotspots) {
            if (hs.name == actorName || hs.id == actorName) {
                hs.x = static_cast<int>(pos.x);
                hs.y = static_cast<int>(pos.y);
                LOG_DEBUG("Synced hotspot '" + actorName + "' position: (" + std::to_string(hs.x) + ", " + std::to_string(hs.y) + ")");
                break;
            }
        }
    }
    
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
