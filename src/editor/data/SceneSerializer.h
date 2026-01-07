/**
 * @file SceneSerializer.h
 * @brief Serializer for scenes.json - handles SceneData serialization
 */
#pragma once

#include "ISerializer.h"
#include "engine/data/GameData.h"
#include "engine/world/Scene.h"
#include <vector>

namespace editor {

/**
 * @brief Serializes Scene data to/from scenes.json
 * 
 * Handles:
 * - Scene definitions (background, walk area, spawn)
 * - Hotspot data (positions, interactions)
 * - Grid positions for spatial view
 * - Camera configurations
 */
class SceneSerializer : public ISyncableSerializer<engine::Scene, std::vector<SceneData>> {
public:
    SceneSerializer();
    
    // ISerializer interface
    std::string getFilePath() const override;
    std::string getDataTypeName() const override;
    SerializationResult load() override;
    SerializationResult save() override;
    bool isDirty() const override { return m_dirty; }
    void markDirty() override { m_dirty = true; }
    void markClean() override { m_dirty = false; }
    
    // ISyncableSerializer interface - operates on single scene
    void syncFromEngine(engine::Scene* scene) override;
    void syncToEngine(engine::Scene* scene) override;
    
    // Batch operations for all scenes
    void syncAllFromEngine(const std::vector<engine::Scene*>& scenes);
    void syncAllToEngine(std::vector<engine::Scene*>& scenes);
    
    // Data access
    std::vector<SceneData>& getData() override { return m_scenes; }
    const std::vector<SceneData>& getData() const override { return m_scenes; }
    
    // Scene-specific helpers
    SceneData* findScene(const std::string& sceneId);
    SceneData* findSceneByName(const std::string& name);
    const SceneData* findScene(const std::string& sceneId) const;
    void addScene(const SceneData& scene);
    void removeScene(const std::string& sceneId);
    
    // Legacy aliases for backward compatibility
    RoomData* findRoom(const std::string& roomId) { return findScene(roomId); }
    RoomData* findRoomByName(const std::string& name) { return findSceneByName(name); }
    const RoomData* findRoom(const std::string& roomId) const { return findScene(roomId); }
    void addRoom(const RoomData& room) { addScene(room); }
    void removeRoom(const std::string& roomId) { removeScene(roomId); }
    
private:
    std::vector<SceneData> m_scenes;
    std::string m_filePath = "assets/data/rooms.json";  // Still rooms.json for compatibility
};

} // namespace editor
