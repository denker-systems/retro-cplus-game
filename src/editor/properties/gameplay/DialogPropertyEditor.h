/**
 * @file DialogPropertyEditor.h
 * @brief Property editor för Dialog data
 */
#pragma once

#include "editor/properties/core/IPropertyEditor.h"
#include "engine/data/GameData.h"
#include <string>

class EditorContext;

/**
 * @brief Editor för Dialog properties
 */
class DialogPropertyEditor : public IPropertyEditor {
public:
    explicit DialogPropertyEditor(EditorContext& context);
    
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
    // Dialog-specifika metoder
    void setDialog(DialogData* dialog);
    DialogData* getDialog() const { return m_dialog; }
    
private:
    void renderBasicProperties();
    void renderNodesList();
    void renderSelectedNode();
    void renderNodeChoices();
    
    EditorContext& m_context;
    DialogData* m_dialog = nullptr;
    DialogData m_originalData;
    bool m_isDirty = false;
    int m_selectedNodeIndex = -1;
};
