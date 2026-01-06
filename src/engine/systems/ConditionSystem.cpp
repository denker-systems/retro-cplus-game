/**
 * @file ConditionSystem.cpp
 * @brief Implementation av data-driven conditions och actions
 */
#include "ConditionSystem.h"
#include "WorldState.h"
#include "InventorySystem.h"
#include "QuestSystem.h"
#include "SceneManager.h"
#include "DialogSystem.h"
#include "CutsceneSystem.h"
#include "GateSystem.h"
#include "audio/AudioManager.h"
#include "utils/Logger.h"

// ============================================================================
// CONDITION EVALUATOR
// ============================================================================

ConditionEvaluator& ConditionEvaluator::instance() {
    static ConditionEvaluator inst;
    return inst;
}

bool ConditionEvaluator::evaluate(const Condition& condition) const {
    auto& world = WorldState::instance();
    
    switch (condition.type) {
        case ConditionType::FlagEquals:
            return world.getFlag(condition.key) == condition.boolValue;
            
        case ConditionType::FlagSet:
            return world.getFlag(condition.key);
            
        case ConditionType::CounterEquals:
            return world.getCounter(condition.key) == condition.intValue;
            
        case ConditionType::CounterGreater:
            return world.getCounter(condition.key) > condition.intValue;
            
        case ConditionType::CounterLess:
            return world.getCounter(condition.key) < condition.intValue;
            
        case ConditionType::HasItem:
            return InventorySystem::instance().hasItem(condition.key);
            
        case ConditionType::QuestActive: {
            auto* quest = QuestSystem::instance().getQuest(condition.key);
            return quest && quest->status == QuestStatus::Active;
        }
            
        case ConditionType::QuestCompleted: {
            auto* quest = QuestSystem::instance().getQuest(condition.key);
            return quest && quest->status == QuestStatus::Completed;
        }
            
        case ConditionType::InRoom: {
            auto* scene = SceneManager::instance().getCurrentScene();
            return scene && scene->getId() == condition.key;
        }
            
        case ConditionType::And:
            return evaluateAll(condition.subConditions);
            
        case ConditionType::Or:
            return evaluateAny(condition.subConditions);
            
        case ConditionType::Not:
            return !condition.subConditions.empty() && 
                   !evaluate(condition.subConditions[0]);
            
        default:
            LOG_WARNING("Unknown condition type");
            return false;
    }
}

bool ConditionEvaluator::evaluateAll(const std::vector<Condition>& conditions) const {
    for (const auto& cond : conditions) {
        if (!evaluate(cond)) {
            return false;
        }
    }
    return true;
}

bool ConditionEvaluator::evaluateAny(const std::vector<Condition>& conditions) const {
    for (const auto& cond : conditions) {
        if (evaluate(cond)) {
            return true;
        }
    }
    return conditions.empty();
}

Condition ConditionEvaluator::parseCondition(const json& j) {
    Condition cond;
    
    if (j.contains("type")) {
        cond.type = j["type"].get<ConditionType>();
    }
    
    if (j.contains("key")) {
        cond.key = j["key"].get<std::string>();
    } else if (j.contains("flag")) {
        cond.key = j["flag"].get<std::string>();
    } else if (j.contains("item")) {
        cond.key = j["item"].get<std::string>();
    } else if (j.contains("quest")) {
        cond.key = j["quest"].get<std::string>();
    } else if (j.contains("room")) {
        cond.key = j["room"].get<std::string>();
    } else if (j.contains("counter")) {
        cond.key = j["counter"].get<std::string>();
    }
    
    if (j.contains("value")) {
        auto& val = j["value"];
        if (val.is_boolean()) {
            cond.boolValue = val.get<bool>();
        } else if (val.is_number_integer()) {
            cond.intValue = val.get<int>();
        } else if (val.is_string()) {
            cond.stringValue = val.get<std::string>();
        }
    }
    
    if (j.contains("conditions")) {
        cond.subConditions = parseConditions(j["conditions"]);
    }
    
    return cond;
}

std::vector<Condition> ConditionEvaluator::parseConditions(const json& j) {
    std::vector<Condition> conditions;
    
    if (j.is_array()) {
        for (const auto& item : j) {
            conditions.push_back(parseCondition(item));
        }
    }
    
    return conditions;
}

// ============================================================================
// ACTION EXECUTOR
// ============================================================================

ActionExecutor& ActionExecutor::instance() {
    static ActionExecutor inst;
    return inst;
}

