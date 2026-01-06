/**
 * @file EditorDataManager.h
 * @brief Central coordinator for all editor data serialization
 * 
 * Provides a single point of entry for all save/load operations.
 * Manages multiple serializers and coordinates engine↔data synchronization.
 */
#pragma once

#include "ISerializer.h"
#include "WorldSerializer.h"
#include "SceneSerializer.h"
#include "EditorDataTypes.h"
#include "engine/world/World.h"
#include <memory>
#include <vector>
#include <functional>

namespace editor {

/**
 * @brief Callback for save/load progress reporting
 */
using ProgressCallback = std::function<void(const std::string& message, float progress)>;

/**
 * @brief Result of a batch save/load operation
 */
struct BatchResult {
    bool success = true;
    int successCount = 0;
    int failCount = 0;
    std::vector<std::string> errors;
    
    void addSuccess() { successCount++; }
    void addFailure(const std::string& error) {
        failCount++;
        success = false;
        errors.push_back(error);
    }
};

/**
 * @brief Central manager for all editor data operations
 * 
 * Responsibilities:
 * - Coordinates all serializers
 * - Handles save/load all operations
 * - Manages dirty state tracking
 * - Provides engine↔data synchronization
 */
class EditorDataManager {
public:
    EditorDataManager();
    ~EditorDataManager() = default;
    
    // Singleton access (optional - can also use as instance)
    static EditorDataManager& instance();
    
    /**
     * @brief Load all data files
     * @param callback Optional progress callback
     * @return Result with success/failure counts
     */
    BatchResult loadAll(ProgressCallback callback = nullptr);
    
    /**
     * @brief Save all modified data files
     * @param callback Optional progress callback
     * @return Result with success/failure counts
     */
    BatchResult saveAll(ProgressCallback callback = nullptr);
    
    /**
     * @brief Save all data files regardless of dirty state
     */
    BatchResult saveAllForced(ProgressCallback callback = nullptr);
    
    /**
     * @brief Sync all data FROM engine objects (after editing)
     * @param world The game world to sync from
     */
    void syncFromEngine(engine::World* world);
    
    /**
     * @brief Sync all data TO engine objects (after loading)
     * @param world The game world to sync to
     */
    void syncToEngine(engine::World* world);
    
    /**
     * @brief Check if any data has been modified
     */
    bool isDirty() const;
    
    /**
     * @brief Mark all data as clean
     */
    void markAllClean();
    
    /**
     * @brief Get status message for UI
     */
    std::string getStatusMessage() const { return m_statusMessage; }
    
    // Individual serializer access
    WorldSerializer& getWorldSerializer() { return m_worldSerializer; }
    SceneSerializer& getSceneSerializer() { return m_sceneSerializer; }
    
    const WorldSerializer& getWorldSerializer() const { return m_worldSerializer; }
    const SceneSerializer& getSceneSerializer() const { return m_sceneSerializer; }
    
    // Direct data access (convenience)
    WorldData& getWorldData() { return m_worldSerializer.getData(); }
    std::vector<SceneData>& getSceneData() { return m_sceneSerializer.getData(); }
    
    // Legacy aliases for backward compatibility
    SceneSerializer& getRoomSerializer() { return m_sceneSerializer; }
    const SceneSerializer& getRoomSerializer() const { return m_sceneSerializer; }
    std::vector<RoomData>& getRoomData() { return m_sceneSerializer.getData(); }
    
    // Editor preferences
    EditorPreferences& getPreferences() { return m_preferences; }
    const EditorPreferences& getPreferences() const { return m_preferences; }
    void loadPreferences();
    void savePreferences();
    
private:
    // Serializers
    WorldSerializer m_worldSerializer;
    SceneSerializer m_sceneSerializer;
    
    // Editor state
    EditorPreferences m_preferences;
    std::string m_statusMessage;
    
    // All serializers for iteration
    std::vector<ISerializer*> getAllSerializers();
};

} // namespace editor
