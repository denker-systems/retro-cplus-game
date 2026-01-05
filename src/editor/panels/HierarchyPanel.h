/**
 * @file HierarchyPanel.h
 * @brief Hierarki-panel för rum, dialoger, quests, items
 */
#pragma once

#include "../IEditorPanel.h"
#include <string>
#include <functional>

class EditorContext;

/**
 * @brief Visar hierarkisk lista över alla spel-objekt
 */
class HierarchyPanel : public IEditorPanel {
public:
    explicit HierarchyPanel(EditorContext& context);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
    // Callbacks för selection
    using SelectionCallback = std::function<void(const std::string&)>;
    void setOnRoomSelected(SelectionCallback cb) { m_onRoomSelected = cb; }
    void setOnDialogSelected(SelectionCallback cb) { m_onDialogSelected = cb; }
    void setOnQuestSelected(SelectionCallback cb) { m_onQuestSelected = cb; }
    void setOnItemSelected(SelectionCallback cb) { m_onItemSelected = cb; }
    
private:
    EditorContext& m_context;
    std::string m_id = "hierarchy";
    std::string m_title = "Hierarchy";
    
    SelectionCallback m_onRoomSelected;
    SelectionCallback m_onDialogSelected;
    SelectionCallback m_onQuestSelected;
    SelectionCallback m_onItemSelected;
};
