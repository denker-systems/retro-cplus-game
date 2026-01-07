/**
 * @file DialogNode.h
 * @brief Graph node for dialog system
 */
#pragma once

#include "editor/panels/graphs/IGraphNode.h"
#include "engine/data/GameData.h"
#include <vector>
#include <string>

/**
 * @brief Represents a dialog node in the graph editor
 */
class DialogNode : public IGraphNode {
public:
    explicit DialogNode(const DialogNodeData& data, int nodeId);
    
    void renderContent() override;
    std::vector<int> getInputAttrIds() const override;
    std::vector<int> getOutputAttrIds() const override;
    unsigned int getTitleBarColor() const override;
    
    // Data
    int dialogNodeId = 0;  // Original ID from DialogNodeData
    std::string speaker;
    std::string text;
    std::vector<DialogChoiceData> choices;
    int nextNodeId = -1;
    bool isStartNode = false;
    
    // Attribute IDs (generated)
    int inputAttrId = 0;
    std::vector<int> outputAttrIds;
    
private:
    void truncateText(std::string& str, size_t maxLen) const;
};
