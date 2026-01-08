/**
 * @file CollisionBoxPropertyEditor.cpp
 * @brief Property editor for collision boxes implementation
 */
#include "CollisionBoxPropertyEditor.h"
#include "editor/properties/core/IPropertyEditor.h"
#include "editor/properties/core/PropertyEditorUtils.h"
#include "engine/core/ActorObjectExtended.h"

CollisionBoxPropertyEditor::CollisionBoxPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void CollisionBoxPropertyEditor::setCollisionBox(engine::ActorObjectExtended* actor, int boxIndex) {
    m_actor = actor;
    m_boxIndex = boxIndex;
}

void CollisionBoxPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_actor || m_boxIndex < 0) {
        ImGui::Text("No collision box selected");
        return;
    }
    
    renderBasicProperties();
    renderShapeProperties();
#endif
}

bool CollisionBoxPropertyEditor::validate(std::string& outError) const {
    // TODO: Implement validation
    return true;
}

void CollisionBoxPropertyEditor::revert() {
    // TODO: Implement revert
    m_isDirty = false;
}

void CollisionBoxPropertyEditor::apply() {
    // TODO: Implement apply
    m_isDirty = false;
}

void CollisionBoxPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Collision Box Properties");
    
    // Box index (read-only)
    ImGui::Text("Box Index: %d", m_boxIndex);
    
    // Active checkbox
    bool active = m_actor->isActive();
    if (ImGui::Checkbox("Active", &active)) {
        m_actor->setActive(active);
        m_isDirty = true;
        m_context.markDirty();
    }
#endif
}

void CollisionBoxPropertyEditor::renderShapeProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Shape Properties");
    
    // TODO: Implement actual collision box properties
    // This would need access to the collision component
    
    ImGui::Text("Collision box shape properties");
    ImGui::Text("TODO: Implement collision component access");
#endif
}
