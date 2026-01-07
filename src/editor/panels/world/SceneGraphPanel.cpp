/**
 * @file SceneGraphPanel.cpp
 * @brief Implementation of SceneGraphPanel
 */
#include "SceneGraphPanel.h"
#include "editor/core/EditorContext.h"
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
        ImGui::Text("Scene: %s", m_scene->getName().c_str());
        ImGui::Separator();
        
        // Render actors
        const auto& actors = m_scene->getActors();
        for (const auto& actor : actors) {
            renderActorTree(actor.get());
        }
    } else {
        ImGui::TextDisabled("No scene loaded");
    }
    
    ImGui::End();
}

void SceneGraphPanel::renderActorTree(engine::ActorObject* actor) {
    if (!actor) return;
    
    // Skip inactive actors if filter is on
    if (!m_showInactive && !actor->isActive()) {
        return;
    }
    
    // Actor flags (leaf since actors don't have children in flat list)
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_Leaf;
    
    if (actor == m_selectedActor) {
        flags |= ImGuiTreeNodeFlags_Selected;
    }
    
    // Actor icon
    const char* icon = "A";  // Actor
    
    // Actor label
    std::string label = std::string(icon) + " " + actor->getName();
    if (!actor->isActive()) {
        label += " (inactive)";
    }
    if (!actor->isVisible()) {
        label += " (hidden)";
    }
    
    // Render tree node
    bool nodeOpen = ImGui::TreeNodeEx(actor, flags, "%s", label.c_str());
    
    // Click to select
    if (ImGui::IsItemClicked()) {
        m_selectedActor = actor;
    }
    
    // Context menu
    if (ImGui::BeginPopupContextItem()) {
        renderActorContextMenu(actor);
        ImGui::EndPopup();
    }
    
    if (nodeOpen) {
        ImGui::TreePop();
    }
}

void SceneGraphPanel::renderActorContextMenu(engine::ActorObject* actor) {
    if (!actor) return;
    
    ImGui::Text("Actor: %s", actor->getName().c_str());
    ImGui::Separator();
    
    if (ImGui::MenuItem("Rename")) {
        // TODO: Show rename dialog
    }
    
    if (ImGui::MenuItem("Duplicate")) {
        // TODO: Duplicate actor
    }
    
    ImGui::Separator();
    
    bool active = actor->isActive();
    if (ImGui::MenuItem("Active", nullptr, &active)) {
        actor->setActive(active);
    }
    
    bool visible = actor->isVisible();
    if (ImGui::MenuItem("Visible", nullptr, &visible)) {
        actor->setVisible(visible);
    }
    
    ImGui::Separator();
    
    if (ImGui::MenuItem("Delete", "Del")) {
        // TODO: Delete actor from scene
    }
}
