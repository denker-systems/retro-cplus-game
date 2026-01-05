/**
 * @file NPCPropertyEditor.cpp
 * @brief Implementation av NPC property editor
 */
#include "NPCPropertyEditor.h"
#include "PropertyEditorUtils.h"
#include "editor/EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

NPCPropertyEditor::NPCPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void NPCPropertyEditor::setNPC(NPCData* npc) {
    m_npc = npc;
    if (m_npc) {
        m_originalData = *m_npc;
        m_isDirty = false;
    }
}

void NPCPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_npc) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No NPC selected");
        return;
    }
    
    renderBasicProperties();
    renderAIProperties();
    renderWaypoints();
    renderSchedules();
#endif
}

void NPCPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("NPC Properties");
    
    // ID
    ImGui::Text("ID: %s", m_npc->id.c_str());
    PropertyEditorUtils::HelpMarker("NPC ID cannot be changed after creation");
    
    // Name
    if (PropertyEditorUtils::InputText("Name", m_npc->name)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Dialog dropdown
    std::vector<std::pair<std::string, std::string>> dialogItems;
    for (const auto& dialog : m_context.dialogs) {
        dialogItems.push_back({dialog.id, dialog.npcName + " (" + dialog.id + ")"});
    }
    if (PropertyEditorUtils::IdCombo("Dialog", m_npc->dialogId, dialogItems)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Dialog to trigger when talking to this NPC");
    
    // Sprite
    if (PropertyEditorUtils::FileBrowser("Sprite", m_npc->sprite, "*.png")) {
        m_isDirty = true;
        m_context.markDirty();
    }
#endif
}

void NPCPropertyEditor::renderAIProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Movement");
    
    // Can Move
    if (ImGui::Checkbox("Can Move", &m_npc->canMove)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Can this NPC move around?");
    
    // Move Speed
    if (m_npc->canMove) {
        if (ImGui::DragFloat("Move Speed", &m_npc->moveSpeed, 1.0f, 0.0f, 200.0f, "%.0f")) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Movement speed in pixels per second");
    }
    
    // Can Talk
    if (ImGui::Checkbox("Can Talk", &m_npc->canTalk)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Can player talk to this NPC?");
#endif
}

void NPCPropertyEditor::renderWaypoints() {
#ifdef HAS_IMGUI
    // TODO: Waypoints kommer i framtida AI-system
    // För nu: visa bara placeholder
    PropertyEditorUtils::SectionHeader("AI Waypoints");
    ImGui::TextDisabled("(Coming soon - requires AI system upgrade)");
#endif
}

void NPCPropertyEditor::renderSchedules() {
#ifdef HAS_IMGUI
    // TODO: Schedules kommer i framtida AI-system
    // För nu: visa bara placeholder
    PropertyEditorUtils::SectionHeader("Time Schedules");
    ImGui::TextDisabled("(Coming soon - requires AI system upgrade)");
#endif
}

bool NPCPropertyEditor::validate(std::string& outError) const {
    if (!m_npc) {
        outError = "No NPC selected";
        return false;
    }
    
    if (m_npc->id.empty()) {
        outError = "NPC ID cannot be empty";
        return false;
    }
    
    if (m_npc->name.empty()) {
        outError = "NPC name cannot be empty";
        return false;
    }
    
    return true;
}

bool NPCPropertyEditor::isDirty() const {
    return m_isDirty;
}

void NPCPropertyEditor::revert() {
    if (m_npc) {
        *m_npc = m_originalData;
        m_isDirty = false;
    }
}

void NPCPropertyEditor::apply() {
    if (m_npc) {
        m_originalData = *m_npc;
        m_isDirty = false;
    }
}
