/**
 * @file QuestGiverComponent.h
 * @brief Quest Giver Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <string>
#include <vector>

namespace engine {

/**
 * @brief Component for NPCs that give quests
 */
class QuestGiverComponent : public ActorComponent {
public:
    QuestGiverComponent(const std::string& name = "QuestGiverComponent");
    virtual ~QuestGiverComponent() = default;
    
    void addQuest(const std::string& questId) { m_availableQuests.push_back(questId); }
    const std::vector<std::string>& getAvailableQuests() const { return m_availableQuests; }
    
    bool hasQuests() const { return !m_availableQuests.empty(); }
    
    void setQuestDialogId(const std::string& dialogId) { m_questDialogId = dialogId; }
    const std::string& getQuestDialogId() const { return m_questDialogId; }
    
private:
    std::vector<std::string> m_availableQuests;
    std::string m_questDialogId;
};

} // namespace engine
