/**
 * @file DialogTools.cpp
 * @brief Implementation of AI tools for dialog manipulation
 */
#include "DialogTools.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"

namespace ai {

ToolResult ListDialogsTool::execute(const nlohmann::json& params) {
    const auto& dialogs = DataLoader::instance().getDialogs();
    
    nlohmann::json dialogList = nlohmann::json::array();
    for (const auto& dialog : dialogs) {
        dialogList.push_back({
            {"id", dialog.id},
            {"npc_name", dialog.npcName},
            {"node_count", dialog.nodes.size()},
            {"start_node_id", dialog.startNodeId}
        });
    }
    
    nlohmann::json result = {
        {"dialog_count", dialogs.size()},
        {"dialogs", dialogList}
    };
    
    return ToolResult::ok("Found " + std::to_string(dialogs.size()) + " dialogs", result);
}

ToolResult GetDialogTool::execute(const nlohmann::json& params) {
    std::string dialogId = params.value("dialog_id", "");
    
    if (dialogId.empty()) {
        return ToolResult::error("dialog_id is required");
    }
    
    const DialogData* dialog = DataLoader::getDialogById(dialogId);
    if (!dialog) {
        return ToolResult::error("Dialog not found: " + dialogId);
    }
    
    nlohmann::json nodes = nlohmann::json::array();
    for (const auto& node : dialog->nodes) {
        nlohmann::json nodeJson = {
            {"id", node.id},
            {"speaker", node.speaker},
            {"text", node.text},
            {"next_node_id", node.nextNodeId},
            {"action", node.action}
        };
        
        if (!node.choices.empty()) {
            nlohmann::json choices = nlohmann::json::array();
            for (const auto& choice : node.choices) {
                choices.push_back({
                    {"text", choice.text},
                    {"next_node_id", choice.nextNodeId},
                    {"tone", choice.tone},
                    {"condition", choice.condition}
                });
            }
            nodeJson["choices"] = choices;
        }
        
        nodes.push_back(nodeJson);
    }
    
    nlohmann::json result = {
        {"id", dialog->id},
        {"npc_name", dialog->npcName},
        {"start_node_id", dialog->startNodeId},
        {"nodes", nodes}
    };
    
    return ToolResult::ok("Dialog '" + dialogId + "' has " + std::to_string(dialog->nodes.size()) + " nodes", result);
}

ToolResult CreateDialogTool::execute(const nlohmann::json& params) {
    std::string id = params.value("id", "");
    std::string npcName = params.value("npc_name", "");
    std::string initialText = params.value("initial_text", "");
    
    if (id.empty() || npcName.empty() || initialText.empty()) {
        return ToolResult::error("id, npc_name, and initial_text are required");
    }
    
    // Check for duplicate ID
    if (DataLoader::getDialogById(id) != nullptr) {
        return ToolResult::error("Dialog with ID '" + id + "' already exists");
    }
    
    // Create new dialog with initial node
    DialogData dialog;
    dialog.id = id;
    dialog.npcName = npcName;
    dialog.startNodeId = 0;
    
    DialogNodeData initialNode;
    initialNode.id = 0;
    initialNode.speaker = npcName;
    initialNode.text = initialText;
    initialNode.nextNodeId = -1;  // End dialog by default
    
    dialog.nodes.push_back(initialNode);
    
    // Add to DataLoader
    if (!DataLoader::addDialog(dialog)) {
        return ToolResult::error("Failed to add dialog");
    }
    
    // Save to file
    if (!DataLoader::saveDialogs()) {
        return ToolResult::error("Failed to save dialog data");
    }
    
    LOG_INFO("[AI] Created dialog '" + id + "' for NPC '" + npcName + "'");
    
    nlohmann::json result = {
        {"id", dialog.id},
        {"npc_name", dialog.npcName},
        {"initial_node", {
            {"id", 0},
            {"text", initialText}
        }}
    };
    
    return ToolResult::ok("Created dialog '" + id + "'", result);
}

ToolResult AddDialogNodeTool::execute(const nlohmann::json& params) {
    std::string dialogId = params.value("dialog_id", "");
    std::string speaker = params.value("speaker", "");
    std::string text = params.value("text", "");
    
    if (dialogId.empty() || speaker.empty() || text.empty()) {
        return ToolResult::error("dialog_id, speaker, and text are required");
    }
    
    DialogData* dialog = DataLoader::getDialogByIdMutable(dialogId);
    if (!dialog) {
        return ToolResult::error("Dialog not found: " + dialogId);
    }
    
    // Find next available node ID
    int nextId = 0;
    for (const auto& node : dialog->nodes) {
        if (node.id >= nextId) {
            nextId = node.id + 1;
        }
    }
    
    // Create new node
    DialogNodeData newNode;
    newNode.id = nextId;
    newNode.speaker = speaker;
    newNode.text = text;
    newNode.nextNodeId = params.value("next_node_id", -1);
    
    // Add choices if provided
    if (params.contains("choices")) {
        for (const auto& choiceJson : params["choices"]) {
            DialogChoiceData choice;
            choice.text = choiceJson.value("text", "");
            choice.nextNodeId = choiceJson.value("next_node_id", -1);
            choice.tone = choiceJson.value("tone", "neutral");
            newNode.choices.push_back(choice);
        }
    }
    
    dialog->nodes.push_back(newNode);
    
    // Save to file
    if (!DataLoader::saveDialogs()) {
        return ToolResult::error("Failed to save dialog data");
    }
    
    LOG_INFO("[AI] Added node " + std::to_string(nextId) + " to dialog '" + dialogId + "'");
    
    nlohmann::json result = {
        {"dialog_id", dialogId},
        {"node", {
            {"id", newNode.id},
            {"speaker", newNode.speaker},
            {"text", newNode.text}
        }}
    };
    
    return ToolResult::ok("Added node " + std::to_string(nextId) + " to dialog", result);
}

} // namespace ai
