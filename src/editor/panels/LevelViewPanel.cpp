/**
 * @file LevelViewPanel.cpp
 * @brief Implementation of LevelViewPanel
 */
#include "LevelViewPanel.h"
#include "editor/EditorContext.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

LevelViewPanel::LevelViewPanel(EditorContext& context)
    : m_context(context) {
}

void LevelViewPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        if (!m_level) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                "No Level selected");
            ImGui::Text("Select a Level from World View");
            ImGui::End();
            return;
        }
        
        // Header
        ImGui::Text("Level: %s", m_level->getName().c_str());
        ImGui::Text("ID: %s", m_level->getId().c_str());
        ImGui::Separator();
        
        // View toggle
        if (ImGui::Button(m_showGridView ? "Grid View" : "List View")) {
            m_showGridView = !m_showGridView;
        }
        
        ImGui::Separator();
        
        // Scene display
        if (m_showGridView) {
            renderSceneGrid();
        } else {
            // List view (simplified)
            const auto& scenes = m_level->getScenes();
            for (const auto& scene : scenes) {
                bool isSelected = (scene->getName() == m_selectedSceneId);
                
                if (ImGui::Selectable(scene->getName().c_str(), isSelected)) {
                    m_selectedSceneId = scene->getName();
                    m_context.selectedSceneId = scene->getName();
                }
            }
        }
        
        ImGui::Separator();
        
        renderCreateSceneButton();
    }
    ImGui::End();
#endif
}

void LevelViewPanel::renderSceneGrid() {
#ifdef HAS_IMGUI
    const auto& scenes = m_level->getScenes();
    
    if (scenes.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "No scenes in this level");
        ImGui::Text("Click 'Create New Scene' to add one");
        return;
    }
    
    // Grid layout
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columns = (int)(panelWidth / 200.0f);  // 200px per card
    if (columns < 1) columns = 1;
    
    ImGui::Columns(columns, nullptr, false);
    
    for (const auto& scene : scenes) {
        renderSceneCard(scene.get());
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
#endif
}

void LevelViewPanel::renderSceneCard(engine::Scene* scene) {
#ifdef HAS_IMGUI
    if (!scene) return;
    
    bool isSelected = (scene->getName() == m_selectedSceneId);
    
    // Card background
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 cardSize(180, 150);
    
    ImU32 bgColor = isSelected 
        ? IM_COL32(80, 120, 160, 255) 
        : IM_COL32(50, 50, 60, 255);
    
    ImGui::GetWindowDrawList()->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
        bgColor,
        4.0f);
    
    // Thumbnail area (placeholder)
    ImVec2 thumbSize(cardSize.x - 10, 100);
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(cursorPos.x + 5, cursorPos.y + 5),
        ImVec2(cursorPos.x + cardSize.x - 5, cursorPos.y + 105),
        IM_COL32(30, 30, 40, 255));
    
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + cardSize.x/2 - 30, cursorPos.y + 50));
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Preview");
    
    // Scene name
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 5, cursorPos.y + 110));
    ImGui::Text("%s", scene->getName().c_str());
    
    // Actor count
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 5, cursorPos.y + 125));
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), 
        "%zu actors", scene->getActors().size());
    
    // Make clickable
    ImGui::SetCursorScreenPos(cursorPos);
    if (ImGui::InvisibleButton(scene->getName().c_str(), cardSize)) {
        m_selectedSceneId = scene->getName();
        m_context.selectedSceneId = scene->getName();
    }
    
    // Double-click to open in Viewport
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        // TODO: Focus ViewportPanel with this scene
    }
    
    // Advance cursor
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + cardSize.y + 10));
#endif
}

void LevelViewPanel::renderCreateSceneButton() {
#ifdef HAS_IMGUI
    if (ImGui::Button("+ Create New Scene")) {
        m_showCreateDialog = true;
        m_newSceneId[0] = '\0';
        m_newSceneName[0] = '\0';
    }
    
    if (m_showCreateDialog) {
        ImGui::OpenPopup("Create Scene");
    }
    
    if (ImGui::BeginPopupModal("Create Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new Scene in this Level");
        ImGui::Separator();
        
        ImGui::InputText("ID", m_newSceneId, sizeof(m_newSceneId));
        ImGui::InputText("Name", m_newSceneName, sizeof(m_newSceneName));
        
        ImGui::Separator();
        
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            // TODO: Create scene in Level
            // auto scene = std::make_unique<Scene>(m_newSceneId);
            // m_level->addScene(std::move(scene));
            
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

void LevelViewPanel::renderSceneProperties(engine::Scene* scene) {
#ifdef HAS_IMGUI
    if (!scene) return;
    
    ImGui::Separator();
    ImGui::Text("Scene Properties:");
    
    ImGui::Text("Name: %s", scene->getName().c_str());
    ImGui::Text("Actors: %zu", scene->getActors().size());
    
    if (ImGui::Button("Open in Viewport")) {
        // TODO: Open ViewportPanel with this scene
        m_context.selectedSceneId = scene->getName();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Delete Scene")) {
        // TODO: Confirm dialog
    }
#endif
}
