/**
 * @file QuestLogPanel.h
 * @brief Quest log panel för att visa aktiva och slutförda quests
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>
#include <vector>

class EditorContext;

/**
 * @brief Quest log panel - visar quest status och progress i editorn
 */
class QuestLogPanel : public IEditorPanel {
public:
    explicit QuestLogPanel(EditorContext& context);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
private:
    void renderQuestList();
    void renderQuestDetails(const std::string& questId);
    void renderObjectiveStatus(const std::string& description, bool completed, bool optional);
    
    EditorContext& m_context;
    std::string m_id = "quest_log";
    std::string m_title = "Quest Log";
    
    std::string m_selectedQuestId;
    bool m_showOptional = true;
};
