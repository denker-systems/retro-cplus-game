/**
 * @file RuntimeQuestSystem.cpp
 * @brief Implementation av quest tracking system
 */
#include "RuntimeQuestSystem.h"
#include "engine/data/DataLoader.h"
#include "engine/systems/WaypointSystem.h"
#include "engine/utils/Logger.h"

RuntimeQuestSystem::RuntimeQuestSystem() = default;

void RuntimeQuestSystem::loadQuests() {
    auto& dataLoader = DataLoader::instance();
    const auto& questData = dataLoader.getQuests();
    
    m_quests.clear();
    
    for (const auto& q : questData) {
        Quest quest;
        quest.id = q.id;
        quest.title = q.title;
        quest.description = q.description;
        quest.autoStart = q.autoStart;
        quest.rewardXP = q.rewardXP;
        quest.rewardItem = q.rewardItem;
        quest.active = q.autoStart;
        quest.completed = false;
        
        for (const auto& o : q.objectives) {
            QuestObjective obj;
            obj.id = o.id;
            obj.type = o.type;
            obj.targetId = o.targetId;
            obj.description = o.description;
            obj.requiredCount = o.requiredCount;
            obj.optional = o.optional;
            obj.currentCount = 0;
            obj.completed = false;
            
            // Load waypoint data
            obj.targetScene = o.targetScene;
            obj.targetPosition = glm::vec3(o.targetX, o.targetY, o.targetZ);
            obj.showOnCompass = o.showOnCompass;
            obj.waypointIcon = o.waypointIcon;
            obj.waypointRadius = o.waypointRadius;
            
            quest.objectives.push_back(obj);
        }
        
        m_quests.push_back(quest);
        
        // Create waypoints for auto-start quests
        if (quest.active) {
            syncWaypointsForQuest(quest);
        }
    }
    
    LOG_INFO("[RuntimeQuestSystem] Loaded " + std::to_string(m_quests.size()) + " quests");
}

void RuntimeQuestSystem::startQuest(const std::string& questId) {
    Quest* quest = getQuest(questId);
    if (!quest) return;
    
    if (!quest->active && !quest->completed) {
        quest->active = true;
        syncWaypointsForQuest(*quest);
        LOG_INFO("[RuntimeQuestSystem] Started quest: " + quest->title);
    }
}

void RuntimeQuestSystem::completeQuest(const std::string& questId) {
    Quest* quest = getQuest(questId);
    if (!quest || quest->completed) return;
    
    quest->completed = true;
    quest->active = false;
    
    // Remove all waypoints for this quest
    engine::WaypointSystem::instance().removeQuestWaypoints(questId);
    
    LOG_INFO("[RuntimeQuestSystem] Completed quest: " + quest->title + 
             " (+" + std::to_string(quest->rewardXP) + " XP)");
    
    if (m_onComplete) {
        m_onComplete(*quest);
    }
}

void RuntimeQuestSystem::onTalk(const std::string& npcId) {
    for (auto& quest : m_quests) {
        if (!quest.active) continue;
        
        for (auto& obj : quest.objectives) {
            if (obj.completed) continue;
            if (obj.type == "talk" && obj.targetId == npcId) {
                obj.currentCount++;
                if (obj.currentCount >= obj.requiredCount) {
                    obj.completed = true;
                    LOG_INFO("[RuntimeQuestSystem] Objective complete: " + obj.description);
                    if (m_onObjective) {
                        m_onObjective(quest, obj);
                    }
                }
                checkQuestCompletion(quest);
            }
        }
    }
}

void RuntimeQuestSystem::onCollect(const std::string& itemId) {
    for (auto& quest : m_quests) {
        if (!quest.active) continue;
        
        for (auto& obj : quest.objectives) {
            if (obj.completed) continue;
            if (obj.type == "collect" && obj.targetId == itemId) {
                obj.currentCount++;
                if (obj.currentCount >= obj.requiredCount) {
                    obj.completed = true;
                    LOG_INFO("[RuntimeQuestSystem] Objective complete: " + obj.description);
                    if (m_onObjective) {
                        m_onObjective(quest, obj);
                    }
                }
                checkQuestCompletion(quest);
            }
        }
    }
}

void RuntimeQuestSystem::onGoto(const std::string& locationId) {
    for (auto& quest : m_quests) {
        if (!quest.active) continue;
        
        for (auto& obj : quest.objectives) {
            if (obj.completed) continue;
            if (obj.type == "goto" && obj.targetId == locationId) {
                obj.currentCount++;
                if (obj.currentCount >= obj.requiredCount) {
                    obj.completed = true;
                    LOG_INFO("[RuntimeQuestSystem] Objective complete: " + obj.description);
                    if (m_onObjective) {
                        m_onObjective(quest, obj);
                    }
                }
                checkQuestCompletion(quest);
            }
        }
    }
}

std::vector<Quest*> RuntimeQuestSystem::getActiveQuests() {
    std::vector<Quest*> active;
    for (auto& quest : m_quests) {
        if (quest.active && !quest.completed) {
            active.push_back(&quest);
        }
    }
    return active;
}

Quest* RuntimeQuestSystem::getQuest(const std::string& id) {
    for (auto& quest : m_quests) {
        if (quest.id == id) {
            return &quest;
        }
    }
    return nullptr;
}

void RuntimeQuestSystem::checkQuestCompletion(Quest& quest) {
    bool allComplete = true;
    for (const auto& obj : quest.objectives) {
        if (!obj.optional && !obj.completed) {
            allComplete = false;
            break;
        }
    }
    
    if (allComplete) {
        completeQuest(quest.id);
    }
}

