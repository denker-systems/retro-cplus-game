/**
 * @file ItemPropertyEditor.cpp
 * @brief Implementation av Item property editor
 */
#include "ItemPropertyEditor.h"
#include "PropertyEditorUtils.h"
#include "editor/EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ItemPropertyEditor::ItemPropertyEditor(EditorContext& context)
    : m_context(context) {
}

void ItemPropertyEditor::setItem(ItemData* item) {
    m_item = item;
    if (m_item) {
        m_originalData = *m_item;
        m_isDirty = false;
    }
}

void ItemPropertyEditor::render() {
#ifdef HAS_IMGUI
    if (!m_item) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No item selected");
        return;
    }
    
    renderBasicProperties();
    renderCombinations();
#endif
}

void ItemPropertyEditor::renderBasicProperties() {
#ifdef HAS_IMGUI
    PropertyEditorUtils::SectionHeader("Item Properties");
    
    // ID
    ImGui::Text("ID: %s", m_item->id.c_str());
    PropertyEditorUtils::HelpMarker("Item ID cannot be changed after creation");
    
    // Name
    if (PropertyEditorUtils::InputText("Name", m_item->name)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Description
    if (PropertyEditorUtils::InputTextMultiline("Description", m_item->description, ImVec2(0, 80))) {
        m_isDirty = true;
        m_context.markDirty();
    }
    
    // Icon
    if (PropertyEditorUtils::FileBrowser("Icon", m_item->icon, "*.png")) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Path to icon image");
    
    // Combinable
    if (ImGui::Checkbox("Combinable", &m_item->combinable)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Can this item be combined with other items?");
#endif
}

void ItemPropertyEditor::renderCombinations() {
#ifdef HAS_IMGUI
    if (!m_item->combinable) {
        return;
    }
    
    PropertyEditorUtils::SectionHeader("Item Combinations");
    
    // Item dropdowns för combinations
    std::vector<std::pair<std::string, std::string>> itemItems;
    for (const auto& item : m_context.items) {
        if (item.id != m_item->id) { // Exkludera sig själv
            itemItems.push_back({item.id, item.name});
        }
    }
    
    if (PropertyEditorUtils::IdCombo("Combines with", m_item->combinesWith, itemItems)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Item to combine with");
    
    if (PropertyEditorUtils::IdCombo("Result", m_item->combineResult, itemItems)) {
        m_isDirty = true;
        m_context.markDirty();
    }
    PropertyEditorUtils::HelpMarker("Resulting item after combination");
    
    ImGui::Spacing();
    ImGui::TextDisabled("Note: Current system supports one combination per item");
#endif
}

bool ItemPropertyEditor::validate(std::string& outError) const {
    if (!m_item) {
        outError = "No item selected";
        return false;
    }
    
    if (m_item->id.empty()) {
        outError = "Item ID cannot be empty";
        return false;
    }
    
    if (m_item->name.empty()) {
        outError = "Item name cannot be empty";
        return false;
    }
    
    return true;
}

bool ItemPropertyEditor::isDirty() const {
    return m_isDirty;
}

void ItemPropertyEditor::revert() {
    if (m_item) {
        *m_item = m_originalData;
        m_isDirty = false;
    }
}

void ItemPropertyEditor::apply() {
    if (m_item) {
        m_originalData = *m_item;
        m_isDirty = false;
    }
}
