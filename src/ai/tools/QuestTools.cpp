/**
 * @file QuestTools.cpp
 * @brief Implementation of quest management tools
 */
#include "QuestTools.h"
#include "engine/data/DataLoader.h"
#include "engine/data/GameData.h"
#include "engine/utils/Logger.h"

namespace ai {

// Helper to find quest by ID
static QuestData* findQuestById(const std::string& id) {
    auto& quests = DataLoader::instance().getQuests();
    for (size_t i = 0; i < quests.size(); ++i) {
        if (quests[i].id == id) {
            return &quests[i];
        }
    }
    return nullptr;
}

ToolResult ListQuestsTool::execute(const nlohmann::json& params) {
    (void)params;
    
    const auto& quests = DataLoader::instance().getQuests();
    
    if (quests.empty()) {
        return ToolResult::ok("No quests found", {{"count", 0}});
    }
    
    nlohmann::json questList = nlohmann::json::array();
    for (const auto& quest : quests) {
        questList.push_back({
            {"id", quest.id},
            {"title", quest.title},
            {"description", quest.description},
            {"objective_count", quest.objectives.size()},
            {"auto_start", quest.autoStart}
        });
    }
    
    std::string summary = "Found " + std::to_string(quests.size()) + " quests:\n";
    for (const auto& quest : quests) {
        summary += "- " + quest.title + " (" + quest.id + "): " + 
                   std::to_string(quest.objectives.size()) + " objectives\n";
    }
    
    return ToolResult::ok(summary, {
        {"quests", questList},
        {"count", quests.size()}
    });
}

ToolResult GetQuestTool::execute(const nlohmann::json& params) {
    if (!params.contains("quest_id")) {
        return ToolResult::error("quest_id is required");
    }
    
    std::string questId = params["quest_id"].get<std::string>();
    
    const auto& quests = DataLoader::instance().getQuests();
    for (const auto& quest : quests) {
        if (quest.id == questId) {
            nlohmann::json objectives = nlohmann::json::array();
            for (const auto& obj : quest.objectives) {
                objectives.push_back({
                    {"id", obj.id},
                    {"description", obj.description},
                    {"type", obj.type},
                    {"target_id", obj.targetId},
                    {"required_count", obj.requiredCount},
                    {"optional", obj.optional}
                });
            }
            
            std::string summary = "Quest: " + quest.title + "\n";
            summary += "Description: " + quest.description + "\n";
            summary += "Objectives:\n";
            for (const auto& obj : quest.objectives) {
                summary += "  - " + obj.description + " (" + obj.type + ": " + obj.targetId + ")\n";
            }
            
            return ToolResult::ok(summary, {
                {"id", quest.id},
                {"title", quest.title},
                {"description", quest.description},
                {"objectives", objectives},
                {"reward_item", quest.rewardItem},
                {"reward_xp", quest.rewardXP},
                {"auto_start", quest.autoStart}
            });
        }
    }
    
    return ToolResult::error("Quest not found: " + questId);
}

ToolResult CreateQuestTool::execute(const nlohmann::json& params) {
    if (!params.contains("id") || !params.contains("title") || !params.contains("description")) {
        return ToolResult::error("id, title, and description are required");
    }
    
    std::string id = params["id"].get<std::string>();
    std::string title = params["title"].get<std::string>();
    std::string description = params["description"].get<std::string>();
    
    // Check if quest already exists
    if (findQuestById(id)) {
        return ToolResult::error("Quest with ID '" + id + "' already exists");
    }
    
    // Create new quest
    QuestData newQuest;
    newQuest.id = id;
    newQuest.title = title;
    newQuest.description = description;
    newQuest.rewardItem = params.value("reward_item", "");
    newQuest.rewardXP = params.value("reward_xp", 0);
    newQuest.autoStart = params.value("auto_start", false);
    
    // Add to DataLoader
    DataLoader::instance().getQuests().push_back(newQuest);
    
    // Save to file
    DataLoader::saveQuests();
    
    LOG_INFO("[AI] Created quest: " + title + " (ID: " + id + ")");
    
    return ToolResult::ok("Quest created: " + title + " (ID: " + id + ")", {
        {"id", id},
        {"title", title},
        {"description", description}
    });
}

ToolResult AddQuestObjectiveTool::execute(const nlohmann::json& params) {
    if (!params.contains("quest_id") || !params.contains("objective_id") ||
        !params.contains("description") || !params.contains("type") ||
        !params.contains("target_id")) {
        return ToolResult::error("quest_id, objective_id, description, type, and target_id are required");
    }
    
    std::string questId = params["quest_id"].get<std::string>();
    std::string objectiveId = params["objective_id"].get<std::string>();
    std::string description = params["description"].get<std::string>();
    std::string type = params["type"].get<std::string>();
    std::string targetId = params["target_id"].get<std::string>();
    int requiredCount = params.value("required_count", 1);
    bool optional = params.value("optional", false);
    
    // Find quest
    QuestData* quest = findQuestById(questId);
    if (!quest) {
        return ToolResult::error("Quest not found: " + questId);
    }
    
    // Check if objective already exists
    for (const auto& obj : quest->objectives) {
        if (obj.id == objectiveId) {
            return ToolResult::error("Objective '" + objectiveId + "' already exists in quest");
        }
    }
    
    // Create objective
    ObjectiveData newObjective;
    newObjective.id = objectiveId;
    newObjective.description = description;
    newObjective.type = type;
    newObjective.targetId = targetId;
    newObjective.requiredCount = requiredCount;
    newObjective.optional = optional;
    
    // Add to quest
    quest->objectives.push_back(newObjective);
    
    // Save
    DataLoader::saveQuests();
    
    LOG_INFO("[AI] Added objective '" + objectiveId + "' to quest '" + questId + "'");
    
    return ToolResult::ok("Objective added: " + description, {
        {"quest_id", questId},
        {"objective_id", objectiveId},
        {"type", type},
        {"target_id", targetId}
    });
}

ToolResult LinkQuestToNPCTool::execute(const nlohmann::json& params) {
    if (!params.contains("quest_id") || !params.contains("npc_id")) {
        return ToolResult::error("quest_id and npc_id are required");
    }
    
    std::string questId = params["quest_id"].get<std::string>();
    std::string npcId = params["npc_id"].get<std::string>();
    std::string dialogId = params.value("dialog_id", "");
    
    // Verify quest exists
    const QuestData* quest = findQuestById(questId);
    if (!quest) {
        return ToolResult::error("Quest not found: " + questId);
    }
    
    // Find NPC
    auto& npcs = DataLoader::instance().getNPCs();
    NPCData* npc = nullptr;
    for (size_t i = 0; i < npcs.size(); ++i) {
        if (npcs[i].id == npcId) {
            npc = &npcs[i];
            break;
        }
    }
    
    if (!npc) {
        return ToolResult::error("NPC not found: " + npcId);
    }
    
    // Link dialog to NPC if provided
    if (!dialogId.empty()) {
        npc->dialogId = dialogId;
    }
    
    // Save
    DataLoader::saveNPCs();
    
    LOG_INFO("[AI] Linked quest '" + questId + "' to NPC '" + npcId + "'");
    
    std::string message = "Quest '" + quest->title + "' linked to NPC '" + npc->name + "'";
    if (!dialogId.empty()) {
        message += " with dialog '" + dialogId + "'";
    }
    
    return ToolResult::ok(message, {
        {"quest_id", questId},
        {"npc_id", npcId},
        {"dialog_id", dialogId}
    });
}

} // namespace ai
