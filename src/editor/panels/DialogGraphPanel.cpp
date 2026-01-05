/**
 * @file DialogGraphPanel.cpp
 * @brief Implementation av Dialog Graph Editor Panel
 */
#include "DialogGraphPanel.h"
#include <queue>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

#ifdef HAS_IMGUI_NODE_EDITOR
#include <imgui-node-editor/imgui_node_editor.h>
namespace ed = ax::NodeEditor;
#endif

// Layout constants
static const float NODE_WIDTH = 300.0f;
static const float NODE_HEIGHT = 180.0f;
static const float HORIZONTAL_SPACING = 400.0f;
static const float VERTICAL_SPACING = 250.0f;

DialogGraphPanel::DialogGraphPanel(EditorContext& context)
    : m_context(context) {
#ifdef HAS_IMGUI_NODE_EDITOR
    // Skapa node editor context
    ed::Config config;
    config.SettingsFile = "dialog_graph.json";
    m_editorContext = ed::CreateEditor(&config);
#endif
}

DialogGraphPanel::~DialogGraphPanel() {
#ifdef HAS_IMGUI_NODE_EDITOR
    if (m_editorContext) {
        ed::DestroyEditor(static_cast<ed::EditorContext*>(m_editorContext));
    }
#endif
}

void DialogGraphPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Hitta vald dialog
        DialogData* dialog = nullptr;
        if (m_context.selectedType == EditorContext::SelectionType::Dialog && 
            !m_context.selectedDialogId.empty()) {
            for (auto& d : m_context.dialogs) {
                if (d.id == m_context.selectedDialogId) {
                    dialog = &d;
                    break;
                }
            }
        }
        
        if (!dialog) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                "Select a dialog in the Hierarchy panel");
        } else {
            ImGui::Text("Dialog: %s", dialog->id.c_str());
            ImGui::Text("NPC: %s", dialog->npcName.c_str());
            ImGui::Separator();
            
#ifdef HAS_IMGUI_NODE_EDITOR
            renderNodeEditor();
#else
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), 
                "Dialog Graph Editor requires imgui-node-editor");
            ImGui::Text("Install via: vcpkg install imgui-node-editor:x64-windows");
#endif
        }
    }
    ImGui::End();
#endif
}

void DialogGraphPanel::renderNodeEditor() {
#ifdef HAS_IMGUI_NODE_EDITOR
    // Hitta vald dialog
    DialogData* dialog = nullptr;
    for (auto& d : m_context.dialogs) {
        if (d.id == m_context.selectedDialogId) {
            dialog = &d;
            break;
        }
    }
    
    if (!dialog) return;
    
    // Kolla om dialog har ändrats - behöver ny layout
    if (m_lastDialogId != dialog->id) {
        m_lastDialogId = dialog->id;
        m_needsLayout = true;
    }
    
    // Beräkna tree layout om det behövs
    if (m_needsLayout) {
        calculateTreeLayout(dialog);
    }
    
    ed::SetCurrentEditor(static_cast<ed::EditorContext*>(m_editorContext));
    ed::Begin("Dialog Graph");
    
    // Rendera alla noder
    for (size_t i = 0; i < dialog->nodes.size(); i++) {
        auto& node = dialog->nodes[i];
        
        // Node ID måste vara unikt
        ed::NodeId nodeId = ed::NodeId(node.id + 1); // +1 för att undvika 0
        
        // Sätt node position baserat på beräknad layout
        if (m_needsLayout) {
            int depth = m_nodeDepth.count(node.id) ? m_nodeDepth[node.id] : 0;
            int order = m_nodeOrder.count(node.id) ? m_nodeOrder[node.id] : 0;
            int nodesAtLevel = m_nodesPerLevel.count(depth) ? m_nodesPerLevel[depth] : 1;
            
            // Centrera noder horisontellt på varje nivå
            float totalWidth = (nodesAtLevel - 1) * HORIZONTAL_SPACING;
            float startX = -totalWidth / 2.0f;
            
            float x = startX + order * HORIZONTAL_SPACING;
            float y = depth * VERTICAL_SPACING;
            
            ed::SetNodePosition(nodeId, ImVec2(x, y));
        }
        
        ed::BeginNode(nodeId);
        
        // Node header
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.5f, 1.0f));
        ImGui::Text("[%d] %s", node.id, node.speaker.c_str());
        ImGui::PopStyleColor();
        
        ImGui::Separator();
        
        // Input pin (om inte start node)
        if (node.id != dialog->startNodeId) {
            ed::BeginPin(ed::PinId((node.id + 1) * 1000), ed::PinKind::Input);
            ImGui::Text("-> In");
            ed::EndPin();
        }
        
        // Node text (trunkerad)
        std::string displayText = node.text;
        if (displayText.length() > 50) {
            displayText = displayText.substr(0, 50) + "...";
        }
        ImGui::TextWrapped("%s", displayText.c_str());
        
        // Output pins för choices (bara om koppling finns)
        if (!node.choices.empty()) {
            bool hasValidChoice = false;
            for (const auto& choice : node.choices) {
                if (choice.nextNodeId >= 0) {
                    hasValidChoice = true;
                    break;
                }
            }
            
            if (hasValidChoice) {
                ImGui::Separator();
                ImGui::Text("Choices:");
                for (size_t c = 0; c < node.choices.size(); c++) {
                    auto& choice = node.choices[c];
                    // Bara visa output pin om choice har giltig koppling
                    if (choice.nextNodeId >= 0) {
                        ed::BeginPin(ed::PinId((node.id + 1) * 1000 + c + 1), ed::PinKind::Output);
                        std::string choiceText = choice.text;
                        if (choiceText.length() > 30) {
                            choiceText = choiceText.substr(0, 30) + "...";
                        }
                        ImGui::Text("-> %s", choiceText.c_str());
                        ed::EndPin();
                    }
                }
            }
        } else if (node.nextNodeId >= 0) {
            // Single output för next node
            ed::BeginPin(ed::PinId((node.id + 1) * 1000 + 1), ed::PinKind::Output);
            ImGui::Text("->");
            ed::EndPin();
        }
        
        ed::EndNode();
    }
    
    // Rendera länkar mellan noder
    renderLinks();
    
    // Navigera till content efter första layout
    if (m_needsLayout) {
        ed::NavigateToContent();
        m_needsLayout = false;
    }
    
    ed::End();
    ed::SetCurrentEditor(nullptr);