// ============================================================================
// WAYPOINT INTEGRATION
// ============================================================================

void RuntimeQuestSystem::updateWaypoints() {
    for (const auto& quest : m_quests) {
        if (quest.active && !quest.completed) {
            syncWaypointsForQuest(quest);
        }
    }
}

void RuntimeQuestSystem::syncWaypointsForQuest(const Quest& quest) {
    auto& waypointSystem = engine::WaypointSystem::instance();
    
    for (const auto& obj : quest.objectives) {
        if (obj.completed) {
            // Remove waypoint for completed objectives
            removeWaypointForObjective(quest.id, obj.id);
        } else if (obj.showOnCompass) {
            // Create/update waypoint for incomplete objectives
            createWaypointForObjective(quest, obj);
        }
    }
}

void RuntimeQuestSystem::createWaypointForObjective(const Quest& quest, const QuestObjective& obj) {
    auto& waypointSystem = engine::WaypointSystem::instance();
    
    engine::Waypoint waypoint;
    waypoint.id = quest.id + "_" + obj.id;
    waypoint.questId = quest.id;
    waypoint.objectiveId = obj.id;
    waypoint.label = obj.description;
    waypoint.description = quest.title + ": " + obj.description;
    
    // Determine waypoint type based on objective type
    if (obj.type == "talk") {
        waypoint.type = engine::WaypointType::QuestObjective;
        waypoint.color = glm::vec3(0.2f, 0.8f, 1.0f);  // Cyan for talk
    } else if (obj.type == "collect") {
        waypoint.type = engine::WaypointType::QuestObjective;
        waypoint.color = glm::vec3(1.0f, 0.8f, 0.2f);  // Gold for collect
    } else if (obj.type == "goto") {
        waypoint.type = engine::WaypointType::PointOfInterest;
        waypoint.color = glm::vec3(0.2f, 1.0f, 0.4f);  // Green for goto
    } else if (obj.type == "examine") {
        waypoint.type = engine::WaypointType::QuestObjective;
        waypoint.color = glm::vec3(0.8f, 0.4f, 1.0f);  // Purple for examine
    } else {
        waypoint.type = engine::WaypointType::Custom;
        waypoint.color = glm::vec3(1.0f, 1.0f, 1.0f);  // White default
    }
    
    // Set position - use explicit position or resolve from target
    if (obj.targetPosition != glm::vec3(0.0f) || !obj.targetScene.empty()) {
        waypoint.position = obj.targetPosition;
        waypoint.sceneId = obj.targetScene;
    } else {
        // Try to resolve position from targetId
        waypoint.position = resolveTargetPosition(obj);
        waypoint.sceneId = resolveTargetScene(obj);
    }
    
    waypoint.arrivalRadius = obj.waypointRadius;
    waypoint.iconName = obj.waypointIcon;
    waypoint.showOnCompass = obj.showOnCompass;
    waypoint.showInWorld = true;
    waypoint.showDistance = true;
    waypoint.priority = engine::WaypointPriority::Normal;
    waypoint.active = true;
    waypoint.visible = true;
    
    waypointSystem.addWaypoint(waypoint);
}

void RuntimeQuestSystem::removeWaypointForObjective(const std::string& questId, const std::string& objectiveId) {
    auto& waypointSystem = engine::WaypointSystem::instance();
    waypointSystem.removeWaypoint(questId + "_" + objectiveId);
}

glm::vec3 RuntimeQuestSystem::resolveTargetPosition(const QuestObjective& obj) {
    // Try to find position from NPCs, items, or scenes
    auto& dataLoader = DataLoader::instance();
    
    if (obj.type == "talk") {
        // Look up NPC position
        for (const auto& npc : dataLoader.getNPCs()) {
            if (npc.id == obj.targetId) {
                // Convert 2D position to 3D (x/100, 0, y/100)
                return glm::vec3(npc.x / 100.0f, 0.0f, npc.y / 100.0f);
            }
        }
    } else if (obj.type == "goto") {
        // Look up scene/room position (use center or spawn point)
        for (const auto& room : dataLoader.getRooms()) {
            if (room.id == obj.targetId) {
                return glm::vec3(room.playerSpawnX / 100.0f, 0.0f, room.playerSpawnY / 100.0f);
            }
        }
    }
    
    // Default: return origin
    return glm::vec3(0.0f);
}

std::string RuntimeQuestSystem::resolveTargetScene(const QuestObjective& obj) {
    auto& dataLoader = DataLoader::instance();
    
    if (obj.type == "talk") {
        // Look up NPC's room
        for (const auto& npc : dataLoader.getNPCs()) {
            if (npc.id == obj.targetId) {
                return npc.room;
            }
        }
    } else if (obj.type == "goto") {
        // Target is the scene itself
        return obj.targetId;
    }
    
    return "";
}

const QuestObjective* RuntimeQuestSystem::getActiveObjective(const std::string& questId) const {
    for (const auto& quest : m_quests) {
        if (quest.id == questId && quest.active) {
            // Return first incomplete non-optional objective
            for (const auto& obj : quest.objectives) {
                if (!obj.completed && !obj.optional) {
                    return &obj;
                }
            }
        }
    }
    return nullptr;
}

std::vector<const QuestObjective*> RuntimeQuestSystem::getAllActiveObjectives() const {
    std::vector<const QuestObjective*> result;
    
    for (const auto& quest : m_quests) {
        if (quest.active && !quest.completed) {
            for (const auto& obj : quest.objectives) {
                if (!obj.completed) {
                    result.push_back(&obj);
                }
            }
        }
    }
    
    return result;
}
