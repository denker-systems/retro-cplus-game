/**
 * @file ViewportPanel_Core.cpp
 * @brief ViewportPanel core functionality - constructor, render, toolbar, breadcrumbs
 */
#include "ViewportPanel.h"
#include "editor/core/EditorContext.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/data/DataLoader.h"
#include <SDL_image.h>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ViewportPanel::ViewportPanel(EditorContext& context)
    : m_context(context) {
}

ViewportPanel::~ViewportPanel() {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
    }
}

void ViewportPanel::update(float deltaTime) {
    // Load room preview if selection changed
    if (m_context.selectedRoomId != m_loadedRoomId) {
        loadRoomPreview();
    }
}

void ViewportPanel::loadRoomPreview() {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
        m_roomTexture = nullptr;
    }
    
    m_loadedRoomId = m_context.selectedRoomId;
    
    if (m_loadedRoomId.empty() || !m_renderer) return;
    
    // Find room data
    const auto& rooms = DataLoader::instance().getRooms();
    const RoomData* room = nullptr;
    for (const auto& r : rooms) {
        if (r.id == m_loadedRoomId) {
            room = &r;
            break;
        }
    }
    
    if (!room) return;
    
    // Load background texture
    std::string path = "assets/backgrounds/" + room->background;
    m_roomTexture = IMG_LoadTexture(m_renderer, path.c_str());
}

void ViewportPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        renderBreadcrumbs();
        ImGui::Separator();
        
        // Render different views based on breadcrumb level
        if (!m_level) {
            // World level - show all levels
            renderWorldView();
        } else if (!m_scene) {
            // Level level - show all scenes
            renderLevelView();
        } else {
            // Scene level - show scene content
            renderToolbar();
            renderSceneView();
        }
    }
    ImGui::End();
#endif
}

void ViewportPanel::renderToolbar() {
#ifdef HAS_IMGUI
    // Tool buttons
    m_toolManager.renderToolbar();
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    
    // Zoom controls
    ImGui::Text("Zoom: %.0f%%", m_zoom * 100);
    ImGui::SameLine();
    if (ImGui::Button("-")) m_zoom = std::max(0.25f, m_zoom - 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("+")) m_zoom = std::min(4.0f, m_zoom + 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("100%")) { m_zoom = 1.0f; m_panX = 0; m_panY = 0; }
    
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    
    // View toggles
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Hotspots", &m_showHotspots);
    ImGui::SameLine();
    ImGui::Checkbox("Walk Area", &m_showWalkArea);
    ImGui::SameLine();
    ImGui::Checkbox("Physics", &m_showPhysicsDebug);
#endif
}

void ViewportPanel::renderBreadcrumbs() {
#ifdef HAS_IMGUI
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));
    
    // World level
    if (m_world) {
        if (ImGui::Button("🌍 World")) {
            // Navigate to world view (show all levels)
            m_level = nullptr;
            m_scene = nullptr;
        }
        
        // Level level
        if (m_level) {
            ImGui::SameLine();
            ImGui::TextDisabled(">");
            ImGui::SameLine();
            
            if (ImGui::Button(("📁 " + m_level->getName()).c_str())) {
                // Navigate to level view (show all scenes)
                m_scene = nullptr;
            }
            
            // Scene level
            if (m_scene) {
                ImGui::SameLine();
                ImGui::TextDisabled(">");
                ImGui::SameLine();
                
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.3f, 1.0f));
                ImGui::Button(("🎬 " + m_scene->getName()).c_str());
                ImGui::PopStyleColor();
            }
        }
    } else {
        ImGui::TextDisabled("No World loaded");
    }
    
    ImGui::PopStyleColor(2);
#endif
}

void ViewportPanel::setLevel(engine::Level* level) {
    m_level = level;
    // Don't auto-select scene - let user see Level View first
    m_scene = nullptr;
}

void ViewportPanel::setScene(engine::Scene* scene) {
    m_scene = scene;
}
