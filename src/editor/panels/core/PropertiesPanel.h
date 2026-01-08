/**
 * @file PropertiesPanel.h
 * @brief Inspector-panel för valt objekt
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>
#include <memory>

class EditorContext;
class IPropertyEditor;
class RoomPropertyEditor;
class HotspotPropertyEditor;
class DialogPropertyEditor;
class QuestPropertyEditor;
class ItemPropertyEditor;
class NPCPropertyEditor;
class ActorPropertyEditor;
class SelectionManager;

/**
 * @brief Visar och editerar egenskaper för valt objekt
 */
class PropertiesPanel : public IEditorPanel {
public:
    explicit PropertiesPanel(EditorContext& context);
    ~PropertiesPanel();
    
    // Set selection manager for synchronization
    void setSelectionManager(SelectionManager* selectionManager);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
private:
    void renderBreadcrumb();
    void renderToolbar();
    IPropertyEditor* getCurrentEditor();
    void updateEditorSelection();
    
    EditorContext& m_context;
    SelectionManager* m_selectionManager = nullptr;
    std::string m_id = "properties";
    std::string m_title = "Properties";
    
    // Strategy pattern - en editor per typ
    std::unique_ptr<RoomPropertyEditor> m_roomEditor;
    std::unique_ptr<HotspotPropertyEditor> m_hotspotEditor;
    std::unique_ptr<DialogPropertyEditor> m_dialogEditor;
    std::unique_ptr<QuestPropertyEditor> m_questEditor;
    std::unique_ptr<ItemPropertyEditor> m_itemEditor;
    std::unique_ptr<NPCPropertyEditor> m_npcEditor;
    std::unique_ptr<ActorPropertyEditor> m_actorEditor;
};
