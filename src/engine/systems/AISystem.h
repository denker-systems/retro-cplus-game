/**
 * @file AISystem.h
 * @brief AI-system för NPC-beteenden och scheman
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace engine { namespace actors {
    class NPC;
}}

// ============================================================================
// BEHAVIOR TYPES
// ============================================================================

enum class BehaviorType {
    Idle,       // Stå still
    Patrol,     // Gå mellan waypoints
    Wander,     // Slumpmässig rörelse
    Follow,     // Följ spelare
    GoTo        // Gå till specifik punkt
};

/**
 * @brief Waypoint för patrol/movement
 */
struct Waypoint {
    float x, y;
    float waitTime = 0.0f;  // Tid att vänta vid waypoint
};

/**
 * @brief Beteendekonfiguration för en NPC
 */
struct Behavior {
    BehaviorType type = BehaviorType::Idle;
    std::vector<Waypoint> waypoints;
    int currentWaypointIndex = 0;
    float waitTimer = 0.0f;
    bool looping = true;
    
    // För Wander
    float wanderRadius = 50.0f;
    float wanderTimer = 0.0f;
    float wanderInterval = 3.0f;
};

// ============================================================================
// SCHEDULE
// ============================================================================

/**
 * @brief Schemalagd aktivitet
 */
struct ScheduleEntry {
    int startHour;      // 0-23
    int endHour;
    std::string roomId;
    float x, y;
    BehaviorType behavior = BehaviorType::Idle;
};

/**
 * @brief Schema för en NPC över dygnet
 */
struct Schedule {
    std::vector<ScheduleEntry> entries;
};

// ============================================================================
// AI SYSTEM
// ============================================================================

/**
 * @brief Singleton som hanterar all NPC AI
 */
class AISystem {
public:
    static AISystem& instance();
    
    /** @brief Uppdatera alla NPC:s AI */
    void update(float deltaTime);
    
    /** @brief Registrera en NPC för AI-hantering */
    void registerNPC(engine::actors::NPC* npc);
    
    /** @brief Avregistrera NPC */
    void unregisterNPC(engine::actors::NPC* npc);
    
    /** @brief Sätt beteende för en NPC */
    void setBehavior(const std::string& npcId, BehaviorType type);
    void setBehavior(const std::string& npcId, const Behavior& behavior);
    
    /** @brief Lägg till patrol waypoints */
    void addPatrolWaypoint(const std::string& npcId, float x, float y, float waitTime = 0.0f);
    void clearPatrolWaypoints(const std::string& npcId);
    
    /** @brief Sätt schema för NPC */
    void setSchedule(const std::string& npcId, const Schedule& schedule);
    
    /** @brief Sätt aktuell speltid (för scheman) */
    void setGameTime(int hour, int minute = 0);
    int getGameHour() const { return m_gameHour; }
    int getGameMinute() const { return m_gameMinute; }
    
    /** @brief Aktivera/avaktivera tidsflöde */
    void setTimeFlow(bool enabled) { m_timeFlowEnabled = enabled; }
    void setTimeScale(float scale) { m_timeScale = scale; }
    
    /** @brief Hämta beteende */
    Behavior* getBehavior(const std::string& npcId);

private:
    AISystem() = default;
    ~AISystem() = default;
    AISystem(const AISystem&) = delete;
    AISystem& operator=(const AISystem&) = delete;
    
    void updateBehavior(engine::actors::NPC* npc, Behavior& behavior, float deltaTime);
    void updateIdle(engine::actors::NPC* npc, Behavior& behavior, float deltaTime);
    void updatePatrol(engine::actors::NPC* npc, Behavior& behavior, float deltaTime);
    void updateWander(engine::actors::NPC* npc, Behavior& behavior, float deltaTime);
    void updateGoTo(engine::actors::NPC* npc, Behavior& behavior, float deltaTime);
    
    void updateSchedules();
    void applyScheduleEntry(engine::actors::NPC* npc, const ScheduleEntry& entry);
    
    std::vector<engine::actors::NPC*> m_npcs;
    std::unordered_map<std::string, Behavior> m_behaviors;
    std::unordered_map<std::string, Schedule> m_schedules;
    
    // Speltid
    int m_gameHour = 12;
    int m_gameMinute = 0;
    float m_timeAccumulator = 0.0f;
    float m_timeScale = 60.0f;  // 1 sekund realtid = 1 minut speltid
    bool m_timeFlowEnabled = true;
};
