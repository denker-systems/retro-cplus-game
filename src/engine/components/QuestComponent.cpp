/**
 * @file QuestComponent.cpp
 * @brief Quest Component Implementation
 */
#include "QuestComponent.h"

namespace engine {

QuestComponent::QuestComponent(const std::string& name)
    : ActorComponent(name)
{
}

void QuestComponent::addObjective(const std::string& id, const std::string& description) {
    QuestObjective obj;
    obj.id = id;
    obj.description = description;
    obj.completed = false;
    m_objectives.push_back(obj);
}

void QuestComponent::completeObjective(const std::string& id) {
    for (auto& obj : m_objectives) {
        if (obj.id == id) {
            obj.completed = true;
            break;
        }
    }
}

bool QuestComponent::isObjectiveComplete(const std::string& id) const {
    for (const auto& obj : m_objectives) {
        if (obj.id == id) {
            return obj.completed;
        }
    }
    return false;
}

bool QuestComponent::isQuestComplete() const {
    for (const auto& obj : m_objectives) {
        if (!obj.completed) {
            return false;
        }
    }
    return !m_objectives.empty();
}

} // namespace engine
