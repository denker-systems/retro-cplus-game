/**
 * @file WorldViewPanel.cpp
 * @brief Implementation of WorldViewPanel
 */
#include "WorldViewPanel.h"
#include "editor/EditorContext.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

WorldViewPanel::WorldViewPanel(EditorContext& context)
    : m_context(context) {
}

void WorldViewPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        if (!m_world) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                "No World loaded");
            ImGui::End();
            return;
        }
        
        renderLevelList();
        
        ImGui::Separator();
        
        renderCreateLevelButton();
    }
    ImGui::End();
#endif
}

void WorldViewPanel::renderLevelList() {
#ifdef HAS_IMGUI
    ImGui::Text("Levels in World:");
    ImGui::Separator();
    
    // TODO: När World har getLevels() metod
    // const auto& levels = m_world->getLevels();
    // 
    // for (const auto& level : levels) {
    //     bool isSelected = (level->getId() == m_selectedLevelId);
    //     
    //     if (ImGui::Selectable(level->getName().c_str(), isSelected)) {
    //         m_selectedLevelId = level->getId();
    //         m_context.selectedLevelId = level->getId();
    //     }
    //     
    //     if (isSelected) {
    //         renderLevelProperties(level.get());
    //     }
    // }
    
    // Placeholder
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), 
        "World.getLevels() not yet implemented");
#endif
}

void WorldViewPanel::renderLevelProperties(engine::Level* level) {
#ifdef HAS_IMGUI
    if (!level) return;
    
    ImGui::Indent();
    
    ImGui::Text("ID: %s", level->getId().c_str());
    ImGui::Text("Name: %s", level->getName().c_str());
    ImGui::Text("Description: %s", level->getDescription().c_str());
    
    // Music
    ImGui::Text("Music: %s", level->getMusic().c_str());
    
    // Scene count
    ImGui::Text("Scenes: %zu", level->getScenes().size());
    
    // Actions
    if (ImGui::Button("Open Level")) {
        // TODO: Open LevelViewPanel with this level
        m_context.selectedLevelId = level->getId();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Delete Level")) {
        // TODO: Confirm dialog
    }
    
    ImGui::Unindent();
#endif
}

void WorldViewPanel::renderCreateLevelButton() {
#ifdef HAS_IMGUI
    if (ImGui::Button("+ Create New Level")) {
        m_showCreateDialog = true;
        m_newLevelId[0] = '\0';
        m_newLevelName[0] = '\0';
    }
    
    if (m_showCreateDialog) {
        ImGui::OpenPopup("Create Level");
    }
    
    if (ImGui::BeginPopupModal("Create Level", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new Level");
        ImGui::Separator();
        
        ImGui::InputText("ID", m_newLevelId, sizeof(m_newLevelId));
        ImGui::InputText("Name", m_newLevelName, sizeof(m_newLevelName));
        
        ImGui::Separator();
        
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            // TODO: Create level in World
            // auto level = std::make_unique<Level>(m_newLevelId);
            // level->setName(m_newLevelName);
            // m_world->addLevel(std::move(level));
            
            m_showCreateDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showCreateDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
#endif
}
