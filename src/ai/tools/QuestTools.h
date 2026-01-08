/**
 * @file QuestTools.h
 * @brief AI tools for quest creation and management
 */
#pragma once

#include "ai/core/IEditorTool.h"

namespace ai {

/**
 * @class ListQuestsTool
 * @brief List all quests in the game
 */
class ListQuestsTool : public IEditorTool {
public:
    const char* getName() const override { return "list_quests"; }
    const char* getDescription() const override { 
        return "List all quests in the game"; 
    }
    const char* getCategory() const override { return "quests"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", nlohmann::json::object()},
            {"required", nlohmann::json::array()}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class GetQuestTool
 * @brief Get details of a specific quest
 */
class GetQuestTool : public IEditorTool {
public:
    const char* getName() const override { return "get_quest"; }
    const char* getDescription() const override { 
        return "Get detailed information about a specific quest including all objectives"; 
    }
    const char* getCategory() const override { return "quests"; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"quest_id", {
                    {"type", "string"},
                    {"description", "ID of the quest to get"}
                }}
            }},
            {"required", {"quest_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class CreateQuestTool
 * @brief Create a new quest
 */
class CreateQuestTool : public IEditorTool {
public:
    const char* getName() const override { return "create_quest"; }
    const char* getDescription() const override { 
        return "Create a new quest with title, description and optional objectives"; 
    }
    const char* getCategory() const override { return "quests"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"id", {
                    {"type", "string"},
                    {"description", "Unique quest ID (e.g., 'find_key', 'save_princess')"}
                }},
                {"title", {
                    {"type", "string"},
                    {"description", "Display title for the quest"}
                }},
                {"description", {
                    {"type", "string"},
                    {"description", "Full description of the quest"}
                }},
                {"reward_item", {
                    {"type", "string"},
                    {"description", "Item ID to give as reward (optional)"}
                }},
                {"reward_xp", {
                    {"type", "integer"},
                    {"description", "XP reward amount (optional)"}
                }},
                {"auto_start", {
                    {"type", "boolean"},
                    {"description", "Start quest automatically at game start (default: false)"}
                }}
            }},
            {"required", {"id", "title", "description"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class AddQuestObjectiveTool
 * @brief Add an objective to a quest
 */
class AddQuestObjectiveTool : public IEditorTool {
public:
    const char* getName() const override { return "add_quest_objective"; }
    const char* getDescription() const override { 
        return "Add a new objective to an existing quest"; 
    }
    const char* getCategory() const override { return "quests"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"quest_id", {
                    {"type", "string"},
                    {"description", "ID of the quest to add objective to"}
                }},
                {"objective_id", {
                    {"type", "string"},
                    {"description", "Unique ID for this objective within the quest"}
                }},
                {"description", {
                    {"type", "string"},
                    {"description", "Description shown to player"}
                }},
                {"type", {
                    {"type", "string"},
                    {"enum", {"talk", "collect", "deliver", "goto", "examine", "kill", "use"}},
                    {"description", "Type of objective: talk (to NPC), collect (item), deliver (item to NPC), goto (scene), examine (hotspot), kill (enemy), use (item on target)"}
                }},
                {"target_id", {
                    {"type", "string"},
                    {"description", "ID of the target (NPC, item, scene, or hotspot depending on type)"}
                }},
                {"required_count", {
                    {"type", "integer"},
                    {"description", "Number required to complete (default: 1)"}
                }},
                {"optional", {
                    {"type", "boolean"},
                    {"description", "Is this objective optional? (default: false)"}
                }}
            }},
            {"required", {"quest_id", "objective_id", "description", "type", "target_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

/**
 * @class LinkQuestToNPCTool
 * @brief Link a quest to an NPC (quest giver)
 */
class LinkQuestToNPCTool : public IEditorTool {
public:
    const char* getName() const override { return "link_quest_to_npc"; }
    const char* getDescription() const override { 
        return "Link a quest to an NPC so they become the quest giver"; 
    }
    const char* getCategory() const override { return "quests"; }
    bool requiresConfirmation() const override { return true; }
    
    nlohmann::json getParameterSchema() const override {
        return {
            {"type", "object"},
            {"properties", {
                {"quest_id", {
                    {"type", "string"},
                    {"description", "ID of the quest"}
                }},
                {"npc_id", {
                    {"type", "string"},
                    {"description", "ID of the NPC who gives the quest"}
                }},
                {"dialog_id", {
                    {"type", "string"},
                    {"description", "Dialog ID to use for quest introduction (optional)"}
                }}
            }},
            {"required", {"quest_id", "npc_id"}}
        };
    }
    
    ToolResult execute(const nlohmann::json& params) override;
};

} // namespace ai
