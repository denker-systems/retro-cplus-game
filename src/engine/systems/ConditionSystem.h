/**
 * @file ConditionSystem.h
 * @brief Data-driven conditions och actions för narrative logic
 * 
 * Ersätter std::function lambdas med serialiserbara JSON-strukturer.
 * Gör det möjligt att skapa content utan C++-ändringar.
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// ============================================================================
// CONDITIONS
// ============================================================================

/**
 * @brief Typ av condition
 */
enum class ConditionType {
    FlagEquals,      // flag == value
    FlagSet,         // flag exists and is true
    CounterEquals,   // counter == value
    CounterGreater,  // counter > value
    CounterLess,     // counter < value
    HasItem,         // player has item
    QuestActive,     // quest is active
    QuestCompleted,  // quest is completed
    InRoom,          // player is in room
    And,             // all sub-conditions true
    Or,              // any sub-condition true
    Not              // invert sub-condition
};

/**
 * @brief En condition som kan evalueras mot WorldState
 */
struct Condition {
    ConditionType type;
    std::string key;           // flag/counter/item/quest/room id
    std::string stringValue;   // för string-jämförelser
    int intValue = 0;          // för counter-jämförelser
    bool boolValue = true;     // för flag-jämförelser
    std::vector<Condition> subConditions;  // för And/Or/Not
};

/**
 * @brief Evaluerar conditions mot WorldState
 */
class ConditionEvaluator {
public:
    static ConditionEvaluator& instance();
    
    /** @brief Evaluera en condition */
    bool evaluate(const Condition& condition) const;
    
    /** @brief Evaluera alla conditions (AND) */
    bool evaluateAll(const std::vector<Condition>& conditions) const;
    
    /** @brief Evaluera alla conditions (OR) */
    bool evaluateAny(const std::vector<Condition>& conditions) const;
    
    /** @brief Parsa condition från JSON */
    static Condition parseCondition(const json& j);
    static std::vector<Condition> parseConditions(const json& j);

private:
    ConditionEvaluator() = default;
};

// ============================================================================
// ACTIONS
// ============================================================================

/**
 * @brief Typ av action
 */
enum class ActionType {
    SetFlag,           // set flag to value
    SetCounter,        // set counter to value
    IncrementCounter,  // counter += amount
    DecrementCounter,  // counter -= amount
    GiveItem,          // add item to inventory
    RemoveItem,        // remove item from inventory
    StartQuest,        // activate quest
    CompleteObjective, // complete quest objective
    CompleteQuest,     // complete entire quest
    ChangeRoom,        // change to room
    StartDialog,       // start dialog tree
    StartCutscene,     // start cutscene
    PlaySound,         // play sound effect
    PlayMusic,         // change music
    ShowMessage,       // show text message
    OpenGate,          // open a gate
    Custom             // custom action with callback
};

/**
 * @brief En action som kan exekveras
 */
struct Action {
    ActionType type;
    std::string target;        // item/quest/room/dialog/sound id
    std::string stringValue;   // additional string param
    int intValue = 0;          // additional int param
    bool boolValue = true;     // additional bool param
};

/**
 * @brief Exekverar actions mot game systems
 */
class ActionExecutor {
public:
    static ActionExecutor& instance();
    
    /** @brief Exekvera en action */
    void execute(const Action& action);
    
    /** @brief Exekvera alla actions i sekvens */
    void executeAll(const std::vector<Action>& actions);
    
    /** @brief Parsa action från JSON */
    static Action parseAction(const json& j);
    static std::vector<Action> parseActions(const json& j);
    
    /** @brief Registrera custom action handler */
    using CustomHandler = std::function<void(const std::string&, const std::string&)>;
    void registerCustomAction(const std::string& name, CustomHandler handler);

private:
    ActionExecutor() = default;
    
    std::unordered_map<std::string, CustomHandler> m_customHandlers;
};

// ============================================================================
// JSON SERIALIZATION
// ============================================================================

NLOHMANN_JSON_SERIALIZE_ENUM(ConditionType, {
    {ConditionType::FlagEquals, "FlagEquals"},
    {ConditionType::FlagSet, "FlagSet"},
    {ConditionType::CounterEquals, "CounterEquals"},
    {ConditionType::CounterGreater, "CounterGreater"},
    {ConditionType::CounterLess, "CounterLess"},
    {ConditionType::HasItem, "HasItem"},
    {ConditionType::QuestActive, "QuestActive"},
    {ConditionType::QuestCompleted, "QuestCompleted"},
    {ConditionType::InRoom, "InRoom"},
    {ConditionType::And, "And"},
    {ConditionType::Or, "Or"},
    {ConditionType::Not, "Not"}
})

NLOHMANN_JSON_SERIALIZE_ENUM(ActionType, {
    {ActionType::SetFlag, "SetFlag"},
    {ActionType::SetCounter, "SetCounter"},
    {ActionType::IncrementCounter, "IncrementCounter"},
    {ActionType::DecrementCounter, "DecrementCounter"},
    {ActionType::GiveItem, "GiveItem"},
    {ActionType::RemoveItem, "RemoveItem"},
    {ActionType::StartQuest, "StartQuest"},
    {ActionType::CompleteObjective, "CompleteObjective"},
    {ActionType::CompleteQuest, "CompleteQuest"},
    {ActionType::ChangeRoom, "ChangeRoom"},
    {ActionType::StartDialog, "StartDialog"},
    {ActionType::StartCutscene, "StartCutscene"},
    {ActionType::PlaySound, "PlaySound"},
    {ActionType::PlayMusic, "PlayMusic"},
    {ActionType::ShowMessage, "ShowMessage"},
    {ActionType::OpenGate, "OpenGate"},
    {ActionType::Custom, "Custom"}
})
