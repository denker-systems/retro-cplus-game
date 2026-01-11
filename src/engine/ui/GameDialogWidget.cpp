/**
 * @file GameDialogWidget.cpp
 * @brief Implementation av delad dialog-UI widget
 */
#include "GameDialogWidget.h"

#ifdef HAS_IMGUI

#include <imgui.h>

void GameDialogWidget::setDialog(const std::string& speaker, const std::string& text,
                                  const std::vector<std::pair<int, std::string>>& choices) {
    m_speaker = speaker;
    m_text = text;
    m_choices = choices;
    m_active = true;
}

void GameDialogWidget::clear() {
    m_speaker.clear();
    m_text.clear();
    m_choices.clear();
    m_active = false;
}

int GameDialogWidget::render(int windowWidth, int windowHeight) {
    if (!m_active) return -1;
    
    int selectedChoice = -1;
    
    // Dialog box dimensions
    float boxHeight = 180.0f;
    float boxWidth = windowWidth - 40.0f;
    float boxX = 20.0f;
    float boxY = windowHeight - boxHeight - 20.0f;
    
    // Setup ImGui window
    ImGui::SetNextWindowPos(ImVec2(boxX, boxY));
    ImGui::SetNextWindowSize(ImVec2(boxWidth, boxHeight));
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
                             ImGuiWindowFlags_NoResize |
                             ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoScrollbar |
                             ImGuiWindowFlags_NoCollapse;
    
    // Dark semi-transparent background
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.08f, 0.12f, 0.24f, 0.95f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.4f, 0.6f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 12.0f));
    
    if (ImGui::Begin("##DialogBox", nullptr, flags)) {
        // Speaker name (gold/yellow)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.85f, 0.3f, 1.0f));
        ImGui::SetWindowFontScale(1.2f);
        ImGui::Text("%s", m_speaker.c_str());
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Dialog text (white)
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::TextWrapped("%s", m_text.c_str());
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
        ImGui::Spacing();
        
        // Choices or continue prompt
        if (!m_choices.empty()) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 1.0f, 1.0f, 1.0f));
            
            for (size_t i = 0; i < m_choices.size(); i++) {
                std::string label = "[" + std::to_string(i + 1) + "] " + m_choices[i].second;
                
                // Highlight on hover
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.3f, 0.5f, 0.5f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.3f, 0.4f, 0.6f, 0.7f));
                
                if (ImGui::Button(label.c_str(), ImVec2(-1, 0))) {
                    selectedChoice = static_cast<int>(i);
                    if (m_onChoice) {
                        m_onChoice(selectedChoice);
                    }
                }
                
                ImGui::PopStyleColor(3);
            }
            
            ImGui::PopStyleColor();
        } else {
            // No choices - show continue prompt
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
            ImGui::Text("[E] Continue...");
            ImGui::PopStyleColor();
        }
    }
    ImGui::End();
    
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    
    return selectedChoice;
}

#endif // HAS_IMGUI