#endif
}

void DialogGraphPanel::calculateTreeLayout(DialogData* dialog) {
#ifdef HAS_IMGUI_NODE_EDITOR
    m_nodeDepth.clear();
    m_nodeOrder.clear();
    m_nodesPerLevel.clear();
    
    if (dialog->nodes.empty()) return;
    
    // Bygg adjacency list (node -> children)
    std::unordered_map<int, std::vector<int>> children;
    std::unordered_map<int, bool> hasParent;
    
    for (const auto& node : dialog->nodes) {
        hasParent[node.id] = false;
    }
    
    for (const auto& node : dialog->nodes) {
        // Från choices
        for (const auto& choice : node.choices) {
            if (choice.nextNodeId >= 0) {
                children[node.id].push_back(choice.nextNodeId);
                hasParent[choice.nextNodeId] = true;
            }
        }
        // Från nextNodeId (om inga choices)
        if (node.choices.empty() && node.nextNodeId >= 0) {
            children[node.id].push_back(node.nextNodeId);
            hasParent[node.nextNodeId] = true;
        }
    }
    
    // Hitta root nodes (start node eller noder utan föräldrar)
    std::vector<int> roots;
    
    // Prioritera startNodeId
    for (const auto& node : dialog->nodes) {
        if (node.id == dialog->startNodeId) {
            roots.push_back(node.id);
            break;
        }
    }
    
    // Om ingen startNode, använd noder utan föräldrar
    if (roots.empty()) {
        for (const auto& node : dialog->nodes) {
            if (!hasParent[node.id]) {
                roots.push_back(node.id);
            }
        }
    }
    
    // Om fortfarande tom, använd första noden
    if (roots.empty() && !dialog->nodes.empty()) {
        roots.push_back(dialog->nodes[0].id);
    }
    
    // BFS för att beräkna djup
    std::queue<int> queue;
    std::unordered_map<int, bool> visited;
    
    for (int root : roots) {
        queue.push(root);
        m_nodeDepth[root] = 0;
        visited[root] = true;
    }
    
    while (!queue.empty()) {
        int current = queue.front();
        queue.pop();
        
        int currentDepth = m_nodeDepth[current];
        
        for (int child : children[current]) {
            if (!visited[child]) {
                visited[child] = true;
                m_nodeDepth[child] = currentDepth + 1;
                queue.push(child);
            }
        }
    }
    
    // Hantera noder som inte nåddes (disconnected)
    for (const auto& node : dialog->nodes) {
        if (!visited[node.id]) {
            m_nodeDepth[node.id] = 0;
        }
    }
    
    // Räkna noder per nivå och tilldela order
    std::unordered_map<int, int> levelCounter;
    
    for (const auto& node : dialog->nodes) {
        int depth = m_nodeDepth[node.id];
        m_nodeOrder[node.id] = levelCounter[depth];
        levelCounter[depth]++;
    }
    
    m_nodesPerLevel = levelCounter;
#endif
}

void DialogGraphPanel::renderLinks() {
#ifdef HAS_IMGUI_NODE_EDITOR
    // Hitta vald dialog
    DialogData* dialog = nullptr;
    for (auto& d : m_context.dialogs) {
        if (d.id == m_context.selectedDialogId) {
            dialog = &d;
            break;
        }
    }
    
    if (!dialog) return;
    
    int linkId = 1;
    
    for (const auto& node : dialog->nodes) {
        // Länkar från choices
        for (size_t c = 0; c < node.choices.size(); c++) {
            const auto& choice = node.choices[c];
            if (choice.nextNodeId >= 0) {
                // Hitta target node
                for (const auto& targetNode : dialog->nodes) {
                    if (targetNode.id == choice.nextNodeId) {
                        ed::PinId outputPin((node.id + 1) * 1000 + c + 1);
                        ed::PinId inputPin((targetNode.id + 1) * 1000);
                        ed::Link(ed::LinkId(linkId++), outputPin, inputPin);
                        break;
                    }
                }
            }
        }
        
        // Länk från nextNodeId (om inga choices)
        if (node.choices.empty() && node.nextNodeId >= 0) {
            for (const auto& targetNode : dialog->nodes) {
                if (targetNode.id == node.nextNodeId) {
                    ed::PinId outputPin((node.id + 1) * 1000 + 1);
                    ed::PinId inputPin((targetNode.id + 1) * 1000);
                    ed::Link(ed::LinkId(linkId++), outputPin, inputPin);
                    break;
                }
            }
        }
    }
#endif
}
