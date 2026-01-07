/**
 * @file ISerializer.h
 * @brief Abstract base class for all data serializers
 * 
 * Provides a common interface for loading and saving game data.
 * Each serializer handles one specific data file (rooms.json, world.json, etc.)
 */
#pragma once

#include <string>
#include <functional>

namespace editor {

/**
 * @brief Serialization result with error handling
 */
struct SerializationResult {
    bool success = false;
    std::string errorMessage;
    int itemCount = 0;
    
    static SerializationResult Success(int count = 0) {
        return { true, "", count };
    }
    
    static SerializationResult Failure(const std::string& error) {
        return { false, error, 0 };
    }
    
    operator bool() const { return success; }
};

/**
 * @brief Abstract base class for data serializers
 * 
 * Template Method Pattern - defines the skeleton of load/save operations.
 * Derived classes implement the specific serialization logic.
 */
class ISerializer {
public:
    virtual ~ISerializer() = default;
    
    /**
     * @brief Get the file path this serializer handles
     */
    virtual std::string getFilePath() const = 0;
    
    /**
     * @brief Get a human-readable name for this data type
     */
    virtual std::string getDataTypeName() const = 0;
    
    /**
     * @brief Load data from file
     * @return Result with success status and error message if failed
     */
    virtual SerializationResult load() = 0;
    
    /**
     * @brief Save data to file
     * @return Result with success status and error message if failed
     */
    virtual SerializationResult save() = 0;
    
    /**
     * @brief Check if data has been modified since last save
     */
    virtual bool isDirty() const = 0;
    
    /**
     * @brief Mark data as modified
     */
    virtual void markDirty() = 0;
    
    /**
     * @brief Mark data as clean (just saved)
     */
    virtual void markClean() = 0;
    
    /**
     * @brief Create a backup of the current file
     * @return true if backup was created successfully
     */
    bool createBackup() const;
    
protected:
    /**
     * @brief Helper to write JSON to file with backup
     */
    bool writeJsonToFile(const std::string& path, const std::string& jsonContent) const;
    
    /**
     * @brief Helper to read JSON from file
     */
    std::string readJsonFromFile(const std::string& path) const;
    
    bool m_dirty = false;
};

/**
 * @brief Base class for serializers that sync with engine objects
 * 
 * Extends ISerializer with sync capabilities for editor↔engine data flow.
 */
template<typename EngineType, typename DataType>
class ISyncableSerializer : public ISerializer {
public:
    /**
     * @brief Sync data FROM engine objects (after editing)
     * @param engineObject The engine object to read from
     */
    virtual void syncFromEngine(EngineType* engineObject) = 0;
    
    /**
     * @brief Sync data TO engine objects (after loading)
     * @param engineObject The engine object to update
     */
    virtual void syncToEngine(EngineType* engineObject) = 0;
    
    /**
     * @brief Get reference to the data being managed
     */
    virtual DataType& getData() = 0;
    virtual const DataType& getData() const = 0;
};

} // namespace editor
