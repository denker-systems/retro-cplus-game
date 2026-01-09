/**
 * @file PlayerStartActor.h
 * @brief Player spawn point marker (similar to Unreal Engine PlayerStart)
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include <glm/glm.hpp>

namespace engine {

/**
 * @brief Marks the spawn location for the player character
 * 
 * Place this actor in the world to define where the player spawns
 * when entering play mode. Only transform data is used.
 * 
 * Similar to APlayerStart in Unreal Engine.
 */
class PlayerStartActor : public ActorObjectExtended {
public:
    PlayerStartActor(const std::string& name = "PlayerStart");
    virtual ~PlayerStartActor() = default;
    
    // ========================================================================
    // SPAWN CONFIGURATION
    // ========================================================================
    
    /** @brief Get spawn position in 3D world coordinates */
    glm::vec3 getSpawnPosition() const;
    
    /** @brief Set spawn position in 3D world coordinates */
    void setSpawnPosition(const glm::vec3& position);
    
    /** @brief Get spawn rotation (yaw angle in degrees) */
    float getSpawnRotation() const { return m_spawnRotation; }
    
    /** @brief Set spawn rotation (yaw angle in degrees) */
    void setSpawnRotation(float yawDegrees) { m_spawnRotation = yawDegrees; }
    
    // ========================================================================
    // EDITOR VISUALIZATION
    // ========================================================================
    
    /** @brief Get the mesh color for editor rendering */
    glm::vec3 getEditorColor() const { return m_editorColor; }
    
    /** @brief Set the mesh color for editor rendering */
    void setEditorColor(const glm::vec3& color) { m_editorColor = color; }
    
    // ========================================================================
    // TYPE IDENTIFICATION
    // ========================================================================
    
    /** @brief Check if this is a PlayerStart actor */
    static bool isPlayerStart(const ActorObjectExtended* actor);
    
private:
    float m_spawnRotation = 0.0f;  // Yaw in degrees
    glm::vec3 m_editorColor{0.0f, 1.0f, 0.3f};  // Green for visibility
};

} // namespace engine
