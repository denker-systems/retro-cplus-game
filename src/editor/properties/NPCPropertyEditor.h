/**
 * @file NPCPropertyEditor.h
 * @brief Property editor för NPC data
 */
#pragma once

#include "IPropertyEditor.h"
#include "engine/data/GameData.h"
#include <string>

class EditorContext;

/**
 * @brief Editor för NPC properties
 */
class NPCPropertyEditor : public IPropertyEditor {
public:
    explicit NPCPropertyEditor(EditorContext& context);
    
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
    // NPC-specifika metoder
    void setNPC(NPCData* npc);
    NPCData* getNPC() const { return m_npc; }
    
private:
    void renderBasicProperties();
    void renderAIProperties();
    void renderWaypoints();
    void renderSchedules();
    
    EditorContext& m_context;
    NPCData* m_npc = nullptr;
    NPCData m_originalData;
    bool m_isDirty = false;
};
