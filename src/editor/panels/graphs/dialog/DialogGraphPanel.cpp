/**
 * @file DialogGraphPanel.cpp
 * @brief Implementation of Dialog Graph Editor Panel
 */
#include "DialogGraphPanel.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

DialogGraphPanel::DialogGraphPanel(EditorContext& context)
    : INodeGraphPanel(context, "dialog_graph", "Dialog Graph") {
}

void DialogGraphPanel::renderHeader() {
#ifdef HAS_IMGUI
    // Check if a dialog is selected
    if (m_context.selectedType != EditorContext::SelectionType::Dialog ||
        m_context.selectedDialogId.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "Select a dialog in the Hierarchy panel");
        return;
    }
    
    // Reload if dialog changed
    if (m_currentDataId != m_context.selectedDialogId) {
        loadDialog(m_context.selectedDialogId);
    }
    
    // Header info
    ImGui::Text("Dialog: %s", m_context.selectedDialogId.c_str());
    
    // Find dialog for NPC name
    for (const auto& d : m_context.dialogs) {
        if (d.id == m_context.selectedDialogId) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), 
                "| NPC: %s", d.npcName.c_str());
            break;
        }
    }
    
    // Toolbar
    if (ImGui::Button("Re-layout")) {
        m_needsLayout = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Fit to Content")) {
#ifdef HAS_IMNODES
        // Note: imnodes doesn't have NavigateToContent, manual zoom needed
#endif
    }
    
    ImGui::Separator();
#endif
}

void DialogGraphPanel::loadDialog(const std::string& dialogId) {
    clearGraph();
    m_currentDataId = dialogId;
    m_attrToDialogNodeId.clear();
    m_attrToChoiceIndex.clear();
    
    // Find dialog data
    DialogData* dialog = nullptr;
    for (auto& d : m_context.dialogs) {
        if (d.id == dialogId) {
            dialog = &d;
            break;
        }
    }
    
    if (!dialog) return;
    
    // Create nodes
    for (const auto& nodeData : dialog->nodes) {
        auto node = std::make_unique<DialogNode>(nodeData, generateId());
        
        // Generate attribute IDs
        node->inputAttrId = generateId();
        node->isStartNode = (nodeData.id == dialog->startNodeId);
        
        // Output attributes for choices
        for (size_t i = 0; i < nodeData.choices.size(); i++) {
            int attrId = generateId();
            node->outputAttrIds.push_back(attrId);
            m_attrToDialogNodeId[attrId] = nodeData.id;
            m_attrToChoiceIndex[attrId] = i;
        }
        
        // Single output for nextNodeId
        if (nodeData.choices.empty() && nodeData.nextNodeId >= 0) {
            int attrId = generateId();
            node->outputAttrIds.push_back(attrId);
            m_attrToDialogNodeId[attrId] = nodeData.id;
        }
        
        // Map input attribute
        m_attrToDialogNodeId[node->inputAttrId] = nodeData.id;
        
        m_nodes.push_back(std::move(node));
    }
    
    // Create links
    createLinks();
    
    m_needsLayout = true;
}

void DialogGraphPanel::createLinks() {
    // Find dialog data
    DialogData* dialog = nullptr;
    for (auto& d : m_context.dialogs) {
        if (d.id == m_currentDataId) {
            dialog = &d;
            break;
        }
    }
    
    if (!dialog) return;
    
    // Build node ID -> DialogNode lookup
    std::unordered_map<int, DialogNode*> dialogNodeById;
    for (const auto& node : m_nodes) {
        DialogNode* dn = static_cast<DialogNode*>(node.get());
        dialogNodeById[dn->dialogNodeId] = dn;
    }
    
    // Create links from choices and nextNodeId
    for (const auto& node : m_nodes) {
        DialogNode* sourceNode = static_cast<DialogNode*>(node.get());
        
        // Links from choices
        for (size_t i = 0; i < sourceNode->choices.size(); i++) {
            int targetId = sourceNode->choices[i].nextNodeId;
            if (targetId >= 0 && dialogNodeById.count(targetId)) {
                DialogNode* targetNode = dialogNodeById[targetId];
                
                if (i < sourceNode->outputAttrIds.size()) {
                    m_links.push_back(Link(
                        generateId(),
                        sourceNode->outputAttrIds[i],
                        targetNode->inputAttrId
                    ));
                }
            }
        }
        
        // Link from nextNodeId (if no choices)
        if (sourceNode->choices.empty() && sourceNode->nextNodeId >= 0) {
            if (dialogNodeById.count(sourceNode->nextNodeId)) {
                DialogNode* targetNode = dialogNodeById[sourceNode->nextNodeId];
                
                if (!sourceNode->outputAttrIds.empty()) {
                    m_links.push_back(Link(
                        generateId(),
                        sourceNode->outputAttrIds[0],
                        targetNode->inputAttrId
                    ));
                }
            }
        }
    }
}

void DialogGraphPanel::onNodeSelected(int nodeId) {
    // Could update properties panel selection here
}

void DialogGraphPanel::onLinkCreated(int startAttr, int endAttr) {
    // TODO: Update DialogData when link is created interactively
}

void DialogGraphPanel::onLinkDeleted(int linkId) {
    // TODO: Update DialogData when link is deleted interactively
}

void DialogGraphPanel::populateFromData() {
    if (!m_context.selectedDialogId.empty()) {
        loadDialog(m_context.selectedDialogId);
    }
}

void DialogGraphPanel::syncToData() {
    // TODO: Sync graph changes back to DialogData
}
