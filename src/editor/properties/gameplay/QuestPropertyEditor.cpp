/**
 * @file QuestPropertyEditor.cpp
 * @brief Implementation av Quest property editor
 */
#include "QuestPropertyEditor.h"
#include "editor/properties/core/PropertyEditorUtils.h"
#include "editor/core/EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

QuestPropertyEditor::QuestPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void QuestPropertyEditor::setQuest(QuestData* quest) {
    m_quest = quest;
    if (m_quest) {
        m_originalData = *m_quest;
        m_isDirty = false;
        m_selectedObjectiveIndex = -1;
    }
}

void QuestPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_quest) {
        ImGui::Text("No quest selected");
        return;
    }
    
    renderBasicProperties();
    renderObjectivesList();
    
    if (m_selectedObjectiveIndex >= 0) {
        renderSelectedObjective();
    }
    
    // Delete button at bottom
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("Delete Quest", ImVec2(-1, 0))) {
        ImGui::OpenPopup("DeleteQuestConfirm");
    }
    ImGui::PopStyleColor();
    
    // Confirmation dialog
    if (ImGui::BeginPopupModal("DeleteQuestConfirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete this quest?");
        ImGui::Text("Quest: %s", m_quest->title.c_str());
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Warning: This action cannot be undone!");
        ImGui::Spacing();
        
        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            auto it = std::find_if(m_context.quests.begin(), m_context.quests.end(),
                [this](const QuestData& q) { return q.id == m_quest->id; });
            if (it != m_context.quests.end()) {
                m_context.quests.erase(it);
                m_context.markDirty();
                m_context.selectedType = EditorContext::SelectionType::None;
                m_context.selectedQuestId.clear();
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

void QuestPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Quest Properties");
    
    // ID
    ImGui::Text("ID: %s", m_quest->id.c_str());
    PropertyEditorUtils::HelpMarker("Quest ID cannot be changed after creation");
    
    // Title
    if (PropertyEditorUtils::InputText("Title", m_quest->title)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Description
    if (PropertyEditorUtils::InputTextMultiline("Description", m_quest->description, ImVec2(0, 80))) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    ImGui::Text("Objectives: %d", (int)m_quest->objectives.size());
#endif
}

void QuestPropertyEditor::renderObjectivesList() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Objectives");
    
    if (ImGui::Button("Add Objective")) {
        ObjectiveData newObj;
        newObj.id = "obj_" + std::to_string(m_quest->objectives.size());
        newObj.description = "New objective";
        newObj.type = "talk";
        newObj.targetId = "";
        newObj.requiredCount = 1;
        newObj.optional = false;
        m_quest->objectives.push_back(newObj);
        m_selectedObjectiveIndex = (int)m_quest->objectives.size() - 1;
        m_isDirty = true;
        m_context.markDirty();
    }
    
    ImGui::Spacing();
    
    // Lista objectives
    for (size_t i = 0; i < m_quest->objectives.size(); i++) {
        const auto& obj = m_quest->objectives[i];
        ImGui::PushID((int)i);
        
        std::string label = obj.description;
        if (obj.optional) {
            label = "[Optional] " + label;
        }
        
        bool isSelected = (m_selectedObjectiveIndex == (int)i);
        if (ImGui::Selectable(label.c_str(), isSelected)) {
            m_selectedObjectiveIndex = (int)i;
        }
        
        ImGui::PopID();
    }
#endif
}

void QuestPropertyEditor::renderSelectedObjective() {
#ifdef HAS_IMGUI
    if (m_selectedObjectiveIndex < 0 || m_selectedObjectiveIndex >= (int)m_quest->objectives.size()) {
        return;
    }
    
    PropertyEditorUtils::SectionHeader("Selected Objective");
    
    auto& obj = m_quest->objectives[m_selectedObjectiveIndex];
    
    // Description
    if (PropertyEditorUtils::InputTextMultiline("Description", obj.description, ImVec2(0, 60))) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Type dropdown (använd string istället för enum)
    std::vector<std::pair<std::string, const char*>> typeItems = {
        {"talk", "Talk"},
        {"collect", "Collect"},
        {"deliver", "Deliver"},
        {"goto", "Go To"},
        {"examine", "Examine"}
    };
    
    std::string currentType = obj.type;
    if (ImGui::BeginCombo("Type", currentType.c_str())) {
        for (const auto& item : typeItems) {
            bool isSelected = (currentType == item.first);
            if (ImGui::Selectable(item.second, isSelected)) {
                obj.type = item.first;
                m_isDirty = true;
                m_context.markDirty();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    // Type-specific properties
    if (obj.type == "talk") {
        // NPC dropdown
        std::vector<std::pair<std::string, std::string>> npcItems;
        for (const auto& npc : m_context.npcs) {
            npcItems.push_back({npc.id, npc.name});
        }
        if (PropertyEditorUtils::IdCombo("Target NPC", obj.targetId, npcItems)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("NPC to talk to");
    }
    else if (obj.type == "collect") {
        // Item dropdown
        std::vector<std::pair<std::string, std::string>> itemItems;
        for (const auto& item : m_context.items) {
            itemItems.push_back({item.id, item.name});
        }
        if (PropertyEditorUtils::IdCombo("Item", obj.targetId, itemItems)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Item to collect");
        
        if (ImGui::DragInt("Required Count", &obj.requiredCount, 1.0f, 1, 99)) {
            m_isDirty = true;
            m_context.markDirty();
        }
    }
    else if (obj.type == "deliver") {
        // Item dropdown
        std::vector<std::pair<std::string, std::string>> itemItems;
        for (const auto& item : m_context.items) {
            itemItems.push_back({item.id, item.name});
        }
        if (PropertyEditorUtils::IdCombo("Item", obj.targetId, itemItems)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Item to deliver to NPC");
    }
    else if (obj.type == "goto") {
        // Room dropdown
        std::vector<std::pair<std::string, std::string>> roomItems;
        for (const auto& room : m_context.rooms) {
            roomItems.push_back({room.id, room.name});
        }
        if (PropertyEditorUtils::IdCombo("Target Room", obj.targetId, roomItems)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Room to visit");
    }
    else if (obj.type == "examine") {
        // Hotspot dropdown (från current room om möjligt)
        ImGui::TextDisabled("Hotspot ID: %s", obj.targetId.c_str());
        PropertyEditorUtils::HelpMarker("Hotspot to examine (manual entry for now)");
    }
    
    // Optional checkbox
    if (ImGui::Checkbox("Optional", &obj.optional)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Is this objective optional?");
    
    // ========================================================================
    // WAYPOINT SETTINGS
    // ========================================================================
    PropertyEditorUtils::SectionHeader("Waypoint / Navigation");
    
    // Show on compass
    if (ImGui::Checkbox("Show on Compass", &obj.showOnCompass)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Show direction indicator in HUD");
    
    // Target scene dropdown
    std::vector<std::pair<std::string, std::string>> sceneItems;
    sceneItems.push_back({"", "(Auto-detect)"});
    for (const auto& room : m_context.rooms) {
        sceneItems.push_back({room.id, room.name});
    }
    if (PropertyEditorUtils::IdCombo("Target Scene", obj.targetScene, sceneItems)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Scene where waypoint is located (auto-detect from target if empty)");
    
    // Target position
    float pos[3] = { obj.targetX, obj.targetY, obj.targetZ };
    if (ImGui::DragFloat3("Position (3D)", pos, 0.1f, -100.0f, 100.0f, "%.1f")) {
        obj.targetX = pos[0];
        obj.targetY = pos[1];
        obj.targetZ = pos[2];
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("3D world position (0,0,0 = auto-detect from target)");
    
    // Waypoint radius
    if (ImGui::DragFloat("Arrival Radius", &obj.waypointRadius, 0.1f, 0.5f, 20.0f, "%.1f m")) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Distance to consider player 'arrived' at waypoint");
    
    // Custom icon (optional)
    if (PropertyEditorUtils::InputText("Custom Icon", obj.waypointIcon)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Custom icon name (leave empty for default based on type)");
    
    // Delete button
    ImGui::Spacing();
    if (ImGui::Button("Delete Objective", ImVec2(-1, 0))) {
        m_quest->objectives.erase(m_quest->objectives.begin() + m_selectedObjectiveIndex);
        m_selectedObjectiveIndex = -1;
        m_isDirty = true;
        m_context.markDirty();
    }
#endif
}

bool QuestPropertyEditor::validate(std::string& outError) const {
    if (!m_quest) {
        outError = "No quest selected";
        return false;
    }
    
    if (m_quest->id.empty()) {
        outError = "Quest ID cannot be empty";
        return false;
    }
    
    if (m_quest->title.empty()) {
        outError = "Quest title cannot be empty";
        return false;
    }
    
    if (m_quest->objectives.empty()) {
        outError = "Quest must have at least one objective";
        return false;
    }
    
    return true;
}

bool QuestPropertyEditor::isDirty() const {
    return m_isDirty;
}

void QuestPropertyEditor::revert() {
    if (m_quest) {
        *m_quest = m_originalData;
        m_isDirty = false;
        m_selectedObjectiveIndex = -1;
    }
}

void QuestPropertyEditor::apply() {
    if (m_quest) {
        m_originalData = *m_quest;
        m_isDirty = false;
    }
}
