/**
 * @file HotspotPropertyEditor.cpp
 * @brief Implementation av Hotspot property editor
 */
#include "HotspotPropertyEditor.h"
#include "editor/properties/core/PropertyEditorUtils.h"
#include "editor/core/EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

HotspotPropertyEditor::HotspotPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void HotspotPropertyEditor::setHotspot(HotspotData* hotspot, RoomData* parentRoom) {
    m_hotspot = hotspot;
    m_parentRoom = parentRoom;
    if (m_hotspot) {
        m_originalData = *m_hotspot;
        m_isDirty = false;
    }
}

void HotspotPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_hotspot) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No hotspot selected");
        return;
    }
    
    renderBasicProperties();
    renderPositionProperties();
    renderTypeSpecificProperties();
    renderPhysicsProperties();
    renderFunnyFails();
    
    // Delete button
    ImGui::Spacing();
    ImGui::Separator();
    if (ImGui::Button("Delete Hotspot", ImVec2(-1, 0))) {
        ImGui::OpenPopup("DeleteHotspotConfirm");
    }
    
    if (ImGui::BeginPopupModal("DeleteHotspotConfirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete this hotspot?");
        ImGui::Text("This action cannot be undone.");
        ImGui::Spacing();
        
        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            if (m_parentRoom) {
                auto it = std::find_if(m_parentRoom->hotspots.begin(), m_parentRoom->hotspots.end(),
                    [this](const HotspotData& hs) { return &hs == m_hotspot; });
                if (it != m_parentRoom->hotspots.end()) {
                    m_parentRoom->hotspots.erase(it);
                    m_context.markDirty();
                    m_context.selectedHotspotIndex = -1;
                    m_context.selectedType = EditorContext::SelectionType::Room;
                }
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

void HotspotPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Basic Properties");
    
    // ID
    char idBuf[256];
    strncpy_s(idBuf, m_hotspot->id.c_str(), sizeof(idBuf) - 1);
    if (ImGui::InputText("ID", idBuf, sizeof(idBuf))) {
        m_hotspot->id = idBuf;
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Unique identifier for this hotspot");
    
    // Name
    char nameBuf[256];
    strncpy_s(nameBuf, m_hotspot->name.c_str(), sizeof(nameBuf) - 1);
    if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
        m_hotspot->name = nameBuf;
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Type dropdown
    std::vector<std::pair<std::string, const char*>> typeItems = {
        {"exit", "Exit (Scene)"},
        {"gateway", "Gateway (Level/World)"},
        {"npc", "NPC"},
        {"item", "Item"},
        {"examine", "Examine"}
    };
    
    std::string currentType = m_hotspot->type;
    if (ImGui::BeginCombo("Type", currentType.c_str())) {
        for (const auto& item : typeItems) {
            bool isSelected = (currentType == item.first);
            if (ImGui::Selectable(item.second, isSelected)) {
                m_hotspot->type = item.first;
                m_isDirty = true;
                m_context.markDirty();
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
#endif
}

void HotspotPropertyEditor::renderPositionProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Position & Size");
    
    // Position
    int pos[2] = { m_hotspot->x, m_hotspot->y };
    if (ImGui::DragInt2("Position (X, Y)", pos, 1.0f, 0, 640)) {
        m_hotspot->x = pos[0];
        m_hotspot->y = pos[1];
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Size
    int size[2] = { m_hotspot->w, m_hotspot->h };
    if (ImGui::DragInt2("Size (W, H)", size, 1.0f, 1, 640)) {
        m_hotspot->w = size[0];
        m_hotspot->h = size[1];
        m_isDirty = true;
        m_context.markDirty();
    }
    
    PropertyEditorUtils::HelpMarker("You can also drag hotspots in the Viewport panel");
#endif
}

void HotspotPropertyEditor::renderTypeSpecificProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Type-Specific Properties");
    
    // Exit (scene transition within same level)
    if (m_hotspot->type == "exit") {
        // Room dropdown
        std::vector<std::pair<std::string, std::string>> roomItems;
        for (const auto& room : m_context.rooms) {
            roomItems.push_back({room.id, room.name});
        }
        if (PropertyEditorUtils::IdCombo("Target Scene", m_hotspot->targetScene, roomItems)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Scene to transition to (within same level)");
    }
    
    // Gateway (level/world transition)
    if (m_hotspot->type == "gateway") {
        ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Gateway - Cross-level/world transition");
        
        // Target Scene (optional)
        std::vector<std::pair<std::string, std::string>> sceneItems;
        sceneItems.push_back({"", "(None)"});
        for (const auto& room : m_context.rooms) {
            sceneItems.push_back({room.id, room.name});
        }
        if (PropertyEditorUtils::IdCombo("Target Scene", m_hotspot->targetScene, sceneItems)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        
        // Target Level
        char levelBuf[256];
        strncpy_s(levelBuf, m_hotspot->targetLevel.c_str(), sizeof(levelBuf) - 1);
        if (ImGui::InputText("Target Level", levelBuf, sizeof(levelBuf))) {
            m_hotspot->targetLevel = levelBuf;
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Level ID to transition to");
        
        // Target World
        char worldBuf[256];
        strncpy_s(worldBuf, m_hotspot->targetWorld.c_str(), sizeof(worldBuf) - 1);
        if (ImGui::InputText("Target World", worldBuf, sizeof(worldBuf))) {
            m_hotspot->targetWorld = worldBuf;
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("World ID to transition to (for cross-world travel)");
    }
    
    // NPC
    if (m_hotspot->type == "npc") {
        // Dialog dropdown
        std::vector<std::pair<std::string, std::string>> dialogItems;
        for (const auto& dialog : m_context.dialogs) {
            dialogItems.push_back({dialog.id, dialog.npcName + " (" + dialog.id + ")"});
        }
        if (PropertyEditorUtils::IdCombo("Dialog", m_hotspot->dialogId, dialogItems)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Dialog to trigger when interacting with this NPC");
    }
    
    // Item (HotspotData har inte itemId field, använd id istället)
    if (m_hotspot->type == "item") {
        ImGui::TextDisabled("Item ID is same as hotspot ID");
        PropertyEditorUtils::HelpMarker("Item to add to inventory when picked up");
    }
    
    // Examine text (alla typer)
    if (PropertyEditorUtils::InputTextMultiline("Examine Text", m_hotspot->examineText, ImVec2(0, 60))) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Text shown when player examines this hotspot");
#endif
}

void HotspotPropertyEditor::renderPhysicsProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Physics");
    
    // Enable physics
    if (ImGui::Checkbox("Enable Physics", &m_hotspot->physics.enabled)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Enable physics collision/trigger for this hotspot");
    
    if (!m_hotspot->physics.enabled) {
        ImGui::BeginDisabled();
    }
    
    // For hotspots, default to trigger mode
    if (ImGui::Checkbox("Is Trigger", &m_hotspot->physics.collider.isTrigger)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Trigger = detect overlap (exits, dialogs). Solid = block movement (walls)");
    
    // Collider size (use hotspot bounds by default)
    ImGui::Text("Collider Size:");
    if (ImGui::Button("Match Hotspot Bounds")) {
        m_hotspot->physics.collider.width = static_cast<float>(m_hotspot->w);
        m_hotspot->physics.collider.height = static_cast<float>(m_hotspot->h);
        m_isDirty = true;
        m_context.markDirty();
    }
    
    if (ImGui::DragFloat("Width", &m_hotspot->physics.collider.width, 1.0f, 1.0f, 500.0f)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    if (ImGui::DragFloat("Height", &m_hotspot->physics.collider.height, 1.0f, 1.0f, 500.0f)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Offset
    float offset[2] = { m_hotspot->physics.collider.offsetX, m_hotspot->physics.collider.offsetY };
    if (ImGui::DragFloat2("Offset", offset, 1.0f)) {
        m_hotspot->physics.collider.offsetX = offset[0];
        m_hotspot->physics.collider.offsetY = offset[1];
        m_isDirty = true;
        m_context.markDirty();
    }
    
    if (!m_hotspot->physics.enabled) {
        ImGui::EndDisabled();
    }
#endif
}

void HotspotPropertyEditor::renderFunnyFails() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Funny Fails");
    
    ImGui::Text("Humorous responses for wrong actions");
    
    if (ImGui::Button("Add Funny Fail")) {
        m_hotspot->funnyFails.push_back("New funny fail message");
        m_isDirty = true;
        m_context.markDirty();
    }
    
    ImGui::Spacing();
    
    // Lista funny fails
    int toDelete = -1;
    for (size_t i = 0; i < m_hotspot->funnyFails.size(); i++) {
        ImGui::PushID((int)i);
        
        if (PropertyEditorUtils::InputTextMultiline("##fail", m_hotspot->funnyFails[i], ImVec2(-40, 40))) {
            m_isDirty = true;
            m_context.markDirty();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("X", ImVec2(30, 40))) {
            toDelete = (int)i;
        }
        
        ImGui::PopID();
    }
    
    if (toDelete >= 0) {
        m_hotspot->funnyFails.erase(m_hotspot->funnyFails.begin() + toDelete);
        m_isDirty = true;
        m_context.markDirty();
    }
#endif
}

bool HotspotPropertyEditor::validate(std::string& outError) const {
    if (!m_hotspot) {
        outError = "No hotspot selected";
        return false;
    }
    
    if (m_hotspot->id.empty()) {
        outError = "Hotspot ID cannot be empty";
        return false;
    }
    
    if (m_hotspot->name.empty()) {
        outError = "Hotspot name cannot be empty";
        return false;
    }
    
    if (m_hotspot->type.empty()) {
        outError = "Hotspot type cannot be empty";
        return false;
    }
    
    // Type-specific validering
    if (m_hotspot->type == "exit" && m_hotspot->targetScene.empty()) {
        outError = "Exit hotspot must have a target room";
        return false;
    }
    
    if (m_hotspot->type == "npc" && m_hotspot->dialogId.empty()) {
        outError = "NPC hotspot should have a dialog ID";
        // Warning, inte error
    }
    
    return true;
}

bool HotspotPropertyEditor::isDirty() const {
    return m_isDirty;
}

void HotspotPropertyEditor::revert() {
    if (m_hotspot) {
        *m_hotspot = m_originalData;
        m_isDirty = false;
    }
}

void HotspotPropertyEditor::apply() {
    if (m_hotspot) {
        m_originalData = *m_hotspot;
        m_isDirty = false;
    }
}
