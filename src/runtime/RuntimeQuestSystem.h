/**
 * @file RuntimeQuestSystem.h
 * @brief Quest tracking system för runtime
 */
#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

struct QuestObjective {
    std::string id;
    std::string type;        // "talk", "collect", "goto", "examine"
    std::string targetId;
    std::string description;
    int requiredCount = 1;
    int currentCount = 0;
    bool completed = false;
    bool optional = false;
    
    // Waypoint data
    std::string targetScene;
    glm::vec3 targetPosition{0.0f};
    bool showOnCompass = true;
    std::string waypointIcon;
    float waypointRadius = 2.0f;
};

struct Quest {
    std::string id;
    std::string title;
    std::string description;
    std::vector<QuestObjective> objectives;
    bool autoStart = false;
    bool active = false;
    bool completed = false;
    int rewardXP = 0;
    std::string rewardItem;
};

/**
 * @brief Manages quest state and progress
 */
class RuntimeQuestSystem {
public:
    RuntimeQuestSystem();
    ~RuntimeQuestSystem() = default;
    
    void loadQuests();
    
    void startQuest(const std::string& questId);
    void completeQuest(const std::string& questId);
    
    // Progress tracking
    void onTalk(const std::string& npcId);
    void onCollect(const std::string& itemId);
    void onGoto(const std::string& locationId);
    
    // Getters
    const std::vector<Quest>& getQuests() const { return m_quests; }
    std::vector<Quest*> getActiveQuests();
    Quest* getQuest(const std::string& id);
    
    // Waypoint integration
    void updateWaypoints();
    void syncWaypointsForQuest(const Quest& quest);
    const QuestObjective* getActiveObjective(const std::string& questId) const;
    std::vector<const QuestObjective*> getAllActiveObjectives() const;
    
    // Callbacks
    void setQuestCompleteCallback(std::function<void(const Quest&)> cb) { m_onComplete = cb; }
    void setObjectiveCompleteCallback(std::function<void(const Quest&, const QuestObjective&)> cb) { m_onObjective = cb; }

private:
    void checkQuestCompletion(Quest& quest);
    void createWaypointForObjective(const Quest& quest, const QuestObjective& obj);
    void removeWaypointForObjective(const std::string& questId, const std::string& objectiveId);
    glm::vec3 resolveTargetPosition(const QuestObjective& obj);
    std::string resolveTargetScene(const QuestObjective& obj);
    
    std::vector<Quest> m_quests;
    std::function<void(const Quest&)> m_onComplete;
    std::function<void(const Quest&, const QuestObjective&)> m_onObjective;
};
