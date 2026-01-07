/**
 * @file BehaviorNode.cpp
 * @brief Implementation of NPC behavior graph nodes
 */
#include "BehaviorNode.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

#ifdef HAS_IMNODES
#include <imnodes/imnodes.h>
#endif

// ============================================================================
// NPCHeaderNode
// ============================================================================

NPCHeaderNode::NPCHeaderNode(const NPCData& data, int nodeId) {
    id = nodeId;
    npcId = data.id;
    title = data.name;
    description = data.description;
    sprite = data.sprite;
    room = data.room;
    posX = data.x;
    posY = data.y;
    canTalk = data.canTalk;
    canMove = data.canMove;
    moveSpeed = data.moveSpeed;
}

void NPCHeaderNode::renderContent() {
#ifdef HAS_IMNODES
    const float nodeWidth = 200.0f;
    
    // Description
    if (!description.empty()) {
        ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + nodeWidth);
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "%s", description.c_str());
        ImGui::PopTextWrapPos();
    }
    
    ImGui::Spacing();
    
    // Properties
    ImGui::Text("Sprite: %s", sprite.c_str());
    ImGui::Text("Position: (%d, %d)", posX, posY);
    
    // Capabilities
    ImGui::Separator();
    if (canTalk) {
        ImGui::TextColored(ImVec4(0.3f, 0.8f, 1.0f, 1.0f), "[Can Talk]");
    }
    if (canMove) {
        ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.5f, 1.0f), "[Can Move] Speed: %.0f", moveSpeed);
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    // Output connections
    ImNodes::BeginOutputAttribute(dialogOutputAttr);
    ImGui::Text("-> Dialog");
    ImNodes::EndOutputAttribute();
    
    ImNodes::BeginOutputAttribute(roomOutputAttr);
    ImGui::Text("-> Room");
    ImNodes::EndOutputAttribute();
#endif
}

std::vector<int> NPCHeaderNode::getInputAttrIds() const {
    return {};
}

std::vector<int> NPCHeaderNode::getOutputAttrIds() const {
    return { dialogOutputAttr, roomOutputAttr };
}

unsigned int NPCHeaderNode::getTitleBarColor() const {
    return 0xFF6B2D4B;  // Purple for NPC
}

// ============================================================================
// DialogRefNode
// ============================================================================

DialogRefNode::DialogRefNode(const std::string& dialogId_, int nodeId) {
    id = nodeId;
    dialogId = dialogId_;
    title = "Dialog: " + dialogId_;
}

void DialogRefNode::renderContent() {
#ifdef HAS_IMNODES
    ImNodes::BeginInputAttribute(inputAttrId);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();
    
    ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), 
        "Click to open dialog editor");
#endif
}

std::vector<int> DialogRefNode::getInputAttrIds() const {
    return { inputAttrId };
}

std::vector<int> DialogRefNode::getOutputAttrIds() const {
    return {};
}

unsigned int DialogRefNode::getTitleBarColor() const {
    return 0xFF4B6B2D;  // Green-ish for dialog
}

// ============================================================================
// RoomRefNode
// ============================================================================

RoomRefNode::RoomRefNode(const std::string& roomId_, int nodeId) {
    id = nodeId;
    roomId = roomId_;
    title = "Room: " + roomId_;
}

void RoomRefNode::renderContent() {
#ifdef HAS_IMNODES
    ImNodes::BeginInputAttribute(inputAttrId);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();
    
    ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), 
        "Location in world");
#endif
}

std::vector<int> RoomRefNode::getInputAttrIds() const {
    return { inputAttrId };
}

std::vector<int> RoomRefNode::getOutputAttrIds() const {
    return {};
}

unsigned int RoomRefNode::getTitleBarColor() const {
    return 0xFF2D4B6B;  // Blue for room
}

// ============================================================================
// BehaviorStateNode
// ============================================================================

BehaviorStateNode::BehaviorStateNode(StateType type, int nodeId) {
    id = nodeId;
    stateType = type;
    
    switch (type) {
        case StateType::Idle: title = "State: Idle"; break;
        case StateType::Patrol: title = "State: Patrol"; break;
        case StateType::Follow: title = "State: Follow"; break;
        case StateType::Flee: title = "State: Flee"; break;
        case StateType::Custom: title = "State: Custom"; break;
    }
}

void BehaviorStateNode::renderContent() {
#ifdef HAS_IMNODES
    ImNodes::BeginInputAttribute(inputAttrId);
    ImGui::Text("In");
    ImNodes::EndInputAttribute();
    
    switch (stateType) {
        case StateType::Idle:
            ImGui::Text("NPC stands still");
            break;
        case StateType::Patrol:
            ImGui::Text("Walks between waypoints");
            break;
        case StateType::Follow:
            ImGui::Text("Follows player");
            break;
        case StateType::Flee:
            ImGui::Text("Runs from player");
            break;
        case StateType::Custom:
            ImGui::Text("Custom behavior");
            break;
    }
    
    ImNodes::BeginOutputAttribute(outputAttrId);
    ImGui::Text("->");
    ImNodes::EndOutputAttribute();
#endif
}

std::vector<int> BehaviorStateNode::getInputAttrIds() const {
    return { inputAttrId };
}

std::vector<int> BehaviorStateNode::getOutputAttrIds() const {
    return { outputAttrId };
}

unsigned int BehaviorStateNode::getTitleBarColor() const {
    switch (stateType) {
        case StateType::Idle: return 0xFF505050;
        case StateType::Patrol: return 0xFF4B2D6B;
        case StateType::Follow: return 0xFF2D6B4B;
        case StateType::Flee: return 0xFF6B2D2D;
        default: return 0xFF505050;
    }
}
