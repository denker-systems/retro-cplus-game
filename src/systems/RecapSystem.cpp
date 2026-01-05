/**
 * @file RecapSystem.cpp
 * @brief Implementation av recap barks system
 */
#include "RecapSystem.h"
#include "QuestSystem.h"
#include "../utils/Logger.h"

RecapSystem& RecapSystem::instance() {
    static RecapSystem inst;
    return inst;
}

void RecapSystem::update(float deltaTime) {
    if (!m_enabled) return;
    
    m_inactivityTimer += deltaTime;
    
    // Trigga recap om tillräckligt inaktiv och inte redan visat
    if (m_inactivityTimer >= m_recapDelay && !m_hasShownRecap) {
        std::string recap = getRecapForActiveQuest();
        
        if (!recap.empty() && m_onRecap) {
            m_onRecap(recap);
            m_hasShownRecap = true;
            LOG_DEBUG("Recap triggered after " + std::to_string(m_inactivityTimer) + " seconds");
        }
    }
}

void RecapSystem::resetTimer() {
    m_inactivityTimer = 0.0f;
    m_hasShownRecap = false;
}

void RecapSystem::registerRecaps(const std::string& questId,
                                  const std::string& objectiveId,
                                  const std::vector<std::string>& recaps) {
    std::string key = makeKey(questId, objectiveId);
    m_recaps[key] = recaps;
    LOG_DEBUG("Registered " + std::to_string(recaps.size()) + " recaps for " + key);
}

void RecapSystem::clearRecaps(const std::string& questId,
                               const std::string& objectiveId) {
    std::string key = makeKey(questId, objectiveId);
    m_recaps.erase(key);
}

void RecapSystem::clearAllRecaps() {
    m_recaps.clear();
    resetTimer();
}

std::string RecapSystem::makeKey(const std::string& questId,
                                  const std::string& objectiveId) const {
    return questId + ":" + objectiveId;
}

std::string RecapSystem::getRecapForActiveQuest() const {
    // Hämta aktivt quest från QuestSystem
    const auto& activeQuests = QuestSystem::instance().getActiveQuests();
    
    if (activeQuests.empty()) {
        return "";
    }
    
    // Hitta första ofullständiga objective
    for (const auto& quest : activeQuests) {
        for (const auto& obj : quest.objectives) {
            if (obj.status != QuestStatus::Completed) {
                std::string key = makeKey(quest.id, obj.id);
                
                auto it = m_recaps.find(key);
                if (it != m_recaps.end() && !it->second.empty()) {
                    // Returnera slumpmässigt recap
                    int index = rand() % it->second.size();
                    return it->second[index];
                }
                
                // Fallback: använd objective description
                return "Jag borde nog " + obj.description + "...";
            }
        }
    }
    
    return "";
}
