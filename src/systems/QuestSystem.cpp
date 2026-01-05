/**
 * @file QuestSystem.cpp
 * @brief Implementation av QuestSystem
 */
#include "QuestSystem.h"
#include <iostream>

QuestSystem& QuestSystem::instance() {
    static QuestSystem instance;
    return instance;
}

void QuestSystem::addQuest(const Quest& quest) {
    m_quests[quest.id] = quest;
    std::cout << "Quest registered: " << quest.title << std::endl;
}

bool QuestSystem::startQuest(const std::string& questId) {
    auto it = m_quests.find(questId);
    if (it == m_quests.end()) {
        std::cerr << "Quest not found: " << questId << std::endl;
        return false;
    }
    
    if (it->second.status != QuestStatus::Inactive) {
        return false;  // Redan startad
    }
    
    it->second.status = QuestStatus::Active;
    for (auto& obj : it->second.objectives) {
        obj.status = QuestStatus::Active;
    }
    
    std::cout << "Quest started: " << it->second.title << std::endl;
    return true;
}

void QuestSystem::updateObjective(ObjectiveType type, const std::string& targetId, int count) {
    for (auto& [id, quest] : m_quests) {
        if (quest.status != QuestStatus::Active) continue;
        
        for (auto& obj : quest.objectives) {
            if (obj.status != QuestStatus::Active) continue;
            if (obj.type != type) continue;
            if (obj.targetId != targetId) continue;
            
            obj.currentCount += count;
            std::cout << "Objective progress: " << obj.description 
                      << " (" << obj.currentCount << "/" << obj.requiredCount << ")" << std::endl;
            
            if (obj.currentCount >= obj.requiredCount) {
                obj.status = QuestStatus::Completed;
                std::cout << "Objective completed: " << obj.description << std::endl;
            }
        }
        
        checkQuestCompletion(quest);
    }
}

void QuestSystem::checkQuestCompletion(Quest& quest) {
    bool allComplete = true;
    
    for (const auto& obj : quest.objectives) {
        if (obj.optional) continue;
        if (obj.status != QuestStatus::Completed) {
            allComplete = false;
            break;
        }
    }
    
    if (allComplete && quest.status == QuestStatus::Active) {
        quest.status = QuestStatus::Completed;
        std::cout << "Quest completed: " << quest.title << std::endl;
        
        if (m_onQuestComplete) {
            m_onQuestComplete(quest.id);
        }
    }
}

bool QuestSystem::isQuestActive(const std::string& questId) const {
    auto it = m_quests.find(questId);
    return it != m_quests.end() && it->second.status == QuestStatus::Active;
}

bool QuestSystem::isQuestCompleted(const std::string& questId) const {
    auto it = m_quests.find(questId);
    return it != m_quests.end() && it->second.status == QuestStatus::Completed;
}

std::vector<const Quest*> QuestSystem::getActiveQuests() const {
    std::vector<const Quest*> active;
    for (const auto& [id, quest] : m_quests) {
        if (quest.status == QuestStatus::Active) {
            active.push_back(&quest);
        }
    }
    return active;
}

std::vector<const Quest*> QuestSystem::getAllQuests() const {
    std::vector<const Quest*> all;
    for (const auto& [id, quest] : m_quests) {
        if (quest.status != QuestStatus::Inactive) {
            all.push_back(&quest);
        }
    }
    return all;
}

const Quest* QuestSystem::getQuest(const std::string& questId) const {
    auto it = m_quests.find(questId);
    return it != m_quests.end() ? &it->second : nullptr;
}
