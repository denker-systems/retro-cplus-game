/**
 * @file ProjectGrid.cpp
 * @brief Project grid implementation
 */
#include "ProjectGrid.h"
#include "editor/ui/EditorTheme.h"
#include <SDL_image.h>
#include <iostream>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace launcher {

ProjectGrid::ProjectGrid() {
}

ProjectGrid::~ProjectGrid() {
    if (m_defaultThumbnail) {
        SDL_DestroyTexture(m_defaultThumbnail);
        m_defaultThumbnail = nullptr;
    }
}

void ProjectGrid::initialize(SDL_Renderer* renderer) {
    m_renderer = renderer;
    
    std::cout << "[ProjectGrid] Loading default thumbnail..." << std::endl;
    m_defaultThumbnail = IMG_LoadTexture(renderer, "retroengine.png");
    if (m_defaultThumbnail) {
        std::cout << "[ProjectGrid] ✓ Default thumbnail loaded" << std::endl;
    } else {
        std::cout << "[ProjectGrid] WARNING: Default thumbnail not found" << std::endl;
    }
}

void ProjectGrid::render(const std::vector<RetroProject>& projects, float screenWidth, float screenHeight, float headerHeight) {
#ifdef HAS_IMGUI
    ImGui::SetNextWindowPos(ImVec2(0, headerHeight));
    ImGui::SetNextWindowSize(ImVec2(screenWidth, screenHeight - headerHeight));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    
    ImGui::Begin("Projects", nullptr, flags);
    
    ImGui::Text("MY PROJECTS");
    ImGui::Separator();
    ImGui::Spacing();
    
    if (projects.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No projects yet. Click 'New Project' to get started!");
    } else {
        // Optimized card size
        float cardWidth = 280.0f;
        float cardHeight = 240.0f;
        int columns = (int)((screenWidth - 40) / (cardWidth + 20));
        if (columns < 1) columns = 1;
        
        for (size_t i = 0; i < projects.size(); i++) {
            renderProjectCard(projects[i], (int)i);
            
            if ((i + 1) % columns != 0 && i < projects.size() - 1) {
                ImGui::SameLine();
            }
        }
    }
    
    ImGui::End();
#endif
}

void ProjectGrid::renderProjectCard(const RetroProject& project, int index) {
#ifdef HAS_IMGUI
    ImGui::BeginChild(("Project" + std::to_string(index)).c_str(), ImVec2(280, 240), true);
    
    // Thumbnail with Retro Engine logo
    ImGui::SetCursorPosX(10);
    ImGui::SetCursorPosY(10);
    
    if (m_defaultThumbnail) {
        // Get texture size and scale to fit
        int texW, texH;
        SDL_QueryTexture(m_defaultThumbnail, nullptr, nullptr, &texW, &texH);
        
        float maxW = 260.0f;
        float maxH = 120.0f;
        float scale = std::min(maxW / texW, maxH / texH);
        float displayW = texW * scale;
        float displayH = texH * scale;
        
        // Center the image
        float offsetX = (maxW - displayW) / 2.0f;
        ImGui::SetCursorPosX(10 + offsetX);
        
        ImGui::Image(reinterpret_cast<ImTextureID>(m_defaultThumbnail), ImVec2(displayW, displayH));
    } else {
        // Fallback to colored box
        ImGui::PushStyleColor(ImGuiCol_Button, editor::EditorTheme::BrandColors::Primary);
        ImGui::Button("##thumbnail", ImVec2(260, 120));
        ImGui::PopStyleColor();
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    
    // Project info (compact)
    ImGui::SetCursorPosX(10);
    ImGui::PushStyleColor(ImGuiCol_Text, editor::EditorTheme::BrandColors::Primary);
    ImGui::Text("%s", project.name.c_str());
    ImGui::PopStyleColor();
    
    ImGui::SetCursorPosX(10);
    ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", project.lastModified.c_str());
    
    ImGui::Spacing();
    
    // Action buttons
    ImGui::SetCursorPosX(10);
    if (ImGui::Button("Open Editor", ImVec2(85, 32))) {
        if (m_onOpenEditor) {
            m_onOpenEditor(project);
        }
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Play", ImVec2(85, 32))) {
        if (m_onLaunchGame) {
            m_onLaunchGame(project);
        }
    }
    
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.3f, 0.3f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
    if (ImGui::Button("Delete", ImVec2(85, 32))) {
        ImGui::OpenPopup(("Delete Project##" + std::to_string(index)).c_str());
    }
    ImGui::PopStyleColor(3);
    
    // Delete confirmation dialog
    if (ImGui::BeginPopupModal(("Delete Project##" + std::to_string(index)).c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Are you sure you want to delete this project?");
        ImGui::Spacing();
        ImGui::TextColored(editor::EditorTheme::BrandColors::Primary, "%s", project.name.c_str());
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "%s", project.path.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
        if (ImGui::Button("Delete", ImVec2(120, 0))) {
            if (m_onDeleteProject) {
                m_onDeleteProject(project);
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::PopStyleColor();
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::EndChild();
#endif
}

} // namespace launcher
