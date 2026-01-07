/**
 * @file WorldSerializer.h
 * @brief Serializer for world.json - handles World/Level hierarchy
 */
#pragma once

#include "ISerializer.h"
#include "EditorDataTypes.h"
#include "engine/world/World.h"

namespace editor {

/**
 * @brief Serializes World hierarchy to/from world.json
 * 
 * Handles:
 * - Level definitions and positions
 * - Level→Scene relationships
 * - World metadata (name, start level, etc.)
 */
class WorldSerializer : public ISyncableSerializer<engine::World, WorldData> {
public:
    WorldSerializer();
    
    // ISerializer interface
    std::string getFilePath() const override;
    std::string getDataTypeName() const override;
    SerializationResult load() override;
    SerializationResult save() override;
    bool isDirty() const override { return m_dirty; }
    void markDirty() override { m_dirty = true; }
    void markClean() override { m_dirty = false; }
    
    // ISyncableSerializer interface
    void syncFromEngine(engine::World* world) override;
    void syncToEngine(engine::World* world) override;
    
    // Data access
    WorldData& getData() override { return m_data; }
    const WorldData& getData() const override { return m_data; }
    
    // Level-specific helpers
    LevelData* findLevel(const std::string& levelId);
    const LevelData* findLevel(const std::string& levelId) const;
    void addLevel(const LevelData& level);
    void removeLevel(const std::string& levelId);
    
private:
    WorldData m_data;
    std::string m_filePath = "assets/data/world.json";
    
    // Create default world if file doesn't exist
    void createDefaultWorld();
};

} // namespace editor
