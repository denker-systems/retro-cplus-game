/**
 * @file LayerEditorPanel.cpp
 * @brief Implementation of LayerEditorPanel
 */
#include "LayerEditorPanel.h"
#include "editor/EditorContext.h"
#include <imgui.h>
#include <cstring>
#include <cmath>

LayerEditorPanel::LayerEditorPanel(EditorContext& context)
    : m_context(context) {}

void LayerEditorPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (!ImGui::Begin(m_title.c_str(), &m_visible)) {
        ImGui::End();
        return;
    }
    
    ImGui::TextDisabled("Layer system being migrated to Actor-based architecture");
    ImGui::Text("This panel will be reimplemented soon.");
    // Two columns: layer list and properties
    ImGui::Columns(2);
    
    renderLayerList();
    
    ImGui::NextColumn();
    
    renderLayerProperties();
    
    ImGui::Columns(1);
    
    // Add layer dialog
    if (m_showAddDialog) {
        renderAddLayerDialog();
    }
    
    ImGui::End();
#endif
}

void LayerEditorPanel::renderLayerList() {
    // Disabled during migration
}

void LayerEditorPanel::renderLayerProperties() {
    // Disabled during migration
}

void LayerEditorPanel::renderAddLayerDialog() {
    // Disabled during migration
}

void LayerEditorPanel::renderParallaxPreview() {
    // Disabled during migration
}
