/**
 * @file IGraphNode.h
 * @brief Base class for all node graph nodes
 */
#pragma once

#include <string>
#include <vector>

/**
 * @brief Abstract base class for graph nodes
 * 
 * All node types (Dialog, Quest, Behavior) inherit from this.
 * Provides common interface for rendering and attribute management.
 */
class IGraphNode {
public:
    virtual ~IGraphNode() = default;
    
    // Unique node ID
    int id = 0;
    
    // Position (managed by imnodes, but we track for layout)
    float posX = 0.0f;
    float posY = 0.0f;
    
    // Display
    std::string title;
    
    /**
     * @brief Render this node using imnodes
     * Called between ImNodes::BeginNode and EndNode
     */
    virtual void renderContent() = 0;
    
    /**
     * @brief Get all input attribute IDs for this node
     */
    virtual std::vector<int> getInputAttrIds() const = 0;
    
    /**
     * @brief Get all output attribute IDs for this node
     */
    virtual std::vector<int> getOutputAttrIds() const = 0;
    
    /**
     * @brief Get title bar color (RGBA as ImU32)
     * Default is dark gray
     */
    virtual unsigned int getTitleBarColor() const { 
        return 0xFF323232; // IM_COL32(50, 50, 50, 255)
    }
};
