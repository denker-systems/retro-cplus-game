/**
 * @file QuestNode.h
 * @brief Graph nodes for quest system visualization
 */
#pragma once

#include "editor/panels/graphs/IGraphNode.h"
#include "engine/data/GameData.h"
#include <vector>
#include <string>

/**
 * @brief Represents a quest header node in the graph
 */
class QuestHeaderNode : public IGraphNode {
public:
    explicit QuestHeaderNode(const QuestData& data, int nodeId);
    
    void renderContent() override;
    std::vector<int> getInputAttrIds() const override;
    std::vector<int> getOutputAttrIds() const override;
    unsigned int getTitleBarColor() const override;
    
    std::string questId;
    std::string description;
    std::string rewardItem;
    int rewardXP = 0;
    
    std::vector<int> outputAttrIds;  // One per objective
};

/**
 * @brief Represents an objective node in the quest graph
 */
class ObjectiveNode : public IGraphNode {
public:
    explicit ObjectiveNode(const ObjectiveData& data, int nodeId, int index);
    
    void renderContent() override;
    std::vector<int> getInputAttrIds() const override;
    std::vector<int> getOutputAttrIds() const override;
    unsigned int getTitleBarColor() const override;
    
    std::string objectiveId;
    std::string description;
    std::string type;
    std::string targetId;
    int requiredCount = 1;
    bool optional = false;
    int objectiveIndex = 0;
    
    int inputAttrId = 0;
    int outputAttrId = 0;  // For chaining objectives
};
