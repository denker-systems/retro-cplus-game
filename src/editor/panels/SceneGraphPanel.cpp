/**
 * @file SceneGraphPanel.cpp
 * @brief Implementation of SceneGraphPanel
 */
#include "SceneGraphPanel.h"
#include "editor/EditorContext.h"
#include <imgui.h>

SceneGraphPanel::SceneGraphPanel(EditorContext& context)
    : m_context(context) {}

void SceneGraphPanel::render() {
    if (!m_visible) return;
    
    if (!ImGui::Begin(m_title.c_str(), &m_visible)) {
        ImGui::End();
        return;
    }
    
    // Toolbar
    if (ImGui::Checkbox("Show Inactive", &m_showInactive)) {
        // Toggle showing inactive nodes
    }
    
    ImGui::Separator();
    
    // Scene tree
    if (m_scene) {
        renderNodeTree(m_scene);
    } else {
        ImGui::TextDisabled("No scene loaded");
    }
    
    ImGui::End();
}

void SceneGraphPanel::renderNodeTree(engine::Node* node) {
    if (!node) return;
    
    // Skip inactive nodes if filter is on
    if (!m_showInactive && !node->isActive()) {
        return;
    }
    
    // Node flags
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    
    if (node == m_selectedNode) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    if (node->getChildCount() == 0) {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }
    
    // Node icon based on type
    const char* icon = "N";  // Default node
    // TODO: Add type-specific icons
    
    // Node label
    std::string label = std::string(icon) + " " + node->getName();
    if (!node->isActive()) {
        label += " (inactive)";
    }
    if (!node->isVisible()) {
        label += " (hidden)";
    }
    
    // Render tree node
    bool nodeOpen = ImGui::TreeNodeEx(node, flags, "%s", label.c_str());
    
    // Click to select
    if (ImGui::IsItemClicked()) {
        m_selectedNode = node;
    }
    
    // Drag-and-drop source
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        ImGui::SetDragDropPayload("SCENE_NODE", &node, sizeof(engine::Node*));
        ImGui::Text("Move: %s", node->getName().c_str());
        ImGui::EndDragDropSource();
    }
    
    // Drag-and-drop target (re-parenting)
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SCENE_NODE")) {
            engine::Node* draggedNode = *(engine::Node**)payload->Data;
            if (draggedNode && draggedNode != node) {
                // Re-parent: remove from old parent, add to new
                if (draggedNode->getParent()) {
                    // Transfer ownership from old parent to new parent
                    auto nodeOwnership = draggedNode->getParent()->removeChild(draggedNode);
                    if (nodeOwnership) {
                        node->addChild(std::move(nodeOwnership));
                    }
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
    
    // Context menu
    if (ImGui::BeginPopupContextItem()) {
        renderNodeContextMenu(node);
        ImGui::EndPopup();
    }
    
    // Render children
    if (nodeOpen) {
        for (size_t i = 0; i < node->getChildCount(); i++) {
            engine::Node* child = node->getChildAt(i);
            if (child) {
                renderNodeTree(child);
            }
        }
        ImGui::TreePop();
    }
}

void SceneGraphPanel::renderNodeContextMenu(engine::Node* node) {
    if (!node) return;
    
    ImGui::Text("Node: %s", node->getName().c_str());
    ImGui::Separator();
    
    if (ImGui::MenuItem("Rename")) {
        // TODO: Show rename dialog
    }
    
    if (ImGui::MenuItem("Duplicate")) {
        // TODO: Duplicate node
    }
    
    ImGui::Separator();
    
    bool active = node->isActive();
    if (ImGui::MenuItem("Active", nullptr, &active)) {
        node->setActive(active);
    }
    
    bool visible = node->isVisible();
    if (ImGui::MenuItem("Visible", nullptr, &visible)) {
        node->setVisible(visible);
    }
    
    ImGui::Separator();
    
    if (ImGui::MenuItem("Delete", "Del")) {
        // TODO: Delete node
    }
}
