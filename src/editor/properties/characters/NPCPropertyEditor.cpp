/**
 * @file NPCPropertyEditor.cpp
 * @brief Implementation av NPC property editor
 */
#include "NPCPropertyEditor.h"
#include "editor/properties/core/PropertyEditorUtils.h"
#include "editor/core/EditorContext.h"

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
    renderPhysicsProperties();
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
    
    // Interaction Volume (3D)
    PropertyEditorUtils::SectionHeader("Interaction Volume (3D)");
    
    float volume[3] = { m_npc->interactVolumeX, m_npc->interactVolumeY, m_npc->interactVolumeZ };
    if (ImGui::DragFloat3("Volume (X, Y, Z)", volume, 0.1f, 0.1f, 20.0f, "%.1f")) {
        m_npc->interactVolumeX = volume[0];
        m_npc->interactVolumeY = volume[1];
        m_npc->interactVolumeZ = volume[2];
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("3D bounding box size for interaction detection in runtime");
#endif
}

void NPCPropertyEditor::renderPhysicsProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Physics");
    
    // Enable physics
    if (ImGui::Checkbox("Enable Physics", &m_npc->physics.enabled)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Enable Box2D physics for this NPC");
    
    if (!m_npc->physics.enabled) {
        ImGui::BeginDisabled();
    }
    
    // Body type
    const char* bodyTypes[] = { "static", "dynamic", "kinematic" };
    int currentType = 0;
    if (m_npc->physics.bodyType == "dynamic") currentType = 1;
    else if (m_npc->physics.bodyType == "kinematic") currentType = 2;
    
    if (ImGui::Combo("Body Type", &currentType, bodyTypes, 3)) {
        m_npc->physics.bodyType = bodyTypes[currentType];
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Static = immovable, Dynamic = affected by gravity, Kinematic = scripted movement");
    
    // Fixed rotation
    if (ImGui::Checkbox("Fixed Rotation", &m_npc->physics.fixedRotation)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Gravity scale
    if (ImGui::DragFloat("Gravity Scale", &m_npc->physics.gravityScale, 0.1f, 0.0f, 5.0f)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Collider section
    if (ImGui::TreeNode("Collider")) {
        // Shape type
        const char* shapes[] = { "box", "circle", "capsule" };
        int currentShape = 0;
        if (m_npc->physics.collider.shape == "circle") currentShape = 1;
        else if (m_npc->physics.collider.shape == "capsule") currentShape = 2;
        
        if (ImGui::Combo("Shape", &currentShape, shapes, 3)) {
            m_npc->physics.collider.shape = shapes[currentShape];
            m_isDirty = true;
            m_context.markDirty();
        }
        
        // Size
        if (ImGui::DragFloat("Width", &m_npc->physics.collider.width, 1.0f, 1.0f, 500.0f)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        if (currentShape != 1) { // Not circle
            if (ImGui::DragFloat("Height", &m_npc->physics.collider.height, 1.0f, 1.0f, 500.0f)) {
                m_isDirty = true;
                m_context.markDirty();
            }
        }
        
        // Offset
        float offset[2] = { m_npc->physics.collider.offsetX, m_npc->physics.collider.offsetY };
        if (ImGui::DragFloat2("Offset", offset, 1.0f)) {
            m_npc->physics.collider.offsetX = offset[0];
            m_npc->physics.collider.offsetY = offset[1];
            m_isDirty = true;
            m_context.markDirty();
        }
        
        // Is trigger
        if (ImGui::Checkbox("Is Trigger", &m_npc->physics.collider.isTrigger)) {
            m_isDirty = true;
            m_context.markDirty();
        }
        PropertyEditorUtils::HelpMarker("Triggers detect overlap but don't block movement");
        
        // Physics material
        if (ImGui::TreeNode("Material")) {
            if (ImGui::DragFloat("Density", &m_npc->physics.collider.density, 0.1f, 0.0f, 10.0f)) {
                m_isDirty = true;
                m_context.markDirty();
            }
            if (ImGui::DragFloat("Friction", &m_npc->physics.collider.friction, 0.05f, 0.0f, 1.0f)) {
                m_isDirty = true;
                m_context.markDirty();
            }
            if (ImGui::DragFloat("Bounciness", &m_npc->physics.collider.restitution, 0.05f, 0.0f, 1.0f)) {
                m_isDirty = true;
                m_context.markDirty();
            }
            ImGui::TreePop();
        }
        
        ImGui::TreePop();
    }
    
    if (!m_npc->physics.enabled) {
        ImGui::EndDisabled();
    }
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
