/**
 * @file PropertiesPanel.h
 * @brief Inspector-panel för valt objekt
 */
#pragma once

#include "../IEditorPanel.h"
#include <string>

class EditorContext;

/**
 * @brief Visar och editerar egenskaper för valt objekt
 */
class PropertiesPanel : public IEditorPanel {
public:
    explicit PropertiesPanel(EditorContext& context);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
private:
    void renderRoomProperties();
    void renderDialogProperties();
    void renderQuestProperties();
    void renderItemProperties();
    void renderHotspotProperties();
    
    EditorContext& m_context;
    std::string m_id = "properties";
    std::string m_title = "Properties";
};
