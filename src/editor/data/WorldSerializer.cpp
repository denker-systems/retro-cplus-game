/**
 * @file WorldSerializer.cpp
 * @brief Implementation of WorldSerializer
 */
#include "WorldSerializer.h"
#include "engine/world/Level.h"
#include "engine/utils/Logger.h"
#include <fstream>
#include <algorithm>

namespace editor {

WorldSerializer::WorldSerializer() {
    // Initialize with empty data
}

std::string WorldSerializer::getFilePath() const {
    return m_filePath;
}

std::string WorldSerializer::getDataTypeName() const {
    return "World";
}

SerializationResult WorldSerializer::load() {
    std::string content = readJsonFromFile(m_filePath);
    
    if (content.empty()) {
        // File doesn't exist - create default world
        LOG_INFO("world.json not found, creating default world");
        createDefaultWorld();
        return SerializationResult::Success(static_cast<int>(m_data.levels.size()));
    }
    
    try {
        json j = json::parse(content);
        m_data = j.get<WorldData>();
        m_dirty = false;
        
        LOG_INFO("Loaded world.json: " + std::to_string(m_data.levels.size()) + " levels");
        return SerializationResult::Success(static_cast<int>(m_data.levels.size()));
        
    } catch (const json::exception& e) {
        LOG_ERROR("Failed to parse world.json: " + std::string(e.what()));
        return SerializationResult::Failure(e.what());
    }
}

SerializationResult WorldSerializer::save() {
    try {
        json j = m_data;
        std::string content = j.dump(2);
        
        if (writeJsonToFile(m_filePath, content)) {
            m_dirty = false;
            LOG_INFO("Saved world.json: " + std::to_string(m_data.levels.size()) + " levels");
            return SerializationResult::Success(static_cast<int>(m_data.levels.size()));
        } else {
            return SerializationResult::Failure("Failed to write file");
        }
        
    } catch (const json::exception& e) {
        LOG_ERROR("Failed to serialize world data: " + std::string(e.what()));
        return SerializationResult::Failure(e.what());
    }
}

void WorldSerializer::syncFromEngine(engine::World* world) {
    if (!world) return;
    
    m_data.name = world->getName();
    m_data.levels.clear();
    
    for (const auto& level : world->getLevels()) {
        LevelData levelData;
        levelData.id = level->getId();
        levelData.name = level->getName();
        levelData.description = level->getDescription();
        levelData.gridPosition = level->getGridPosition();
        
        // Collect scene IDs
        for (const auto& scene : level->getScenes()) {
            levelData.sceneIds.push_back(scene->getName());
        }
        
        m_data.levels.push_back(levelData);
    }
    
    // Track active level/scene
    if (world->getActiveLevel()) {
        m_data.startLevelId = world->getActiveLevel()->getId();
        if (world->getActiveLevel()->getActiveScene()) {
            m_data.startSceneId = world->getActiveLevel()->getActiveScene()->getName();
        }
    }
    
    m_dirty = true;
    LOG_INFO("Synced " + std::to_string(m_data.levels.size()) + " levels from engine");
}

void WorldSerializer::syncToEngine(engine::World* world) {
    if (!world) return;
    
    world->setName(m_data.name);
    
    // Update existing levels with data from WorldData
    for (const auto& levelData : m_data.levels) {
        engine::Level* level = world->getLevel(levelData.id);
        if (level) {
            level->setName(levelData.name);
            level->setDescription(levelData.description);
            level->setGridPosition(levelData.gridPosition);
        }
    }
    
    LOG_INFO("Synced world data to engine");
}

LevelData* WorldSerializer::findLevel(const std::string& levelId) {
    auto it = std::find_if(m_data.levels.begin(), m_data.levels.end(),
        [&levelId](const LevelData& l) { return l.id == levelId; });
    return (it != m_data.levels.end()) ? &(*it) : nullptr;
}

const LevelData* WorldSerializer::findLevel(const std::string& levelId) const {
    auto it = std::find_if(m_data.levels.begin(), m_data.levels.end(),
        [&levelId](const LevelData& l) { return l.id == levelId; });
    return (it != m_data.levels.end()) ? &(*it) : nullptr;
}

void WorldSerializer::addLevel(const LevelData& level) {
    m_data.levels.push_back(level);
    m_dirty = true;
}

void WorldSerializer::removeLevel(const std::string& levelId) {
    m_data.levels.erase(
        std::remove_if(m_data.levels.begin(), m_data.levels.end(),
            [&levelId](const LevelData& l) { return l.id == levelId; }),
        m_data.levels.end()
    );
    m_dirty = true;
}

void WorldSerializer::createDefaultWorld() {
    m_data.name = "Retro Adventure";
    m_data.version = "1.0";
    m_data.startLevelId = "main_game";
    m_data.startSceneId = "tavern";
    
    // Create default level with all existing rooms
    LevelData mainLevel;
    mainLevel.id = "main_game";
    mainLevel.name = "Main Game";
    mainLevel.description = "The main game world";
    mainLevel.gridPosition = {0, 0, 800, 600};
    
    // These will be populated when syncing from engine
    m_data.levels.push_back(mainLevel);
    
    m_dirty = true;
}

} // namespace editor
