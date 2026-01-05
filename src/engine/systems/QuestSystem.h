/**
 * @file QuestSystem.h
 * @brief Quest-hantering med objectives och triggers
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

/**
 * @brief Status för quest/objective
 */
enum class QuestStatus {
    Inactive,   // Inte påbörjad
    Active,     // Pågår
    Completed,  // Klar
    Failed      // Misslyckad
};

/**
 * @brief Typ av objective
 */
enum class ObjectiveType {
    Talk,       // Prata med NPC
    Collect,    // Samla item
    Deliver,    // Leverera item till NPC
    GoTo,       // Gå till rum/plats
    Examine     // Undersök hotspot
};

/**
 * @brief Ett mål inom en quest
 */
struct Objective {
    std::string id;
    std::string description;
    ObjectiveType type;
    std::string targetId;       // NPC/Item/Room ID
    int requiredCount = 1;      // Antal som krävs
    int currentCount = 0;       // Aktuellt antal
    QuestStatus status = QuestStatus::Inactive;
    bool optional = false;      // Valfritt mål
};

/**
 * @brief En quest med flera objectives
 */
struct Quest {
    std::string id;
    std::string title;
    std::string description;
    std::vector<Objective> objectives;
    QuestStatus status = QuestStatus::Inactive;
    std::string rewardItem;     // Item-ID som belöning
    int rewardXP = 0;
};

/**
 * @brief Singleton för quest-hantering
 */
class QuestSystem {
public:
    static QuestSystem& instance();
    
    /** @brief Registrera en quest */
    void addQuest(const Quest& quest);
    
    /** @brief Starta en quest */
    bool startQuest(const std::string& questId);
    
    /** @brief Uppdatera objective progress */
    void updateObjective(ObjectiveType type, const std::string& targetId, int count = 1);
    
    /** @brief Kolla om quest är aktiv */
    bool isQuestActive(const std::string& questId) const;
    
    /** @brief Kolla om quest är klar */
    bool isQuestCompleted(const std::string& questId) const;
    
    /** @brief Hämta aktiva quests */
    std::vector<Quest> getActiveQuests() const;
    
    /** @brief Hämta avklarade quests */
    std::vector<Quest> getCompletedQuests() const;
    
    /** @brief Hämta alla quests */
    std::vector<const Quest*> getAllQuests() const;
    
    /** @brief Hämta specifik quest */
    const Quest* getQuest(const std::string& questId) const;
    
    /** @brief Markera en quest som klar */
    void completeQuest(const std::string& questId);
    
    /** @brief Markera ett objective som klart via ID */
    void completeObjectiveById(const std::string& questId, const std::string& objectiveId);
    
    /** @brief Sätt callback för quest completion */
    void setOnQuestComplete(std::function<void(const std::string&)> callback) {
        m_onQuestComplete = callback;
    }

private:
    QuestSystem() = default;
    
    void checkQuestCompletion(Quest& quest);
    
    std::unordered_map<std::string, Quest> m_quests;
    std::function<void(const std::string&)> m_onQuestComplete;
};
