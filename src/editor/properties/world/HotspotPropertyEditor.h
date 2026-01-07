/**
 * @file HotspotPropertyEditor.h
 * @brief Property editor för Hotspot data
 */
#pragma once

#include "editor/properties/core/IPropertyEditor.h"
#include "engine/data/GameData.h"
#include <string>

class EditorContext;

/**
 * @brief Editor för Hotspot properties
 */
class HotspotPropertyEditor : public IPropertyEditor {
public:
    explicit HotspotPropertyEditor(EditorContext& context);
    
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
    // Hotspot-specifika metoder
    void setHotspot(HotspotData* hotspot, RoomData* parentRoom);
    HotspotData* getHotspot() const { return m_hotspot; }
    
private:
    void renderBasicProperties();
    void renderPositionProperties();
    void renderTypeSpecificProperties();
    void renderFunnyFails();
    
    EditorContext& m_context;
    HotspotData* m_hotspot = nullptr;
    RoomData* m_parentRoom = nullptr;
    HotspotData m_originalData;
    bool m_isDirty = false;
};
