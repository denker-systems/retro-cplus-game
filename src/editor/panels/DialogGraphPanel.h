/**
 * @file DialogGraphPanel.h
 * @brief Dialog Graph Editor Panel med imgui-node-editor
 */
#pragma once

#include "editor/IEditorPanel.h"
#include "editor/EditorContext.h"
#include <string>

class DialogGraphPanel : public IEditorPanel {
public:
    explicit DialogGraphPanel(EditorContext& context);
    ~DialogGraphPanel();
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
private:
    void renderNodeEditor();
    void renderDialogNode(DialogNodeData& node, int nodeIndex);
    void renderLinks();
    
    EditorContext& m_context;
    std::string m_id = "dialog_graph";
    std::string m_title = "Dialog Graph";
    
    // Node editor context (opaque pointer)
    void* m_editorContext = nullptr;
    
    // Selection state
    int m_selectedNodeIndex = -1;
};
