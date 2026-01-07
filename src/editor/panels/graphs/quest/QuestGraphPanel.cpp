/**
 * @file QuestGraphPanel.cpp
 * @brief Implementation of Quest Graph Editor Panel
 */
#include "QuestGraphPanel.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

QuestGraphPanel::QuestGraphPanel(EditorContext& context)
    : INodeGraphPanel(context, "quest_graph", "Quest Graph") {
}

void QuestGraphPanel::renderHeader() {
#ifdef HAS_IMGUI
    // Check if a quest is selected
    if (m_context.selectedType != EditorContext::SelectionType::Quest ||
        m_context.selectedQuestId.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "Select a quest in the Hierarchy panel");
        return;
    }
    
    // Reload if quest changed
    if (m_currentDataId != m_context.selectedQuestId) {
        loadQuest(m_context.selectedQuestId);
    }
    
    // Header info
    ImGui::Text("Quest: %s", m_context.selectedQuestId.c_str());
    
    // Find quest for title
    for (const auto& q : m_context.quests) {
        if (q.id == m_context.selectedQuestId) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.6f, 1.0f, 0.6f, 1.0f), 
                "| %s", q.title.c_str());
            break;
        }
    }
    
    // Toolbar
    if (ImGui::Button("Re-layout")) {
        m_needsLayout = true;
    }
    
    ImGui::Separator();
#endif
}

void QuestGraphPanel::loadQuest(const std::string& questId) {
    clearGraph();
    m_currentDataId = questId;
    
    // Find quest data
    QuestData* quest = nullptr;
    for (auto& q : m_context.quests) {
        if (q.id == questId) {
            quest = &q;
            break;
        }
    }
    
    if (!quest) return;
    
    // Create quest header node
    auto headerNode = std::make_unique<QuestHeaderNode>(*quest, generateId());
    
    // Generate output attributes for each objective
    for (size_t i = 0; i < quest->objectives.size(); i++) {
        headerNode->outputAttrIds.push_back(generateId());
    }
    
    m_nodes.push_back(std::move(headerNode));
    
    // Create objective nodes
    for (size_t i = 0; i < quest->objectives.size(); i++) {
        auto objNode = std::make_unique<ObjectiveNode>(
            quest->objectives[i], generateId(), static_cast<int>(i));
        objNode->inputAttrId = generateId();
        objNode->outputAttrId = generateId();
        m_nodes.push_back(std::move(objNode));
    }
    
    // Create links
    createLinks();
    
    m_needsLayout = true;
}

void QuestGraphPanel::createLinks() {
    if (m_nodes.empty()) return;
    
    // First node is header
    QuestHeaderNode* header = static_cast<QuestHeaderNode*>(m_nodes[0].get());
    
    // Link header to each objective
    for (size_t i = 0; i < header->outputAttrIds.size() && i + 1 < m_nodes.size(); i++) {
        ObjectiveNode* objNode = static_cast<ObjectiveNode*>(m_nodes[i + 1].get());
        
        m_links.push_back(Link(
            generateId(),
            header->outputAttrIds[i],
            objNode->inputAttrId
        ));
    }
    
    // Chain objectives together (sequential flow)
    for (size_t i = 1; i < m_nodes.size() - 1; i++) {
        ObjectiveNode* current = static_cast<ObjectiveNode*>(m_nodes[i].get());
        ObjectiveNode* next = static_cast<ObjectiveNode*>(m_nodes[i + 1].get());
        
        m_links.push_back(Link(
            generateId(),
            current->outputAttrId,
            next->inputAttrId
        ));
    }
}

void QuestGraphPanel::onNodeSelected(int nodeId) {
    // Could update properties panel
}

void QuestGraphPanel::onLinkCreated(int startAttr, int endAttr) {
    // Quest structure is fixed, links are informational
}

void QuestGraphPanel::onLinkDeleted(int linkId) {
    // Quest structure is fixed
}

void QuestGraphPanel::populateFromData() {
    if (!m_context.selectedQuestId.empty()) {
        loadQuest(m_context.selectedQuestId);
    }
}

void QuestGraphPanel::syncToData() {
    // Quest editing done via properties panel
}
