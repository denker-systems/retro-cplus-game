/**
 * @file QuestNode.cpp
 * @brief Implementation of quest graph nodes
 */
#include "QuestNode.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

#ifdef HAS_IMNODES
#include <imnodes/imnodes.h>
#endif

// ============================================================================
// QuestHeaderNode
// ============================================================================

QuestHeaderNode::QuestHeaderNode(const QuestData& data, int nodeId) {
    id = nodeId;
    questId = data.id;
    title = data.title;
    description = data.description;
    rewardItem = data.rewardItem;
    rewardXP = data.rewardXP;
}

void QuestHeaderNode::renderContent() {
#ifdef HAS_IMNODES
    const float nodeWidth = 280.0f;
    
    // Description
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + nodeWidth);
    ImGui::TextWrapped("%s", description.c_str());
    ImGui::PopTextWrapPos();
    
    ImGui::Spacing();
    
    // Rewards
    if (!rewardItem.empty() || rewardXP > 0) {
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 0.84f, 0.0f, 1.0f), "Rewards:");
        if (!rewardItem.empty()) {
            ImGui::Text("  Item: %s", rewardItem.c_str());
        }
        if (rewardXP > 0) {
            ImGui::Text("  XP: %d", rewardXP);
        }
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("Objectives:");
    
    // Output attributes for each objective
    for (size_t i = 0; i < outputAttrIds.size(); i++) {
        ImNodes::BeginOutputAttribute(outputAttrIds[i]);
        ImGui::Text("-> Objective %zu", i + 1);
        ImNodes::EndOutputAttribute();
    }
#endif
}

std::vector<int> QuestHeaderNode::getInputAttrIds() const {
    return {};  // Quest header has no inputs
}

std::vector<int> QuestHeaderNode::getOutputAttrIds() const {
    return outputAttrIds;
}

unsigned int QuestHeaderNode::getTitleBarColor() const {
    return 0xFF0B6B2D;  // Green for quest header
}

// ============================================================================
// ObjectiveNode
// ============================================================================

ObjectiveNode::ObjectiveNode(const ObjectiveData& data, int nodeId, int index) {
    id = nodeId;
    objectiveId = data.id;
    title = "Objective " + std::to_string(index + 1);
    description = data.description;
    type = data.type;
    targetId = data.targetId;
    requiredCount = data.requiredCount;
    optional = data.optional;
    objectiveIndex = index;
}

void ObjectiveNode::renderContent() {
#ifdef HAS_IMNODES
    const float nodeWidth = 220.0f;
    
    // Input
    ImNodes::BeginInputAttribute(inputAttrId);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();
    
    // Type badge
    ImVec4 typeColor = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
    if (type == "talk") typeColor = ImVec4(0.3f, 0.7f, 1.0f, 1.0f);
    else if (type == "collect") typeColor = ImVec4(1.0f, 0.8f, 0.2f, 1.0f);
    else if (type == "deliver") typeColor = ImVec4(0.2f, 1.0f, 0.5f, 1.0f);
    else if (type == "goto") typeColor = ImVec4(0.8f, 0.4f, 1.0f, 1.0f);
    else if (type == "examine") typeColor = ImVec4(1.0f, 0.5f, 0.3f, 1.0f);
    
    ImGui::TextColored(typeColor, "[%s]", type.c_str());
    
    // Description
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + nodeWidth);
    ImGui::TextWrapped("%s", description.c_str());
    ImGui::PopTextWrapPos();
    
    // Target
    if (!targetId.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "Target: %s", targetId.c_str());
    }
    
    // Count
    if (requiredCount > 1) {
        ImGui::Text("Count: %d", requiredCount);
    }
    
    // Optional badge
    if (optional) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "(Optional)");
    }
    
    // Output for chaining
    ImNodes::BeginOutputAttribute(outputAttrId);
    ImGui::Text("->");
    ImNodes::EndOutputAttribute();
#endif
}

std::vector<int> ObjectiveNode::getInputAttrIds() const {
    return { inputAttrId };
}

std::vector<int> ObjectiveNode::getOutputAttrIds() const {
    return { outputAttrId };
}

unsigned int ObjectiveNode::getTitleBarColor() const {
    if (optional) {
        return 0xFF505050;  // Gray for optional
    }
    return 0xFF2D4B6B;  // Blue for required
}
