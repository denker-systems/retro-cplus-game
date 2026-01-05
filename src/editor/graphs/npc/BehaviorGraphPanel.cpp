/**
 * @file BehaviorGraphPanel.cpp
 * @brief Implementation of NPC Behavior Graph Editor Panel
 */
#include "BehaviorGraphPanel.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

BehaviorGraphPanel::BehaviorGraphPanel(EditorContext& context)
    : INodeGraphPanel(context, "behavior_graph", "NPC Behavior") {
}

void BehaviorGraphPanel::renderHeader() {
#ifdef HAS_IMGUI
    // Check if an NPC is selected
    if (m_context.selectedType != EditorContext::SelectionType::NPC ||
        m_context.selectedNPCId.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "Select an NPC in the Hierarchy panel");
        return;
    }
    
    // Reload if NPC changed
    if (m_currentDataId != m_context.selectedNPCId) {
        loadNPC(m_context.selectedNPCId);
    }
    
    // Header info
    ImGui::Text("NPC: %s", m_context.selectedNPCId.c_str());
    
    // Find NPC for name
    for (const auto& n : m_context.npcs) {
        if (n.id == m_context.selectedNPCId) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.8f, 0.5f, 1.0f, 1.0f), 
                "| %s", n.name.c_str());
            break;
        }
    }
    
    // Toolbar
    if (ImGui::Button("Re-layout")) {
        m_needsLayout = true;
    }
    
    ImGui::Separator();
#endif
}

void BehaviorGraphPanel::loadNPC(const std::string& npcId) {
    clearGraph();
    m_currentDataId = npcId;
    
    // Find NPC data
    NPCData* npc = nullptr;
    for (auto& n : m_context.npcs) {
        if (n.id == npcId) {
            npc = &n;
            break;
        }
    }
    
    if (!npc) return;
    
    // Create NPC header node
    auto headerNode = std::make_unique<NPCHeaderNode>(*npc, generateId());
    headerNode->dialogOutputAttr = generateId();
    headerNode->roomOutputAttr = generateId();
    m_nodes.push_back(std::move(headerNode));
    
    // Create dialog reference node if has dialog
    if (!npc->dialogId.empty()) {
        auto dialogNode = std::make_unique<DialogRefNode>(npc->dialogId, generateId());
        dialogNode->inputAttrId = generateId();
        m_nodes.push_back(std::move(dialogNode));
    }
    
    // Create room reference node if has room
    if (!npc->room.empty()) {
        auto roomNode = std::make_unique<RoomRefNode>(npc->room, generateId());
        roomNode->inputAttrId = generateId();
        m_nodes.push_back(std::move(roomNode));
    }
    
    // Add behavior state if can move
    if (npc->canMove) {
        auto idleNode = std::make_unique<BehaviorStateNode>(
            BehaviorStateNode::StateType::Idle, generateId());
        idleNode->inputAttrId = generateId();
        idleNode->outputAttrId = generateId();
        m_nodes.push_back(std::move(idleNode));
    }
    
    // Create links
    createLinks();
    
    m_needsLayout = true;
}

void BehaviorGraphPanel::createLinks() {
    if (m_nodes.empty()) return;
    
    NPCHeaderNode* header = static_cast<NPCHeaderNode*>(m_nodes[0].get());
    
    // Find dialog and room nodes
    DialogRefNode* dialogNode = nullptr;
    RoomRefNode* roomNode = nullptr;
    
    for (size_t i = 1; i < m_nodes.size(); i++) {
        if (auto* dn = dynamic_cast<DialogRefNode*>(m_nodes[i].get())) {
            dialogNode = dn;
        }
        if (auto* rn = dynamic_cast<RoomRefNode*>(m_nodes[i].get())) {
            roomNode = rn;
        }
    }
    
    // Link header to dialog
    if (dialogNode) {
        m_links.push_back(Link(
            generateId(),
            header->dialogOutputAttr,
            dialogNode->inputAttrId
        ));
    }
    
    // Link header to room
    if (roomNode) {
        m_links.push_back(Link(
            generateId(),
            header->roomOutputAttr,
            roomNode->inputAttrId
        ));
    }
}

void BehaviorGraphPanel::onNodeSelected(int nodeId) {
    // Could navigate to dialog/room editor
}

void BehaviorGraphPanel::onLinkCreated(int startAttr, int endAttr) {
    // NPC structure is informational
}

void BehaviorGraphPanel::onLinkDeleted(int linkId) {
    // NPC structure is informational
}

void BehaviorGraphPanel::populateFromData() {
    if (!m_context.selectedNPCId.empty()) {
        loadNPC(m_context.selectedNPCId);
    }
}

void BehaviorGraphPanel::syncToData() {
    // NPC editing done via properties panel
}
