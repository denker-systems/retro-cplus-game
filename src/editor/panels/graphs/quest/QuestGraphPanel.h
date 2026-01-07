/**
 * @file QuestGraphPanel.h
 * @brief Quest Graph Editor Panel using imnodes
 */
#pragma once

#include "editor/panels/graphs/INodeGraphPanel.h"
#include "QuestNode.h"

/**
 * @brief Node graph editor for quest visualization
 * 
 * Shows quest structure with:
 * - Quest header node with title and rewards
 * - Objective nodes connected in sequence
 */
class QuestGraphPanel : public INodeGraphPanel {
public:
    explicit QuestGraphPanel(EditorContext& context);
    
protected:
    void onNodeSelected(int nodeId) override;
    void onLinkCreated(int startAttr, int endAttr) override;
    void onLinkDeleted(int linkId) override;
    void populateFromData() override;
    void syncToData() override;
    void renderHeader() override;
    
private:
    void loadQuest(const std::string& questId);
    void createLinks();
};
