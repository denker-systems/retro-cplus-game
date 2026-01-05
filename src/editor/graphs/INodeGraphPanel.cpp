/**
 * @file INodeGraphPanel.cpp
 * @brief Base implementation for node graph editor panels
 */
#include "INodeGraphPanel.h"
#include <queue>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

#ifdef HAS_IMNODES
#include <imnodes/imnodes.h>
#endif

INodeGraphPanel::INodeGraphPanel(EditorContext& context, 
                                 const std::string& id, 
                                 const std::string& title)
    : m_context(context), m_id(id), m_title(title) {
#ifdef HAS_IMNODES
    ImNodes::CreateContext();
#endif
}

INodeGraphPanel::~INodeGraphPanel() {
#ifdef HAS_IMNODES
    ImNodes::DestroyContext();
#endif
}

void INodeGraphPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        renderHeader();
        renderGraph();
    }
    ImGui::End();
#endif
}

void INodeGraphPanel::renderGraph() {
#ifdef HAS_IMNODES
    ImNodes::BeginNodeEditor();
    
    // Apply layout if needed
    if (m_needsLayout && !m_nodes.empty()) {
        calculateTreeLayout();
        applyTreeLayout();
        m_needsLayout = false;
    }
    
    renderNodes();
    renderLinks();
    
    // Mini-map
    ImNodes::MiniMap(0.2f, ImNodesMiniMapLocation_BottomRight);
    
    ImNodes::EndNodeEditor();
    
    // Handle link creation
    int startAttr, endAttr;
    if (ImNodes::IsLinkCreated(&startAttr, &endAttr)) {
        m_links.push_back(Link(generateId(), startAttr, endAttr));
        onLinkCreated(startAttr, endAttr);
    }
    
    // Handle link deletion
    int linkId;
    if (ImNodes::IsLinkDestroyed(&linkId)) {
        auto it = std::find_if(m_links.begin(), m_links.end(),
            [linkId](const Link& l) { return l.id == linkId; });
        if (it != m_links.end()) {
            onLinkDeleted(linkId);
            m_links.erase(it);
        }
    }
    
    // Handle node selection
    int numSelected = ImNodes::NumSelectedNodes();
    if (numSelected > 0) {
        std::vector<int> selectedNodes(numSelected);
        ImNodes::GetSelectedNodes(selectedNodes.data());
        if (m_selectedNodeId != selectedNodes[0]) {
            m_selectedNodeId = selectedNodes[0];
            onNodeSelected(m_selectedNodeId);
        }
    }
#endif
}

void INodeGraphPanel::renderNodes() {
#ifdef HAS_IMNODES
    for (const auto& node : m_nodes) {
        ImNodes::BeginNode(node->id);
        
        // Title bar
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node->title.c_str());
        ImNodes::EndNodeTitleBar();
        
        // Node content (implemented by subclass)
        node->renderContent();
        
        ImNodes::EndNode();
    }
#endif
}

void INodeGraphPanel::renderLinks() {
#ifdef HAS_IMNODES
    for (const auto& link : m_links) {
        ImNodes::Link(link.id, link.startAttr, link.endAttr);
    }
#endif
}

void INodeGraphPanel::calculateTreeLayout() {
    m_nodeDepth.clear();
    m_nodeOrder.clear();
    m_nodesPerLevel.clear();
    
    if (m_nodes.empty()) return;
    
    // Build adjacency from links
    std::unordered_map<int, std::vector<int>> children;
    std::unordered_set<int> hasParent;
    
    // Map attribute ID to node ID
    std::unordered_map<int, int> attrToNode;
    for (const auto& node : m_nodes) {
        for (int attrId : node->getInputAttrIds()) {
            attrToNode[attrId] = node->id;
        }
        for (int attrId : node->getOutputAttrIds()) {
            attrToNode[attrId] = node->id;
        }
    }
    
    // Build parent-child relationships from links
    for (const auto& link : m_links) {
        int parentNode = attrToNode.count(link.startAttr) ? attrToNode[link.startAttr] : -1;
        int childNode = attrToNode.count(link.endAttr) ? attrToNode[link.endAttr] : -1;
        
        if (parentNode >= 0 && childNode >= 0) {
            children[parentNode].push_back(childNode);
            hasParent.insert(childNode);
        }
    }
    
    // Find root nodes (no parent)
    std::vector<int> roots;
    for (const auto& node : m_nodes) {
        if (hasParent.find(node->id) == hasParent.end()) {
            roots.push_back(node->id);
        }
    }
    
    // If no roots found, use first node
    if (roots.empty() && !m_nodes.empty()) {
        roots.push_back(m_nodes[0]->id);
    }
    
    // BFS to calculate depth
    std::queue<int> queue;
    std::unordered_set<int> visited;
    
    for (int root : roots) {
        queue.push(root);
        m_nodeDepth[root] = 0;
        visited.insert(root);
    }
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        int currentDepth = m_nodeDepth[current];
        
        for (int child : children[current]) {
            if (visited.find(child) == visited.end()) {
                visited.insert(child);
                m_nodeDepth[child] = currentDepth + 1;
                queue.push(child);
            }
        }
    }
    
    // Handle disconnected nodes
    for (const auto& node : m_nodes) {
        if (visited.find(node->id) == visited.end()) {
            m_nodeDepth[node->id] = 0;
        }
    }
    
    // Calculate order within each level
    std::unordered_map<int, int> levelCounter;
    for (const auto& node : m_nodes) {
        int depth = m_nodeDepth[node->id];
        m_nodeOrder[node->id] = levelCounter[depth];
        levelCounter[depth]++;
    }
    
    m_nodesPerLevel = levelCounter;
}

void INodeGraphPanel::applyTreeLayout() {
#ifdef HAS_IMNODES
    // First pass: assign positions based on depth and order
    // Use grid space positions for consistent layout
    
    const float baseX = 50.0f;
    const float baseY = 50.0f;
    
    for (const auto& node : m_nodes) {
        int depth = m_nodeDepth.count(node->id) ? m_nodeDepth[node->id] : 0;
        int order = m_nodeOrder.count(node->id) ? m_nodeOrder[node->id] : 0;
        
        // Horizontal layout: depth determines X (left to right tree)
        // Vertical layout: order determines Y (spread nodes vertically)
        float x = baseX + depth * NODE_SPACING_X;
        float y = baseY + order * NODE_SPACING_Y;
        
        ImNodes::SetNodeGridSpacePos(node->id, ImVec2(x, y));
    }
#endif
}

void INodeGraphPanel::clearGraph() {
    m_nodes.clear();
    m_links.clear();
    m_nextId = 1;
    m_selectedNodeId = -1;
    m_needsLayout = true;
    m_nodeDepth.clear();
    m_nodeOrder.clear();
    m_nodesPerLevel.clear();
}
