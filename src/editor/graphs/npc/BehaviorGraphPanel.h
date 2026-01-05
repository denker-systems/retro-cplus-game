/**
 * @file BehaviorGraphPanel.h
 * @brief NPC Behavior Graph Editor Panel using imnodes
 */
#pragma once

#include "editor/graphs/INodeGraphPanel.h"
#include "BehaviorNode.h"

/**
 * @brief Node graph editor for NPC behavior visualization
 * 
 * Shows NPC structure with:
 * - NPC header node with properties
 * - Dialog connection
 * - Room location
 * - Behavior states (if canMove)
 */
class BehaviorGraphPanel : public INodeGraphPanel {
public:
    explicit BehaviorGraphPanel(EditorContext& context);
    
protected:
    void onNodeSelected(int nodeId) override;
    void onLinkCreated(int startAttr, int endAttr) override;
    void onLinkDeleted(int linkId) override;
    void populateFromData() override;
    void syncToData() override;
    void renderHeader() override;
    
private:
    void loadNPC(const std::string& npcId);
    void createLinks();
};
