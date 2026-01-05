/**
 * @file HintSystem.cpp
 * @brief Implementation av tiered hint system
 */
#include "HintSystem.h"
#include "QuestSystem.h"
#include "utils/Logger.h"

HintSystem& HintSystem::instance() {
    static HintSystem inst;
    return inst;
}

void HintSystem::registerHints(const std::string& questId,
                                const std::string& objectiveId,
                                const std::vector<std::string>& hints) {
    std::string key = makeKey(questId, objectiveId);
    
    HintData data;
    data.questId = questId;
    data.objectiveId = objectiveId;
    data.hints = hints;
    
    // Fyll ut till 4 nivåer om färre hints ges
    while (data.hints.size() < 4) {
        if (data.hints.empty()) {
            data.hints.push_back("Ingen hint tillgänglig.");
        } else {
            data.hints.push_back(data.hints.back());
        }
    }
    
    m_hints[key] = data;
    m_currentLevels[key] = 0;
    
    LOG_DEBUG("Registered hints for " + key + " (" + std::to_string(hints.size()) + " hints)");
}

std::string HintSystem::getHint(const std::string& questId,
                                 const std::string& objectiveId,
                                 HintLevel level) const {
    std::string key = makeKey(questId, objectiveId);
    
    auto it = m_hints.find(key);
    if (it == m_hints.end()) {
        return "Ingen hint tillgänglig för detta mål.";
    }
    
    int index = static_cast<int>(level);
    if (index >= 0 && index < static_cast<int>(it->second.hints.size())) {
        return it->second.hints[index];
    }
    
    return it->second.hints.back();
}

std::string HintSystem::requestHint(const std::string& questId,
                                     const std::string& objectiveId) {
    // Kolla cooldown
    if (isOnCooldown()) {
        int remaining = static_cast<int>(m_cooldownTimer);
        return "Vänta " + std::to_string(remaining) + " sekunder innan nästa hint.";
    }
    
    std::string key = makeKey(questId, objectiveId);
    
    // Hämta eller skapa nivå
    int& level = m_currentLevels[key];
    
    // Hämta hint
    std::string hint = getHint(questId, objectiveId, static_cast<HintLevel>(level));
    
    // Callback
    if (m_onHintShown) {
        m_onHintShown(hint, static_cast<HintLevel>(level));
    }
    
    LOG_INFO("Hint requested: " + key + " level=" + std::to_string(level));
    
    // Öka nivå för nästa gång (max 3)
    if (level < 3) {
        level++;
    }
    
    // Starta cooldown
    m_cooldownTimer = m_cooldown;
    
    return hint;
}

std::string HintSystem::requestHintForActiveQuest() {
    // Hämta aktivt quest från QuestSystem
    const auto& activeQuests = QuestSystem::instance().getActiveQuests();
    
    if (activeQuests.empty()) {
        return "Du har inga aktiva uppdrag just nu.";
    }
    
    // Hitta första ofullständiga objective
    for (const auto& quest : activeQuests) {
        for (const auto& obj : quest.objectives) {
            if (obj.status != QuestStatus::Completed) {
                return requestHint(quest.id, obj.id);
            }
        }
    }
    
    return "Alla dina nuvarande mål verkar vara klara!";
}

void HintSystem::resetHintLevel(const std::string& questId,
                                 const std::string& objectiveId) {
    std::string key = makeKey(questId, objectiveId);
    m_currentLevels[key] = 0;
    LOG_DEBUG("Reset hint level for " + key);
}

void HintSystem::resetAllHints() {
    for (auto& pair : m_currentLevels) {
        pair.second = 0;
    }
    m_cooldownTimer = 0.0f;
    LOG_INFO("Reset all hint levels");
}

HintLevel HintSystem::getCurrentLevel(const std::string& questId,
                                       const std::string& objectiveId) const {
    std::string key = makeKey(questId, objectiveId);
    
    auto it = m_currentLevels.find(key);
    if (it != m_currentLevels.end()) {
        return static_cast<HintLevel>(it->second);
    }
    
    return HintLevel::Goal;
}

bool HintSystem::isOnCooldown() const {
    return m_cooldownTimer > 0.0f;
}

void HintSystem::update(float deltaTime) {
    if (m_cooldownTimer > 0.0f) {
        m_cooldownTimer -= deltaTime;
        if (m_cooldownTimer < 0.0f) {
            m_cooldownTimer = 0.0f;
        }
    }
}

std::string HintSystem::makeKey(const std::string& questId,
                                 const std::string& objectiveId) const {
    return questId + ":" + objectiveId;
}
