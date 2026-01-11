/**
 * @file WaypointSystem.cpp
 * @brief Implementation of WaypointSystem
 */
#include "WaypointSystem.h"
#include "engine/utils/Logger.h"
#include <glm/gtc/constants.hpp>
#include <algorithm>
#include <cmath>

namespace engine {

WaypointSystem& WaypointSystem::instance() {
    static WaypointSystem instance;
    return instance;
}

void WaypointSystem::addWaypoint(const Waypoint& waypoint) {
    // Check for duplicate ID
    auto it = std::find_if(m_waypoints.begin(), m_waypoints.end(),
        [&waypoint](const Waypoint& w) { return w.id == waypoint.id; });
    
    if (it != m_waypoints.end()) {
        // Update existing
        *it = waypoint;
        LOG_DEBUG("[WaypointSystem] Updated waypoint: " + waypoint.id);
    } else {
        m_waypoints.push_back(waypoint);
        LOG_INFO("[WaypointSystem] Added waypoint: " + waypoint.id + " at scene " + waypoint.sceneId);
    }
}

void WaypointSystem::removeWaypoint(const std::string& id) {
    auto it = std::remove_if(m_waypoints.begin(), m_waypoints.end(),
        [&id](const Waypoint& w) { return w.id == id; });
    
    if (it != m_waypoints.end()) {
        m_waypoints.erase(it, m_waypoints.end());
        LOG_DEBUG("[WaypointSystem] Removed waypoint: " + id);
    }
    
    // Also remove from arrived list
    m_arrivedWaypoints.erase(
        std::remove(m_arrivedWaypoints.begin(), m_arrivedWaypoints.end(), id),
        m_arrivedWaypoints.end());
}

void WaypointSystem::removeQuestWaypoints(const std::string& questId) {
    auto it = std::remove_if(m_waypoints.begin(), m_waypoints.end(),
        [&questId](const Waypoint& w) { return w.questId == questId; });
    
    if (it != m_waypoints.end()) {
        int count = static_cast<int>(std::distance(it, m_waypoints.end()));
        m_waypoints.erase(it, m_waypoints.end());
        LOG_DEBUG("[WaypointSystem] Removed " + std::to_string(count) + " waypoints for quest: " + questId);
    }
}

void WaypointSystem::clearAll() {
    m_waypoints.clear();
    m_arrivedWaypoints.clear();
    LOG_INFO("[WaypointSystem] Cleared all waypoints");
}

Waypoint* WaypointSystem::getWaypoint(const std::string& id) {
    auto it = std::find_if(m_waypoints.begin(), m_waypoints.end(),
        [&id](const Waypoint& w) { return w.id == id; });
    return (it != m_waypoints.end()) ? &(*it) : nullptr;
}

const Waypoint* WaypointSystem::getWaypoint(const std::string& id) const {
    auto it = std::find_if(m_waypoints.begin(), m_waypoints.end(),
        [&id](const Waypoint& w) { return w.id == id; });
    return (it != m_waypoints.end()) ? &(*it) : nullptr;
}

std::vector<Waypoint*> WaypointSystem::getActiveWaypoints() {
    std::vector<Waypoint*> result;
    for (auto& w : m_waypoints) {
        if (w.active && w.visible) {
            result.push_back(&w);
        }
    }
    return result;
}

std::vector<const Waypoint*> WaypointSystem::getCompassWaypoints() const {
    std::vector<const Waypoint*> result;
    for (const auto& w : m_waypoints) {
        if (w.active && w.visible && w.showOnCompass) {
            result.push_back(&w);
        }
    }
    
    // Sort by priority (highest first), then by distance
    std::sort(result.begin(), result.end(),
        [](const Waypoint* a, const Waypoint* b) {
            if (static_cast<int>(a->priority) != static_cast<int>(b->priority)) {
                return static_cast<int>(a->priority) > static_cast<int>(b->priority);
            }
            return a->distanceToPlayer < b->distanceToPlayer;
        });
    
    return result;
}

std::vector<const Waypoint*> WaypointSystem::getSceneWaypoints(const std::string& sceneId) const {
    std::vector<const Waypoint*> result;
    for (const auto& w : m_waypoints) {
        if (w.active && w.visible && w.sceneId == sceneId) {
            result.push_back(&w);
        }
    }
    return result;
}

const Waypoint* WaypointSystem::getNearestWaypoint(const glm::vec3& position) const {
    const Waypoint* nearest = nullptr;
    float minDist = std::numeric_limits<float>::max();
    
    for (const auto& w : m_waypoints) {
        if (!w.active || !w.visible) continue;
        
        float dist = glm::length(w.position - position);
        if (dist < minDist) {
            minDist = dist;
            nearest = &w;
        }
    }
    
    return nearest;
}

const Waypoint* WaypointSystem::getPrimaryWaypoint() const {
    const Waypoint* primary = nullptr;
    
    for (const auto& w : m_waypoints) {
        if (!w.active || !w.visible || !w.showOnCompass) continue;
        
        // Prefer waypoints in current scene/level
        if (!primary) {
            primary = &w;
            continue;
        }
        
        // Priority comparison
        if (static_cast<int>(w.priority) > static_cast<int>(primary->priority)) {
            primary = &w;
        } else if (static_cast<int>(w.priority) == static_cast<int>(primary->priority)) {
            // Same priority - prefer closer
            if (w.distanceToPlayer < primary->distanceToPlayer) {
                primary = &w;
            }
        }
    }
    
    return primary;
}

void WaypointSystem::update(const glm::vec3& playerPosition, float playerYaw,
                            const std::string& currentSceneId, const std::string& currentLevelId) {
    for (auto& w : m_waypoints) {
        // Update scene/level flags
        w.isInCurrentScene = (w.sceneId == currentSceneId);
        w.isInCurrentLevel = (w.levelId == currentLevelId || w.levelId.empty());
        
        // Calculate distance
        glm::vec3 toWaypoint = w.position - playerPosition;
        w.distanceToPlayer = glm::length(toWaypoint);
        
        // Calculate angle (horizontal only)
        float waypointAngle = std::atan2(toWaypoint.x, toWaypoint.z);
        float playerAngleRad = glm::radians(playerYaw);
        w.angleToPlayer = waypointAngle - playerAngleRad;
        
        // Normalize angle to -PI to PI
        while (w.angleToPlayer > glm::pi<float>()) w.angleToPlayer -= 2.0f * glm::pi<float>();
        while (w.angleToPlayer < -glm::pi<float>()) w.angleToPlayer += 2.0f * glm::pi<float>();
        
        // Check arrival
        if (w.active && w.isInCurrentScene && w.distanceToPlayer <= w.arrivalRadius) {
            // Check if we already triggered arrival for this waypoint
            auto it = std::find(m_arrivedWaypoints.begin(), m_arrivedWaypoints.end(), w.id);
            if (it == m_arrivedWaypoints.end()) {
                m_arrivedWaypoints.push_back(w.id);
                
                if (m_onArrival) {
                    m_onArrival(w);
                }
                
                LOG_DEBUG("[WaypointSystem] Player arrived at waypoint: " + w.id);
            }
        }
    }
}

} // namespace engine
