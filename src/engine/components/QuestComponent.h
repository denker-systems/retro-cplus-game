/**
 * @file QuestComponent.h
 * @brief Quest Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include <string>
#include <vector>

namespace engine {

/**
 * @brief Component for quest tracking
 */
class QuestComponent : public ActorComponent {
public:
    struct QuestObjective {
        std::string id;
        std::string description;
        bool completed = false;
    };
    
    QuestComponent(const std::string& name = "QuestComponent");
    virtual ~QuestComponent() = default;
    
    void setQuestId(const std::string& questId) { m_questId = questId; }
    const std::string& getQuestId() const { return m_questId; }
    
    void addObjective(const std::string& id, const std::string& description);
    void completeObjective(const std::string& id);
    bool isObjectiveComplete(const std::string& id) const;
    
    bool isQuestComplete() const;
    
    const std::vector<QuestObjective>& getObjectives() const { return m_objectives; }
    
private:
    std::string m_questId;
    std::vector<QuestObjective> m_objectives;
};

} // namespace engine
