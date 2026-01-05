/**
 * @file DialogGraphPanel.h
 * @brief Dialog Graph Editor Panel using imnodes
 */
#pragma once

#include "editor/graphs/INodeGraphPanel.h"
#include "DialogNode.h"

/**
 * @brief Node graph editor for dialog trees
 * 
 * Visualizes DialogData as a node graph where:
 * - Each DialogNodeData becomes a DialogNode
 * - Choices and nextNodeId create links between nodes
 */
class DialogGraphPanel : public INodeGraphPanel {
public:
    explicit DialogGraphPanel(EditorContext& context);
    
protected:
    void onNodeSelected(int nodeId) override;
    void onLinkCreated(int startAttr, int endAttr) override;
    void onLinkDeleted(int linkId) override;
    void populateFromData() override;
    void syncToData() override;
    void renderHeader() override;
    
private:
    void loadDialog(const std::string& dialogId);
    void createLinks();
    
    // Maps for attribute <-> node lookup
    std::unordered_map<int, int> m_attrToDialogNodeId;
    std::unordered_map<int, size_t> m_attrToChoiceIndex;
};
