/**
 * @file INodeGraphPanel.h
 * @brief Base class for node graph editor panels
 */
#pragma once

#include "editor/IEditorPanel.h"
#include "editor/EditorContext.h"
#include "IGraphNode.h"
#include "Link.h"
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

/**
 * @brief Abstract base class for node graph editor panels
 * 
 * Provides common functionality for:
 * - Rendering nodes and links using imnodes
 * - Tree layout algorithm
 * - Link creation/deletion
 * - Node selection
 */
class INodeGraphPanel : public IEditorPanel {
public:
    explicit INodeGraphPanel(EditorContext& context, 
                             const std::string& id, 
                             const std::string& title);
    virtual ~INodeGraphPanel();
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
protected:
    // Core rendering
    void renderGraph();
    void renderNodes();
    void renderLinks();
    
    // Tree layout
    void calculateTreeLayout();
    void applyTreeLayout();
    
    // Abstract - subclasses implement
    virtual void onNodeSelected(int nodeId) = 0;
    virtual void onLinkCreated(int startAttr, int endAttr) = 0;
    virtual void onLinkDeleted(int linkId) = 0;
    virtual void populateFromData() = 0;
    virtual void syncToData() = 0;
    virtual void renderHeader() = 0;
    
    // Helpers
    int generateId() { return m_nextId++; }
    void clearGraph();
    
    // Data
    EditorContext& m_context;
    std::vector<std::unique_ptr<IGraphNode>> m_nodes;
    std::vector<Link> m_links;
    
    // State
    int m_nextId = 1;
    int m_selectedNodeId = -1;
    bool m_needsLayout = true;
    std::string m_currentDataId;  // Track which data is loaded
    
    // Layout data
    std::unordered_map<int, int> m_nodeDepth;
    std::unordered_map<int, int> m_nodeOrder;
    std::unordered_map<int, int> m_nodesPerLevel;
    
    // Layout constants
    static constexpr float NODE_SPACING_X = 350.0f;
    static constexpr float NODE_SPACING_Y = 200.0f;
    
private:
    std::string m_id;
    std::string m_title;
};
