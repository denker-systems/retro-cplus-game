/**
 * @file WaypointSystem.h
 * @brief Waypoint and navigation marker system for quest objectives
 */
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <functional>

namespace engine {

/**
 * @brief Type of waypoint marker
 */
enum class WaypointType {
    QuestObjective,     // Active quest objective
    QuestTurnIn,        // Quest completion NPC
    PointOfInterest,    // General POI
    Custom              // Custom marker
};

/**
 * @brief Priority for waypoint display (higher = more prominent)
 */
enum class WaypointPriority {
    Low = 0,
    Normal = 1,
    High = 2,
    Critical = 3
};

/**
 * @brief A navigation waypoint/marker in the world
 */
struct Waypoint {
    std::string id;                     // Unique identifier
    std::string label;                  // Display text
    std::string description;            // Extended description
    WaypointType type = WaypointType::QuestObjective;
    WaypointPriority priority = WaypointPriority::Normal;
    
    // Location
    std::string sceneId;                // Scene where waypoint is located
    std::string levelId;                // Level (for cross-level waypoints)
    glm::vec3 position{0.0f};           // 3D world position
    float arrivalRadius = 2.0f;         // Distance to consider "arrived"
    
    // Visual
    std::string iconName;               // Icon to display (empty = default)
    glm::vec3 color{1.0f, 0.8f, 0.0f};  // Marker color (default: gold)
    bool showDistance = true;           // Show distance in meters
    bool showOnCompass = true;          // Show on compass HUD
    bool showInWorld = true;            // Show 3D marker in world
    bool pulse = false;                 // Pulsing animation
    
    // Quest link
    std::string questId;                // Associated quest (if any)
    std::string objectiveId;            // Associated objective (if any)
    
    // State
    bool active = true;
    bool visible = true;
    float distanceToPlayer = 0.0f;      // Updated each frame
    float angleToPlayer = 0.0f;         // Horizontal angle from player view
    bool isInCurrentScene = false;      // Is in player's current scene
    bool isInCurrentLevel = false;      // Is in player's current level
};

/**
 * @brief Manages all waypoints and navigation markers
 */
class WaypointSystem {
public:
    static WaypointSystem& instance();
    
    // ========================================================================
    // WAYPOINT MANAGEMENT
    // ========================================================================
    
    /** @brief Add a waypoint */
    void addWaypoint(const Waypoint& waypoint);
    
    /** @brief Remove waypoint by ID */
    void removeWaypoint(const std::string& id);
    
    /** @brief Remove all waypoints for a quest */
    void removeQuestWaypoints(const std::string& questId);
    
    /** @brief Clear all waypoints */
    void clearAll();
    
    /** @brief Get waypoint by ID */
    Waypoint* getWaypoint(const std::string& id);
    const Waypoint* getWaypoint(const std::string& id) const;
    
    // ========================================================================
    // QUERIES
    // ========================================================================
    
    /** @brief Get all active waypoints */
    std::vector<Waypoint*> getActiveWaypoints();
    
    /** @brief Get waypoints visible on compass */
    std::vector<const Waypoint*> getCompassWaypoints() const;
    
    /** @brief Get waypoints in current scene */
    std::vector<const Waypoint*> getSceneWaypoints(const std::string& sceneId) const;
    
    /** @brief Get nearest waypoint */
    const Waypoint* getNearestWaypoint(const glm::vec3& position) const;
    
    /** @brief Get primary quest waypoint (highest priority active) */
    const Waypoint* getPrimaryWaypoint() const;
    
    // ========================================================================
    // UPDATE
    // ========================================================================
    
    /** @brief Update waypoint distances and angles from player */
    void update(const glm::vec3& playerPosition, float playerYaw, 
                const std::string& currentSceneId, const std::string& currentLevelId);
    
    // ========================================================================
    // CALLBACKS
    // ========================================================================
    
    /** @brief Set callback for waypoint arrival */
    void setArrivalCallback(std::function<void(const Waypoint&)> callback) {
        m_onArrival = callback;
    }
    
private:
    WaypointSystem() = default;
    ~WaypointSystem() = default;
    WaypointSystem(const WaypointSystem&) = delete;
    WaypointSystem& operator=(const WaypointSystem&) = delete;
    
    std::vector<Waypoint> m_waypoints;
    std::function<void(const Waypoint&)> m_onArrival;
    
    // Track arrivals to avoid repeated callbacks
    std::vector<std::string> m_arrivedWaypoints;
};

} // namespace engine
