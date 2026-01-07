/**
 * @file DialogPropertyEditor.cpp
 * @brief Implementation av Dialog property editor
 */
#include "DialogPropertyEditor.h"
#include "editor/properties/core/PropertyEditorUtils.h"
#include "editor/core/EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

DialogPropertyEditor::DialogPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void DialogPropertyEditor::setDialog(DialogData* dialog) {
    m_dialog = dialog;
    if (m_dialog) {
        m_originalData = *m_dialog;
        m_isDirty = false;
        m_selectedNodeIndex = -1;
    }
}

void DialogPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_dialog) {
        ImGui::Text("No dialog selected");
        return;
    }
    
    renderBasicProperties();
    renderNodesList();
    
    if (m_selectedNodeIndex >= 0) {
        renderSelectedNode();
        renderNodeChoices();
    }
    
    // Delete button at bottom
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("Delete Dialog", ImVec2(-1, 0))) {
        ImGui::OpenPopup("DeleteDialogConfirm");
    }
    ImGui::PopStyleColor();
    
    // Confirmation dialog
    if (ImGui::BeginPopupModal("DeleteDialogConfirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete this dialog?");
        ImGui::Text("Dialog: %s", m_dialog->id.c_str());
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Warning: This may break references in NPCs/Hotspots!");
        ImGui::Spacing();
        
        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            auto it = std::find_if(m_context.dialogs.begin(), m_context.dialogs.end(),
                [this](const DialogData& d) { return d.id == m_dialog->id; });
            if (it != m_context.dialogs.end()) {
                m_context.dialogs.erase(it);
                m_context.markDirty();
                m_context.selectedType = EditorContext::SelectionType::None;
                m_context.selectedDialogId.clear();
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
#endif
}

void DialogPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Dialog Properties");
    
    // ID
    ImGui::Text("ID: %s", m_dialog->id.c_str());
    PropertyEditorUtils::HelpMarker("Dialog ID cannot be changed after creation");
    
    // NPC dropdown
    std::vector<std::pair<std::string, std::string>> npcItems;
    for (const auto& npc : m_context.npcs) {
        npcItems.push_back({npc.name, npc.name}); // Använd name som både ID och display
    }
    // Lägg till custom option för att skriva eget namn
    if (ImGui::BeginCombo("NPC Name", m_dialog->npcName.c_str())) {
        // Lista NPCs
        for (const auto& item : npcItems) {
            bool isSelected = (m_dialog->npcName == item.first);
            if (ImGui::Selectable(item.second.c_str(), isSelected)) {
                m_dialog->npcName = item.first;
                m_isDirty = true;
                m_context.markDirty();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        
        // Separator
        ImGui::Separator();
        ImGui::TextDisabled("Or enter custom name:");
        
        ImGui::EndCombo();
    }
    
    // Manual input fallback
    ImGui::SameLine();
    if (ImGui::SmallButton("Edit")) {
        ImGui::OpenPopup("EditNPCName");
    }
    
    if (ImGui::BeginPopup("EditNPCName")) {
        char buffer[256];
        strncpy_s(buffer, m_dialog->npcName.c_str(), sizeof(buffer) - 1);
        if (ImGui::InputText("Custom Name", buffer, sizeof(buffer))) {
            m_dialog->npcName = buffer;
            m_isDirty = true;
            m_context.markDirty();
        }
        ImGui::EndPopup();
    }
    
    // Start Node ID
    if (ImGui::InputInt("Start Node ID", &m_dialog->startNodeId)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("ID of the first dialog node to show");
    
    ImGui::Text("Total Nodes: %d", (int)m_dialog->nodes.size());
#endif
}

void DialogPropertyEditor::renderNodesList() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Dialog Nodes");
    
    if (ImGui::Button("Add Node")) {
        DialogNodeData newNode;
        newNode.id = (int)m_dialog->nodes.size();
        newNode.speaker = m_dialog->npcName;
        newNode.text = "New dialog text";
        newNode.nextNodeId = -1;
        m_dialog->nodes.push_back(newNode);
        m_selectedNodeIndex = (int)m_dialog->nodes.size() - 1;
        m_isDirty = true;
        m_context.markDirty();
    }
    
    ImGui::Spacing();
    
    // Lista noder
    for (size_t i = 0; i < m_dialog->nodes.size(); i++) {
        const auto& node = m_dialog->nodes[i];
        ImGui::PushID((int)i);
        
        // Förhandsvisning av text (första 30 tecken)
        std::string preview = node.text.length() > 30 ? 
            node.text.substr(0, 30) + "..." : node.text;
        
        std::string label = "[" + std::to_string(node.id) + "] " + 
                           node.speaker + ": " + preview;
        
        bool isSelected = (m_selectedNodeIndex == (int)i);
        if (ImGui::Selectable(label.c_str(), isSelected)) {
            m_selectedNodeIndex = (int)i;
        }
        
        ImGui::PopID();
    }
#endif
}

void DialogPropertyEditor::renderSelectedNode() {
#ifdef HAS_IMGUI
    if (m_selectedNodeIndex < 0 || m_selectedNodeIndex >= (int)m_dialog->nodes.size()) {
        return;
    }
    
    PropertyEditorUtils::SectionHeader("Selected Node");
    
    auto& node = m_dialog->nodes[m_selectedNodeIndex];
    
    // Node ID
    if (ImGui::InputInt("Node ID", &node.id)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Unique ID for this node");
    
    // Speaker
    if (PropertyEditorUtils::InputText("Speaker", node.speaker)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Text
    if (PropertyEditorUtils::InputTextMultiline("Text", node.text, ImVec2(0, 80))) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Next Node ID (om inga choices)
    if (node.choices.empty()) {
        if (ImGui::InputInt("Next Node ID", &node.nextNodeId)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Next node to show (-1 = end dialog)");
    } else {
        ImGui::TextDisabled("Next Node ID: (controlled by choices)");
    }
    
    // Choices
    renderNodeChoices();
    
    // Delete button
    ImGui::Spacing();
    if (ImGui::Button("Delete Node", ImVec2(-1, 0))) {
        m_dialog->nodes.erase(m_dialog->nodes.begin() + m_selectedNodeIndex);
        m_selectedNodeIndex = -1;
        m_isDirty = true;
        m_context.markDirty();
    }
#endif
}

void DialogPropertyEditor::renderNodeChoices() {
#ifdef HAS_IMGUI
    if (m_selectedNodeIndex < 0 || m_selectedNodeIndex >= (int)m_dialog->nodes.size()) {
        return;
    }
    
    auto& node = m_dialog->nodes[m_selectedNodeIndex];
    
    ImGui::Spacing();
    ImGui::Text("Choices:");
    
    if (ImGui::Button("Add Choice")) {
        DialogChoiceData newChoice;
        newChoice.text = "New choice";
        newChoice.nextNodeId = -1;
        node.choices.push_back(newChoice);
        m_isDirty = true;
        m_context.markDirty();
    }
    
    ImGui::Spacing();
    
    // Lista choices
    for (size_t i = 0; i < node.choices.size(); i++) {
        ImGui::PushID((int)i);
        
        auto& choice = node.choices[i];
        
        // Choice text
        if (PropertyEditorUtils::InputText("##choiceText", choice.text)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        
        // Next node
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputInt("##nextNode", &choice.nextNodeId)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        
        // Delete button
        ImGui::SameLine();
        if (ImGui::Button("X", ImVec2(30, 0))) {
            node.choices.erase(node.choices.begin() + i);
            m_isDirty = true;
            m_context.markDirty();
            ImGui::PopID();
            break;
        }
        
        ImGui::PopID();
    }
#endif
}

bool DialogPropertyEditor::validate(std::string& outError) const {
    if (!m_dialog) {
        outError = "No dialog selected";
        return false;
    }
    
    if (m_dialog->id.empty()) {
        outError = "Dialog ID cannot be empty";
        return false;
    }
    
    if (m_dialog->nodes.empty()) {
        outError = "Dialog must have at least one node";
        return false;
    }
    
    // Validera att start node finns
    bool startNodeExists = false;
    for (const auto& node : m_dialog->nodes) {
        if (node.id == m_dialog->startNodeId) {
            startNodeExists = true;
            break;
        }
    }
    
    if (!startNodeExists) {
        outError = "Start node ID " + std::to_string(m_dialog->startNodeId) + " does not exist";
        return false;
    }
    
    return true;
}

bool DialogPropertyEditor::isDirty() const {
    return m_isDirty;
}

void DialogPropertyEditor::revert() {
    if (m_dialog) {
        *m_dialog = m_originalData;
        m_isDirty = false;
        m_selectedNodeIndex = -1;
    }
}

void DialogPropertyEditor::apply() {
    if (m_dialog) {
        m_originalData = *m_dialog;
        m_isDirty = false;
    }
}
