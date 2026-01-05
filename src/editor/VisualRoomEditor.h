/**
 * @file VisualRoomEditor.h
 * @brief Visuell editor för rum-redigering
 */
#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include "../data/GameData.h"

/**
 * @brief Visuell editor för att redigera rum, hotspots, walk areas och player spawn
 */
class VisualRoomEditor {
public:
    VisualRoomEditor();
    ~VisualRoomEditor();
    
    /** @brief Rendera visuell editor */
    void render(SDL_Renderer* renderer, const RoomData& roomData, SDL_Texture* previewTexture);
    
    /** @brief Hantera mus-klick */
    void handleMouseClick(int x, int y, RoomData& roomData);
    
    /** @brief Hantera mus-rörelse (drag) */
    void handleMouseMotion(int mouseX, int mouseY, RoomData& roomData);
    
    /** @brief Hantera musknapp upp */
    void handleMouseUp();
    
    /** @brief Kolla om vi redigerar något */
    bool isEditing() const { return m_draggingHotspot || m_resizingHotspot || m_editingWalkArea || m_editingPlayerSpawn; }
    
    /** @brief Hämta vald hotspot index */
    int getSelectedHotspot() const { return m_selectedHotspot; }
    
    /** @brief Sätt vald hotspot */
    void setSelectedHotspot(int index) { m_selectedHotspot = index; }
    
    /** @brief Justera depth scale */
    void adjustDepthScale(bool increase, bool top, RoomData& roomData);
    
    /** @brief Ta bort vald hotspot */
    bool deleteSelectedHotspot(RoomData& roomData);

private:
    // Selection state
    int m_selectedHotspot = -1;
    
    // Drag state
    bool m_draggingHotspot = false;
    bool m_resizingHotspot = false;
    int m_resizeHandle = -1;  // 0-3 = hörn (TL, TR, BL, BR)
    int m_dragStartX = 0;
    int m_dragStartY = 0;
    int m_hotspotOrigX = 0;
    int m_hotspotOrigY = 0;
    int m_hotspotOrigW = 0;
    int m_hotspotOrigH = 0;
    
    // Walk area editing
    bool m_editingWalkArea = false;
    int m_walkAreaHandle = -1;  // 0-3 = sidor (top, bottom, left, right)
    
    // Player spawn editing
    bool m_editingPlayerSpawn = false;
    
    // Helper methods
    void renderHotspots(SDL_Renderer* renderer, const std::vector<HotspotData>& hotspots, float scaleX, float scaleY);
    void renderWalkArea(SDL_Renderer* renderer, const WalkAreaData& walkArea, float scaleX, float scaleY);
    void renderPlayerSpawn(SDL_Renderer* renderer, float spawnX, float spawnY, float scaleX, float scaleY);
};
