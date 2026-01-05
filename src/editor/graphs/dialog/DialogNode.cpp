/**
 * @file DialogNode.cpp
 * @brief Implementation of DialogNode
 */
#include "DialogNode.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

#ifdef HAS_IMNODES
#include <imnodes/imnodes.h>
#endif

DialogNode::DialogNode(const DialogNodeData& data, int nodeId) {
    id = nodeId;
    dialogNodeId = data.id;
    speaker = data.speaker;
    text = data.text;
    choices = data.choices;
    nextNodeId = data.nextNodeId;
    title = "[" + std::to_string(data.id) + "] " + speaker;
}

void DialogNode::renderContent() {
#ifdef HAS_IMNODES
    const float nodeWidth = 250.0f;
    
    // Input attribute (not for start node)
    if (!isStartNode) {
        ImNodes::BeginInputAttribute(inputAttrId);
        ImGui::Text("In");
        ImNodes::EndInputAttribute();
    }
    
    // Text content
    std::string displayText = text;
    truncateText(displayText, 60);
    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + nodeWidth);
    ImGui::TextWrapped("%s", displayText.c_str());
    ImGui::PopTextWrapPos();
    
    ImGui::Spacing();
    
    // Output attributes
    if (!choices.empty()) {
        ImGui::Separator();
        ImGui::Text("Choices:");
        
        for (size_t i = 0; i < choices.size() && i < outputAttrIds.size(); i++) {
            ImNodes::BeginOutputAttribute(outputAttrIds[i]);
            std::string choiceText = choices[i].text;
            truncateText(choiceText, 25);
            ImGui::Text("-> %s", choiceText.c_str());
            ImNodes::EndOutputAttribute();
        }
    } else if (nextNodeId >= 0 && !outputAttrIds.empty()) {
        ImNodes::BeginOutputAttribute(outputAttrIds[0]);
        ImGui::Text("->");
        ImNodes::EndOutputAttribute();
    }
#endif
}

std::vector<int> DialogNode::getInputAttrIds() const {
    if (isStartNode) return {};
    return { inputAttrId };
}

std::vector<int> DialogNode::getOutputAttrIds() const {
    return outputAttrIds;
}

unsigned int DialogNode::getTitleBarColor() const {
    if (isStartNode) {
        return 0xFF2D6B0B;  // Green for start node
    }
    return 0xFF505050;  // Default gray
}

void DialogNode::truncateText(std::string& str, size_t maxLen) const {
    if (str.length() > maxLen) {
        str = str.substr(0, maxLen) + "...";
    }
}
