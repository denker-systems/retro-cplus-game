/**
 * @file AISystem.cpp
 * @brief Implementation av AI-systemet
 */
#include "AISystem.h"
#include "../entities/NPC.h"
#include "RoomManager.h"
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <algorithm>

AISystem& AISystem::instance() {
    static AISystem inst;
    return inst;
}

void AISystem::update(float deltaTime) {
    // Uppdatera speltid
    if (m_timeFlowEnabled) {
        m_timeAccumulator += deltaTime * m_timeScale;
        while (m_timeAccumulator >= 60.0f) {
            m_timeAccumulator -= 60.0f;
            m_gameMinute++;
            if (m_gameMinute >= 60) {
                m_gameMinute = 0;
                m_gameHour = (m_gameHour + 1) % 24;
                updateSchedules();
            }
        }
    }
    
    // Uppdatera alla NPC:s beteenden
    for (NPC* npc : m_npcs) {
        if (!npc) continue;
        
        auto it = m_behaviors.find(npc->getName());
        if (it != m_behaviors.end()) {
            updateBehavior(npc, it->second, deltaTime);
        }
    }
}

void AISystem::registerNPC(NPC* npc) {
    if (!npc) return;
    
    // Kolla om redan registrerad
    auto it = std::find(m_npcs.begin(), m_npcs.end(), npc);
    if (it == m_npcs.end()) {
        m_npcs.push_back(npc);
        
        // Skapa default behavior om ingen finns
        if (m_behaviors.find(npc->getName()) == m_behaviors.end()) {
            m_behaviors[npc->getName()] = Behavior{};
        }
    }
}

void AISystem::unregisterNPC(NPC* npc) {
    if (!npc) return;
    m_npcs.erase(std::remove(m_npcs.begin(), m_npcs.end(), npc), m_npcs.end());
}

void AISystem::setBehavior(const std::string& npcId, BehaviorType type) {
    Behavior behavior;
    behavior.type = type;
    m_behaviors[npcId] = behavior;
}

void AISystem::setBehavior(const std::string& npcId, const Behavior& behavior) {
    m_behaviors[npcId] = behavior;
}

void AISystem::addPatrolWaypoint(const std::string& npcId, float x, float y, float waitTime) {
    Waypoint wp;
    wp.x = x;
    wp.y = y;
    wp.waitTime = waitTime;
    m_behaviors[npcId].waypoints.push_back(wp);
    m_behaviors[npcId].type = BehaviorType::Patrol;
}

void AISystem::clearPatrolWaypoints(const std::string& npcId) {
    m_behaviors[npcId].waypoints.clear();
    m_behaviors[npcId].currentWaypointIndex = 0;
}

void AISystem::setSchedule(const std::string& npcId, const Schedule& schedule) {
    m_schedules[npcId] = schedule;
}

void AISystem::setGameTime(int hour, int minute) {
    m_gameHour = hour % 24;
    m_gameMinute = minute % 60;
    updateSchedules();
}

Behavior* AISystem::getBehavior(const std::string& npcId) {
    auto it = m_behaviors.find(npcId);
    return (it != m_behaviors.end()) ? &it->second : nullptr;
}

// ============================================================================
// BEHAVIOR UPDATES
// ============================================================================

void AISystem::updateBehavior(NPC* npc, Behavior& behavior, float deltaTime) {
    switch (behavior.type) {
        case BehaviorType::Idle:
            updateIdle(npc, behavior, deltaTime);
            break;
        case BehaviorType::Patrol:
            updatePatrol(npc, behavior, deltaTime);
            break;
        case BehaviorType::Wander:
            updateWander(npc, behavior, deltaTime);
            break;
        case BehaviorType::GoTo:
            updateGoTo(npc, behavior, deltaTime);
            break;
        case BehaviorType::Follow:
            // TODO: Implementera follow-logik
            break;
    }
}

void AISystem::updateIdle(NPC* npc, Behavior& behavior, float deltaTime) {
    // Idle gör inget speciellt, NPC står still
    (void)npc;
    (void)behavior;
    (void)deltaTime;
}

