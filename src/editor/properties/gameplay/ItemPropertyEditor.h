/**
 * @file ItemPropertyEditor.h
 * @brief Property editor för Item data
 */
#pragma once

#include "editor/properties/core/IPropertyEditor.h"
#include "engine/data/GameData.h"
#include <string>

class EditorContext;

/**
 * @brief Editor för Item properties
 */
class ItemPropertyEditor : public IPropertyEditor {
public:
    explicit ItemPropertyEditor(EditorContext& context);
    
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
    // Item-specifika metoder
    void setItem(ItemData* item);
    ItemData* getItem() const { return m_item; }
    
private:
    void renderBasicProperties();
    void renderCombinations();
    
    EditorContext& m_context;
    ItemData* m_item = nullptr;
    ItemData m_originalData;
    bool m_isDirty = false;
};
