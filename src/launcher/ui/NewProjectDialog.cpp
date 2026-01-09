/**
 * @file NewProjectDialog.cpp
 * @brief New project dialog implementation
 */
#include "NewProjectDialog.h"
#include "launcher/project/ProjectManager.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

#include <cstring>

namespace launcher {

NewProjectDialog::NewProjectDialog() {
    m_templates = ProjectManager::getTemplates();
}

void NewProjectDialog::show() {
    m_visible = true;
}

void NewProjectDialog::hide() {
    m_visible = false;
}

void NewProjectDialog::setDefaultPath(const std::string& path) {
    strncpy_s(m_projectPath, sizeof(m_projectPath), path.c_str(), _TRUNCATE);
}

void NewProjectDialog::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    ImGui::OpenPopup("New Project");
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 400));
    
    if (ImGui::BeginPopupModal("New Project", &m_visible)) {
        ImGui::Text("Create a new Retro Engine project");
        ImGui::Separator();
        ImGui::Spacing();
        
        ImGui::InputText("Project Name", m_projectName, sizeof(m_projectName));
        ImGui::InputText("Project Path", m_projectPath, sizeof(m_projectPath));
        
        ImGui::SameLine();
        if (ImGui::Button("Browse...")) {
            // TODO: Native file dialog
        }
        
        ImGui::Spacing();
        ImGui::Text("Template:");
        ImGui::Combo("##template", &m_selectedTemplate, 
                    [](void* data, int idx, const char** out_text) {
                        auto* templates = (std::vector<std::string>*)data;
                        *out_text = (*templates)[idx].c_str();
                        return true;
                    }, &m_templates, (int)m_templates.size());
        
        ImGui::Spacing();
        ImGui::Separator();
        
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            if (strlen(m_projectName) > 0 && strlen(m_projectPath) > 0) {
                if (m_onCreate) {
                    m_onCreate(m_projectName, m_projectPath, m_templates[m_selectedTemplate]);
                }
                m_visible = false;
                memset(m_projectName, 0, sizeof(m_projectName));
            }
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_visible = false;
        }
        
        ImGui::EndPopup();
    }
#endif
}

} // namespace launcher