void ActionExecutor::execute(const Action& action) {
    LOG_DEBUG("Executing action: " + std::to_string(static_cast<int>(action.type)) + 
              " target=" + action.target);
    
    switch (action.type) {
        case ActionType::SetFlag:
            WorldState::instance().setFlag(action.target, action.boolValue);
            break;
            
        case ActionType::SetCounter:
            WorldState::instance().setCounter(action.target, action.intValue);
            break;
            
        case ActionType::IncrementCounter:
            WorldState::instance().incrementCounter(action.target, 
                action.intValue != 0 ? action.intValue : 1);
            break;
            
        case ActionType::DecrementCounter:
            WorldState::instance().decrementCounter(action.target,
                action.intValue != 0 ? action.intValue : 1);
            break;
            
        case ActionType::GiveItem:
            InventorySystem::instance().addItem(action.target);
            break;
            
        case ActionType::RemoveItem:
            InventorySystem::instance().removeItem(action.target);
            break;
            
        case ActionType::StartQuest:
            QuestSystem::instance().startQuest(action.target);
            break;
            
        case ActionType::CompleteObjective:
            // target = "quest_id:objective_id"
            {
                size_t pos = action.target.find(':');
                if (pos != std::string::npos) {
                    std::string questId = action.target.substr(0, pos);
                    std::string objId = action.target.substr(pos + 1);
                    QuestSystem::instance().completeObjectiveById(questId, objId);
                }
            }
            break;
            
        case ActionType::CompleteQuest:
            QuestSystem::instance().completeQuest(action.target);
            break;
            
        case ActionType::ChangeRoom:
            SceneManager::instance().changeScene(action.target);
            break;
            
        case ActionType::StartDialog:
            DialogSystem::instance().startDialog(action.target);
            break;
            
        case ActionType::StartCutscene:
            CutsceneSystem::instance().play(action.target);
            break;
            
        case ActionType::PlaySound:
            AudioManager::instance().playSound(action.target);
            break;
            
        case ActionType::PlayMusic:
            AudioManager::instance().crossfadeToMusic(action.target, 1000);
            break;
            
        case ActionType::ShowMessage:
            // TODO: Implementera message display
            LOG_INFO("Message: " + action.target);
            break;
            
        case ActionType::OpenGate:
            GateSystem::instance().tryOpenGate(action.target, ApproachType::Social);
            break;
            
        case ActionType::Custom:
            {
                auto it = m_customHandlers.find(action.target);
                if (it != m_customHandlers.end()) {
                    it->second(action.target, action.stringValue);
                } else {
                    LOG_WARNING("Unknown custom action: " + action.target);
                }
            }
            break;
            
        default:
            LOG_WARNING("Unhandled action type");
            break;
    }
}

void ActionExecutor::executeAll(const std::vector<Action>& actions) {
    for (const auto& action : actions) {
        execute(action);
    }
}

Action ActionExecutor::parseAction(const json& j) {
    Action action;
    
    if (j.contains("type")) {
        action.type = j["type"].get<ActionType>();
    }
    
    if (j.contains("target")) {
        action.target = j["target"].get<std::string>();
    } else if (j.contains("flag")) {
        action.target = j["flag"].get<std::string>();
    } else if (j.contains("item")) {
        action.target = j["item"].get<std::string>();
    } else if (j.contains("quest")) {
        action.target = j["quest"].get<std::string>();
    } else if (j.contains("room")) {
        action.target = j["room"].get<std::string>();
    } else if (j.contains("dialog")) {
        action.target = j["dialog"].get<std::string>();
    } else if (j.contains("sound")) {
        action.target = j["sound"].get<std::string>();
    } else if (j.contains("counter")) {
        action.target = j["counter"].get<std::string>();
    }
    
    if (j.contains("value")) {
        auto& val = j["value"];
        if (val.is_boolean()) {
            action.boolValue = val.get<bool>();
        } else if (val.is_number_integer()) {
            action.intValue = val.get<int>();
        } else if (val.is_string()) {
            action.stringValue = val.get<std::string>();
        }
    }
    
    if (j.contains("amount")) {
        action.intValue = j["amount"].get<int>();
    }
    
    return action;
}

std::vector<Action> ActionExecutor::parseActions(const json& j) {
    std::vector<Action> actions;
    
    if (j.is_array()) {
        for (const auto& item : j) {
            actions.push_back(parseAction(item));
        }
    }
    
    return actions;
}

void ActionExecutor::registerCustomAction(const std::string& name, CustomHandler handler) {
    m_customHandlers[name] = handler;
    LOG_DEBUG("Registered custom action: " + name);
}
