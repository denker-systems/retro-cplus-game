/**
 * @file DialogGraphPanel.cpp
 * @brief Implementation av Dialog Graph Editor Panel
 */
#include "DialogGraphPanel.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

#ifdef HAS_IMGUI_NODE_EDITOR
#include <imgui-node-editor/imgui_node_editor.h>
namespace ed = ax::NodeEditor;
#endif

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
    
    ed::SetCurrentEditor(static_cast<ed::EditorContext*>(m_editorContext));
    ed::Begin("Dialog Graph");
    
    // Rendera alla noder
    for (size_t i = 0; i < dialog->nodes.size(); i++) {
        auto& node = dialog->nodes[i];
        
        // Node ID måste vara unikt
        ed::NodeId nodeId = ed::NodeId(node.id + 1); // +1 för att undvika 0
        
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
        
        // Output pins för choices
        if (!node.choices.empty()) {
            ImGui::Separator();
            ImGui::Text("Choices:");
            for (size_t c = 0; c < node.choices.size(); c++) {
                auto& choice = node.choices[c];
                ed::BeginPin(ed::PinId((node.id + 1) * 1000 + c + 1), ed::PinKind::Output);
                ImGui::Text("Out -> %s", choice.text.c_str());
                ed::EndPin();
            }
        } else if (node.nextNodeId >= 0) {
            // Single output för next node
            ed::BeginPin(ed::PinId((node.id + 1) * 1000 + 1), ed::PinKind::Output);
            ImGui::Text("Out ->");
            ed::EndPin();
        }
        
        ed::EndNode();
    }
    
    // Rendera länkar mellan noder
    renderLinks();
    
    ed::End();
    ed::SetCurrentEditor(nullptr);
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