void AISystem::updatePatrol(NPC* npc, Behavior& behavior, float deltaTime) {
    if (behavior.waypoints.empty()) return;
    
    // Vänta vid waypoint?
    if (behavior.waitTimer > 0.0f) {
        behavior.waitTimer -= deltaTime;
        return;
    }
    
    const Waypoint& target = behavior.waypoints[behavior.currentWaypointIndex];
    float dx = target.x - npc->getX();
    float dy = target.y - npc->getY();
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < 5.0f) {
        // Nådde waypoint
        behavior.waitTimer = target.waitTime;
        behavior.currentWaypointIndex++;
        
        if (behavior.currentWaypointIndex >= static_cast<int>(behavior.waypoints.size())) {
            if (behavior.looping) {
                behavior.currentWaypointIndex = 0;
            } else {
                behavior.currentWaypointIndex = static_cast<int>(behavior.waypoints.size()) - 1;
                behavior.type = BehaviorType::Idle;
            }
        }
    } else {
        // Rör sig mot waypoint
        float speed = npc->getSpeed() * deltaTime;
        float moveX = (dx / dist) * speed;
        float moveY = (dy / dist) * speed;
        npc->setPosition(npc->getX() + moveX, npc->getY() + moveY);
    }
}

void AISystem::updateWander(NPC* npc, Behavior& behavior, float deltaTime) {
    behavior.wanderTimer -= deltaTime;
    
    if (behavior.wanderTimer <= 0.0f) {
        // Välj ny slumpmässig destination
        float angle = static_cast<float>(rand()) / RAND_MAX * 6.28318f;
        float radius = static_cast<float>(rand()) / RAND_MAX * behavior.wanderRadius;
        
        float baseX = npc->getX();
        float baseY = npc->getY();
        
        // Sätt nytt mål
        behavior.waypoints.clear();
        Waypoint wp;
        wp.x = baseX + std::cos(angle) * radius;
        wp.y = baseY + std::sin(angle) * radius;
        wp.waitTime = 0.5f;
        behavior.waypoints.push_back(wp);
        behavior.currentWaypointIndex = 0;
        
        behavior.wanderTimer = behavior.wanderInterval + 
            (static_cast<float>(rand()) / RAND_MAX * 2.0f);
    }
    
    // Använd patrol-logik för att röra sig mot målet
    if (!behavior.waypoints.empty()) {
        updatePatrol(npc, behavior, deltaTime);
    }
}

void AISystem::updateGoTo(NPC* npc, Behavior& behavior, float deltaTime) {
    if (behavior.waypoints.empty()) {
        behavior.type = BehaviorType::Idle;
        return;
    }
    
    const Waypoint& target = behavior.waypoints[0];
    float dx = target.x - npc->getX();
    float dy = target.y - npc->getY();
    float dist = std::sqrt(dx * dx + dy * dy);
    
    if (dist < 5.0f) {
        // Nådde målet
        behavior.waypoints.clear();
        behavior.type = BehaviorType::Idle;
    } else {
        float speed = npc->getSpeed() * deltaTime;
        float moveX = (dx / dist) * speed;
        float moveY = (dy / dist) * speed;
        npc->setPosition(npc->getX() + moveX, npc->getY() + moveY);
    }
}

// ============================================================================
// SCHEDULES
// ============================================================================

void AISystem::updateSchedules() {
    for (auto& [npcId, schedule] : m_schedules) {
        for (const auto& entry : schedule.entries) {
            if (m_gameHour >= entry.startHour && m_gameHour < entry.endHour) {
                // Hitta NPC
                for (NPC* npc : m_npcs) {
                    if (npc && npc->getName() == npcId) {
                        applyScheduleEntry(npc, entry);
                        break;
                    }
                }
                break;
            }
        }
    }
}

void AISystem::applyScheduleEntry(NPC* npc, const ScheduleEntry& entry) {
    // Kolla om NPC behöver byta rum
    std::string currentRoom = RoomManager::instance().getCurrentRoomId();
    
    if (entry.roomId != currentRoom) {
        // NPC är i ett annat rum, sätt bara position för när spelaren går dit
        // I en mer avancerad implementation skulle vi hantera NPC:er i alla rum
        return;
    }
    
    // Sätt beteende och position
    Behavior behavior;
    behavior.type = entry.behavior;
    
    if (entry.behavior == BehaviorType::GoTo) {
        Waypoint wp;
        wp.x = entry.x;
        wp.y = entry.y;
        behavior.waypoints.push_back(wp);
    }
    
    m_behaviors[npc->getName()] = behavior;
    
    // Om idle, teleportera direkt till position
    if (entry.behavior == BehaviorType::Idle) {
        npc->setPosition(entry.x, entry.y);
    }
}
