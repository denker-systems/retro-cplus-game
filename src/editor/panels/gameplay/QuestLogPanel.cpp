/**
 * @file QuestLogPanel.cpp
 * @brief Implementation av QuestLogPanel - editor-only quest browser
 */
#include "QuestLogPanel.h"
#include "editor/core/EditorContext.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

QuestLogPanel::QuestLogPanel(EditorContext& context)
    : m_context(context) {
}

void QuestLogPanel::render() {
#ifdef HAS_IMGUI
    // Filter toolbar
    ImGui::Checkbox("Show Optional", &m_showOptional);
    
    ImGui::Separator();
    
    // Two-column layout
    float panelWidth = ImGui::GetContentRegionAvail().x;
    
    // Left: Quest list
    ImGui::BeginChild("QuestList", ImVec2(panelWidth * 0.4f, 0), true);
    renderQuestList();
    ImGui::EndChild();
    
    ImGui::SameLine();
    
    // Right: Quest details
    ImGui::BeginChild("QuestDetails", ImVec2(0, 0), true);
    
    if (!m_selectedQuestId.empty()) {
        renderQuestDetails(m_selectedQuestId);
    } else {
        ImGui::TextDisabled("Select a quest to view details");
    }
    
    ImGui::EndChild();
#endif
}

void QuestLogPanel::renderQuestList() {
#ifdef HAS_IMGUI
    ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.5f, 0.2f, 0.5f));
    if (ImGui::CollapsingHeader("Quests", ImGuiTreeNodeFlags_DefaultOpen)) {
        
        if (m_context.quests.empty()) {
            ImGui::TextDisabled("No quests defined");
        }
        
        for (const auto& quest : m_context.quests) {
            std::string label = quest.title + " (" + std::to_string(quest.objectives.size()) + " objectives)";
            
            bool selected = (m_selectedQuestId == quest.id);
            if (ImGui::Selectable(label.c_str(), selected)) {
                m_selectedQuestId = quest.id;
            }
            
            // Tooltip with description
            if (ImGui::IsItemHovered() && !quest.description.empty()) {
                ImGui::BeginTooltip();
                ImGui::TextWrapped("%s", quest.description.c_str());
                ImGui::EndTooltip();
            }
        }
    }
    ImGui::PopStyleColor();
#endif
}

void QuestLogPanel::renderQuestDetails(const std::string& questId) {
#ifdef HAS_IMGUI
    // Find the quest
    const QuestData* quest = nullptr;
    for (const auto& q : m_context.quests) {
        if (q.id == questId) {
            quest = &q;
            break;
        }
    }
    
    if (!quest) {
        ImGui::TextDisabled("Quest not found");
        m_selectedQuestId.clear();
        return;
    }
    
    // Title
    ImGui::TextColored(ImVec4(1.0f, 0.9f, 0.3f, 1.0f), "%s", quest->title.c_str());
    ImGui::TextDisabled("ID: %s", quest->id.c_str());
    
    ImGui::Separator();
    
    // Description
    ImGui::Text("Description:");
    ImGui::TextWrapped("%s", quest->description.c_str());
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Objectives header
    ImGui::Text("Objectives (%d):", (int)quest->objectives.size());
    ImGui::Spacing();
    
    for (const auto& obj : quest->objectives) {
        if (!m_showOptional && obj.optional) continue;
        renderObjectiveStatus(obj.description, false, obj.optional);
        
        // Show waypoint info if set
        if (obj.showOnCompass) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.5f, 0.8f, 1.0f, 1.0f), "[Compass]");
        }
    }
    
    // Rewards section
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::Text("Rewards:");
    if (!quest->rewardItem.empty()) {
        ImGui::BulletText("Item: %s", quest->rewardItem.c_str());
    }
    if (quest->rewardXP > 0) {
        ImGui::BulletText("XP: %d", quest->rewardXP);
    }
    if (quest->rewardItem.empty() && quest->rewardXP == 0) {
        ImGui::TextDisabled("No rewards");
    }
    
    // Auto-start info
    if (quest->autoStart) {
        ImGui::Spacing();
        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Auto-starts at game begin");
    }
#endif
}

void QuestLogPanel::renderObjectiveStatus(const std::string& description, bool completed, bool optional) {
#ifdef HAS_IMGUI
    // Checkbox style indicator
    ImVec4 color;
    const char* icon;
    
    if (completed) {
        color = ImVec4(0.3f, 1.0f, 0.3f, 1.0f);  // Green
        icon = "[X]";
    } else {
        color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);  // White
        icon = "[ ]";
    }
    
    ImGui::TextColored(color, "%s", icon);
    ImGui::SameLine();
    
    // Strike-through effect for completed
    if (completed) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
    }
    
    ImGui::TextWrapped("%s", description.c_str());
    
    if (completed) {
        ImGui::PopStyleColor();
    }
    
    // Optional marker
    if (optional) {
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.8f, 1.0f), "(Optional)");
    }
#endif
}
