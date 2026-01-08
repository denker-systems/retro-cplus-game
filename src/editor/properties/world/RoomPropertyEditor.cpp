/**
 * @file RoomPropertyEditor.cpp
 * @brief Implementation av Room property editor
 */
#include "RoomPropertyEditor.h"
#include "editor/properties/core/PropertyEditorUtils.h"
#include "editor/core/EditorContext.h"
#include <filesystem>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

RoomPropertyEditor::RoomPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void RoomPropertyEditor::setRoom(RoomData* room) {
    m_room = room;
    if (m_room) {
        m_originalData = *m_room;
        m_isDirty = false;
    }
}

void RoomPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_room) {
        ImGui::Text("No room selected");
        return;
    }
    
    renderBasicProperties();
    renderPlayerSpawn();
    renderWalkArea();
    renderHotspotsList();
    renderCollisionBoxes();
    renderLayersList();
    
    // Delete button at bottom
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("Delete Room", ImVec2(-1, 0))) {
        ImGui::OpenPopup("DeleteRoomConfirm");
    }
    ImGui::PopStyleColor();
    
    // Confirmation dialog
    if (ImGui::BeginPopupModal("DeleteRoomConfirm", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete this room?");
        ImGui::Text("Room: %s", m_room->name.c_str());
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), "Warning: This action cannot be undone!");
        ImGui::Spacing();
        
        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            // Find and delete room
            auto it = std::find_if(m_context.rooms.begin(), m_context.rooms.end(),
                [this](const RoomData& r) { return r.id == m_room->id; });
            if (it != m_context.rooms.end()) {
                m_context.rooms.erase(it);
                m_context.markDirty();
                m_context.selectedType = EditorContext::SelectionType::None;
                m_context.selectedRoomId.clear();
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

void RoomPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Basic Properties");
    
    // ID (read-only)
    ImGui::Text("ID: %s", m_room->id.c_str());
    PropertyEditorUtils::HelpMarker("Room ID cannot be changed after creation");
    
    // Name
    if (PropertyEditorUtils::InputText("Name", m_room->name)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Background
    if (PropertyEditorUtils::FileBrowser("Background", m_room->background, "*.png")) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Validera background path
    if (!m_room->background.empty() && !std::filesystem::exists(m_room->background)) {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Warning: File not found!");
    }
#endif
}

void RoomPropertyEditor::renderPlayerSpawn() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Player Spawn");
    
    float spawn[2] = { m_room->playerSpawnX, m_room->playerSpawnY };
    if (ImGui::DragFloat2("Spawn Position", spawn, 1.0f, 0.0f, 640.0f, "%.0f")) {
        m_room->playerSpawnX = spawn[0];
        m_room->playerSpawnY = spawn[1];
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Player spawn position when entering this room");
#endif
}

void RoomPropertyEditor::renderWalkArea() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Walk Area");
    
    // Min bounds
    int minBounds[2] = { m_room->walkArea.minX, m_room->walkArea.minY };
    if (ImGui::DragInt2("Min (X, Y)", minBounds, 1.0f, 0, 640)) {
        m_room->walkArea.minX = minBounds[0];
        m_room->walkArea.minY = minBounds[1];
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Max bounds
    int maxBounds[2] = { m_room->walkArea.maxX, m_room->walkArea.maxY };
    if (ImGui::DragInt2("Max (X, Y)", maxBounds, 1.0f, 0, 640)) {
        m_room->walkArea.maxX = maxBounds[0];
        m_room->walkArea.maxY = maxBounds[1];
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Depth scale
    float scale[2] = { m_room->walkArea.scaleTop, m_room->walkArea.scaleBottom };
    if (ImGui::SliderFloat2("Depth Scale", scale, 0.1f, 2.0f, "%.2f")) {
        m_room->walkArea.scaleTop = scale[0];
        m_room->walkArea.scaleBottom = scale[1];
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Character scale at top/bottom of walk area (for perspective)");
#endif
}

void RoomPropertyEditor::renderHotspotsList() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Hotspots");
    
    ImGui::Text("Count: %d", (int)m_room->hotspots.size());
    
    if (ImGui::Button("Add Hotspot")) {
        HotspotData newHotspot;
        newHotspot.id = "hotspot_" + std::to_string(m_room->hotspots.size());
        newHotspot.name = "New Hotspot";
        newHotspot.type = "examine";
        newHotspot.x = 100;
        newHotspot.y = 100;
        newHotspot.w = 50;
        newHotspot.h = 50;
        m_room->hotspots.push_back(newHotspot);
        m_isDirty = true;
        m_context.markDirty();
    }
    
    ImGui::Spacing();
    
    // Lista hotspots
    for (size_t i = 0; i < m_room->hotspots.size(); i++) {
        const auto& hs = m_room->hotspots[i];
        ImGui::PushID((int)i);
        
        bool isSelected = (m_context.selectedHotspotIndex == (int)i);
        if (ImGui::Selectable(hs.name.c_str(), isSelected)) {
            m_context.selectedHotspotIndex = (int)i;
            m_context.selectedType = EditorContext::SelectionType::Hotspot;
        }
        
        ImGui::PopID();
    }
#endif
}

void RoomPropertyEditor::renderCollisionBoxes() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Collision Boxes");
    
    ImGui::Text("Count: %d", (int)m_room->collisionBoxes.size());
    
    if (ImGui::Button("Add Collision Box")) {
        CollisionBoxData newBox;
        newBox.id = "box_" + std::to_string(m_room->collisionBoxes.size());
        newBox.type = "ground";
        newBox.x = 0;
        newBox.y = 350;
        newBox.width = 640;
        newBox.height = 30;
        newBox.oneWay = false;
        m_room->collisionBoxes.push_back(newBox);
        m_isDirty = true;
        m_context.markDirty();
    }
    
    ImGui::Spacing();
    
    // List collision boxes with inline editing
    int toDelete = -1;
    for (size_t i = 0; i < m_room->collisionBoxes.size(); i++) {
        auto& box = m_room->collisionBoxes[i];
        ImGui::PushID((int)i);
        
        // Color indicator based on type
        ImVec4 color;
        if (box.type == "ground") color = ImVec4(0.4f, 1.0f, 0.4f, 1.0f);
        else if (box.type == "wall") color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
        else if (box.type == "platform") color = ImVec4(0.4f, 0.8f, 1.0f, 1.0f);
        else color = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
        
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        bool open = ImGui::TreeNode(box.id.c_str());
        ImGui::PopStyleColor();
        
        if (open) {
            // ID
            if (PropertyEditorUtils::InputText("ID", box.id)) {
                m_isDirty = true;
                m_context.markDirty();
            }
            
            // Type dropdown
            const char* types[] = { "ground", "wall", "platform", "hazard" };
            int currentType = 0;
            for (int t = 0; t < 4; t++) {
                if (box.type == types[t]) { currentType = t; break; }
            }
            if (ImGui::Combo("Type", &currentType, types, 4)) {
                box.type = types[currentType];
                m_isDirty = true;
                m_context.markDirty();
            }
            
            // Position
            int pos[2] = { static_cast<int>(box.x), static_cast<int>(box.y) };
            if (ImGui::DragInt2("Position", pos, 1.0f)) {
                box.x = static_cast<float>(pos[0]);
                box.y = static_cast<float>(pos[1]);
                m_isDirty = true;
                m_context.markDirty();
            }
            
            // Size
            int size[2] = { static_cast<int>(box.width), static_cast<int>(box.height) };
            if (ImGui::DragInt2("Size", size, 1.0f, 1, 1000)) {
                box.width = static_cast<float>(size[0]);
                box.height = static_cast<float>(size[1]);
                m_isDirty = true;
                m_context.markDirty();
            }
            
            // One-way checkbox
            if (ImGui::Checkbox("One-Way", &box.oneWay)) {
                m_isDirty = true;
                m_context.markDirty();
            }
            PropertyEditorUtils::HelpMarker("One-way platforms can be jumped through from below");
            
            // Delete button
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
            if (ImGui::Button("Delete")) {
                toDelete = (int)i;
            }
            ImGui::PopStyleColor();
            
            ImGui::TreePop();
        }
        
        ImGui::PopID();
    }
    
    // Handle deletion outside loop
    if (toDelete >= 0) {
        m_room->collisionBoxes.erase(m_room->collisionBoxes.begin() + toDelete);
        m_isDirty = true;
        m_context.markDirty();
    }
#endif
}

void RoomPropertyEditor::renderLayersList() {
#ifdef HAS_IMGUI
    if (m_room->layers.empty()) return;
    
    PropertyEditorUtils::SectionHeader("Layers");
    
    ImGui::Text("Count: %d", (int)m_room->layers.size());
    
    for (size_t i = 0; i < m_room->layers.size(); i++) {
        auto& layer = m_room->layers[i];
        ImGui::PushID((int)i);
        
        if (ImGui::TreeNode(layer.image.c_str())) {
            if (ImGui::DragInt("Z-Index", &layer.zIndex, 1.0f, -10, 10)) {
                m_isDirty = true;
                m_context.markDirty();
            }
            
            float parallax[2] = { layer.parallaxX, layer.parallaxY };
            if (ImGui::DragFloat2("Parallax", parallax, 0.01f, 0.0f, 2.0f)) {
                layer.parallaxX = parallax[0];
                layer.parallaxY = parallax[1];
                m_isDirty = true;
                m_context.markDirty();
            }
            
            if (ImGui::SliderFloat("Opacity", &layer.opacity, 0.0f, 1.0f)) {
                m_isDirty = true;
                m_context.markDirty();
            }
            
            ImGui::TreePop();
        }
        
        ImGui::PopID();
    }
#endif
}

bool RoomPropertyEditor::validate(std::string& outError) const {
    if (!m_room) {
        outError = "No room selected";
        return false;
    }
    
    if (m_room->id.empty()) {
        outError = "Room ID cannot be empty";
        return false;
    }
    
    if (m_room->name.empty()) {
        outError = "Room name cannot be empty";
        return false;
    }
    
    if (m_room->background.empty()) {
        outError = "Background path cannot be empty";
        return false;
    }
    
    if (!std::filesystem::exists(m_room->background)) {
        outError = "Background file not found: " + m_room->background;
        return false;
    }
    
    // Validera walk area bounds
    if (m_room->walkArea.minX >= m_room->walkArea.maxX) {
        outError = "Walk area: minX must be less than maxX";
        return false;
    }
    
    if (m_room->walkArea.minY >= m_room->walkArea.maxY) {
        outError = "Walk area: minY must be less than maxY";
        return false;
    }
    
    return true;
}

bool RoomPropertyEditor::isDirty() const {
    return m_isDirty;
}

void RoomPropertyEditor::revert() {
    if (m_room) {
        *m_room = m_originalData;
        m_isDirty = false;
    }
}

void RoomPropertyEditor::apply() {
    if (m_room) {
        m_originalData = *m_room;
        m_isDirty = false;
    }
}
