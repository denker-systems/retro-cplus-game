/**
 * @file RoomPropertyEditor.h
 * @brief Property editor för Room data
 */
#pragma once

#include "IPropertyEditor.h"
#include "engine/data/GameData.h"
#include <string>

class EditorContext;

/**
 * @brief Editor för Room properties
 */
class RoomPropertyEditor : public IPropertyEditor {
public:
    explicit RoomPropertyEditor(EditorContext& context);
    
    void render() override;
    bool validate(std::string& outError) const override;
    bool isDirty() const override;
    void revert() override;
    void apply() override;
    
    // Room-specifika metoder
    void setRoom(RoomData* room);
    RoomData* getRoom() const { return m_room; }
    
private:
    void renderBasicProperties();
    void renderSpawnProperties();
    void renderWalkAreaProperties();
    void renderHotspotsList();
    void renderLayersList();
    
    EditorContext& m_context;
    RoomData* m_room = nullptr;
    RoomData m_originalData;
    bool m_isDirty = false;
};
