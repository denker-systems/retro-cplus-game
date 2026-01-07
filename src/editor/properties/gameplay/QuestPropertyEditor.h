/**
 * @file QuestPropertyEditor.h
 * @brief Property editor för Quest data
 */
#pragma once

#include "editor/properties/core/IPropertyEditor.h"
#include "engine/data/GameData.h"
#include <string>

class EditorContext;

/**
 * @brief Editor för Quest properties
 */
class QuestPropertyEditor : public IPropertyEditor {
public:
    explicit QuestPropertyEditor(EditorContext& context);
    
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
    // Quest-specifika metoder
    void setQuest(QuestData* quest);
    QuestData* getQuest() const { return m_quest; }
    
private:
    void renderBasicProperties();
    void renderObjectivesList();
    void renderSelectedObjective();
    
    EditorContext& m_context;
    QuestData* m_quest = nullptr;
    QuestData m_originalData;
    bool m_isDirty = false;
    int m_selectedObjectiveIndex = -1;
};
