/**
 * @file BehaviorNode.h
 * @brief Graph nodes for NPC behavior visualization
 */
#pragma once

#include "editor/panels/graphs/IGraphNode.h"
#include "engine/data/GameData.h"
#include <vector>
#include <string>

/**
 * @brief Represents an NPC header node in the behavior graph
 */
class NPCHeaderNode : public IGraphNode {
public:
    explicit NPCHeaderNode(const NPCData& data, int nodeId);
    
    void renderContent() override;
    std::vector<int> getInputAttrIds() const override;
    std::vector<int> getOutputAttrIds() const override;
    unsigned int getTitleBarColor() const override;
    
    std::string npcId;
    std::string description;
    std::string sprite;
    std::string room;
    int posX = 0, posY = 0;
    bool canTalk = true;
    bool canMove = false;
    float moveSpeed = 50.0f;
    
    int dialogOutputAttr = 0;
    int roomOutputAttr = 0;
};

/**
 * @brief Represents a dialog connection node
 */
class DialogRefNode : public IGraphNode {
public:
    explicit DialogRefNode(const std::string& dialogId, int nodeId);
    
    void renderContent() override;
    std::vector<int> getInputAttrIds() const override;
    std::vector<int> getOutputAttrIds() const override;
    unsigned int getTitleBarColor() const override;
    
    std::string dialogId;
    int inputAttrId = 0;
};

/**
 * @brief Represents a room location node
 */
class RoomRefNode : public IGraphNode {
public:
    explicit RoomRefNode(const std::string& roomId, int nodeId);
    
    void renderContent() override;
    std::vector<int> getInputAttrIds() const override;
    std::vector<int> getOutputAttrIds() const override;
    unsigned int getTitleBarColor() const override;
    
    std::string roomId;
    int inputAttrId = 0;
};

/**
 * @brief Represents a behavior state node (idle, patrol, follow, etc.)
 */
class BehaviorStateNode : public IGraphNode {
public:
    enum class StateType { Idle, Patrol, Follow, Flee, Custom };
    
    explicit BehaviorStateNode(StateType type, int nodeId);
    
    void renderContent() override;
    std::vector<int> getInputAttrIds() const override;
    std::vector<int> getOutputAttrIds() const override;
    unsigned int getTitleBarColor() const override;
    
    StateType stateType = StateType::Idle;
    int inputAttrId = 0;
    int outputAttrId = 0;
};
