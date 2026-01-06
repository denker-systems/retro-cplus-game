/**
 * @file EditorDataManager.cpp
 * @brief Implementation of EditorDataManager
 */
#include "EditorDataManager.h"
#include "engine/world/Level.h"
#include "engine/utils/Logger.h"
#include <fstream>

namespace editor {

EditorDataManager::EditorDataManager() {
}

EditorDataManager& EditorDataManager::instance() {
    static EditorDataManager instance;
    return instance;
}

BatchResult EditorDataManager::loadAll(ProgressCallback callback) {
    BatchResult result;
    auto serializers = getAllSerializers();
    float step = 1.0f / serializers.size();
    float progress = 0.0f;
    
    for (auto* serializer : serializers) {
        if (callback) {
            callback("Loading " + serializer->getDataTypeName() + "...", progress);
        }
        
        auto loadResult = serializer->load();
        if (loadResult.success) {
            result.addSuccess();
            LOG_INFO("Loaded " + serializer->getDataTypeName() + ": " + 
                    std::to_string(loadResult.itemCount) + " items");
        } else {
            result.addFailure(serializer->getDataTypeName() + ": " + loadResult.errorMessage);
        }
        
        progress += step;
    }
    
    if (callback) {
        callback("Load complete", 1.0f);
    }
    
    m_statusMessage = result.success ? 
        "Loaded " + std::to_string(result.successCount) + " data files" :
        "Load completed with " + std::to_string(result.failCount) + " errors";
    
    return result;
}

BatchResult EditorDataManager::saveAll(ProgressCallback callback) {
    BatchResult result;
    auto serializers = getAllSerializers();
    float step = 1.0f / serializers.size();
    float progress = 0.0f;
    
    for (auto* serializer : serializers) {
        // Only save if dirty
        if (!serializer->isDirty()) {
            progress += step;
            continue;
        }
        
        if (callback) {
            callback("Saving " + serializer->getDataTypeName() + "...", progress);
        }
        
        auto saveResult = serializer->save();
        if (saveResult.success) {
            result.addSuccess();
            LOG_INFO("Saved " + serializer->getDataTypeName() + ": " + 
                    std::to_string(saveResult.itemCount) + " items");
        } else {
            result.addFailure(serializer->getDataTypeName() + ": " + saveResult.errorMessage);
        }
        
        progress += step;
    }
    
    if (callback) {
        callback("Save complete", 1.0f);
    }
    
    m_statusMessage = result.success ? 
        "Saved " + std::to_string(result.successCount) + " data files" :
        "Save completed with " + std::to_string(result.failCount) + " errors";
    
    return result;
}

BatchResult EditorDataManager::saveAllForced(ProgressCallback callback) {
    // Mark all as dirty first
    for (auto* serializer : getAllSerializers()) {
        serializer->markDirty();
    }
    return saveAll(callback);
}

void EditorDataManager::syncFromEngine(engine::World* world) {
    if (!world) {
        LOG_WARNING("syncFromEngine: No world provided");
        return;
    }
    
    // Sync world data
    m_worldSerializer.syncFromEngine(world);
    
    // Collect all scenes and sync scene data
    std::vector<engine::Scene*> scenes;
    for (const auto& level : world->getLevels()) {
        for (const auto& scene : level->getScenes()) {
            scenes.push_back(scene.get());
        }
    }
    m_sceneSerializer.syncAllFromEngine(scenes);
    
    LOG_INFO("Synced data from engine: " + 
            std::to_string(world->getLevels().size()) + " levels, " +
            std::to_string(scenes.size()) + " scenes");
}

void EditorDataManager::syncToEngine(engine::World* world) {
    if (!world) {
        LOG_WARNING("syncToEngine: No world provided");
        return;
    }
    
    // Sync world data to engine
    m_worldSerializer.syncToEngine(world);
    
    // Sync scene data to scenes
    for (const auto& level : world->getLevels()) {
        for (const auto& scene : level->getScenes()) {
            m_sceneSerializer.syncToEngine(scene.get());
        }
    }
    
    LOG_INFO("Synced data to engine");
}

bool EditorDataManager::isDirty() const {
    for (auto* serializer : const_cast<EditorDataManager*>(this)->getAllSerializers()) {
        if (serializer->isDirty()) return true;
    }
    return false;
}

void EditorDataManager::markAllClean() {
    for (auto* serializer : getAllSerializers()) {
        serializer->markClean();
    }
}

void EditorDataManager::loadPreferences() {
    std::ifstream file("editor_preferences.json");
    if (file.is_open()) {
        try {
            json j = json::parse(file);
            m_preferences = j.get<EditorPreferences>();
            LOG_INFO("Loaded editor preferences");
        } catch (const json::exception& e) {
            LOG_WARNING("Failed to load preferences: " + std::string(e.what()));
        }
    }
}

void EditorDataManager::savePreferences() {
    std::ofstream file("editor_preferences.json");
    if (file.is_open()) {
        json j = m_preferences;
        file << j.dump(2);
        LOG_INFO("Saved editor preferences");
    }
}

std::vector<ISerializer*> EditorDataManager::getAllSerializers() {
    return {
        &m_worldSerializer,
        &m_sceneSerializer
        // Add more serializers here as needed:
        // &m_dialogSerializer,
        // &m_questSerializer,
        // &m_itemSerializer,
        // &m_npcSerializer
    };
}

} // namespace editor
