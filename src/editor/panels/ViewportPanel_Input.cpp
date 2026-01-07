/**
 * @file ViewportPanel_Input.cpp
 * @brief ViewportPanel legacy room input handling and preview
 */
#include "ViewportPanel.h"
#include "editor/EditorContext.h"
#include "engine/data/DataLoader.h"
#include <algorithm>
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

// Helper function to check walk area corners (avoids MSVC bug with member access)
static int checkWalkAreaCorner(float roomX, float roomY, const WalkAreaData& wa, float radius) {
    float cx[4] = { (float)wa.minX, (float)wa.maxX, (float)wa.maxX, (float)wa.minX };
    float cy[4] = { (float)wa.minY, (float)wa.minY, (float)wa.maxY, (float)wa.maxY };
    
    for (int i = 0; i < 4; i++) {
        float dx = roomX - cx[i];
        float dy = roomY - cy[i];
        if (std::sqrt(dx*dx + dy*dy) < radius) {
            return i;
        }
    }
    return -1;
}

void ViewportPanel::handleMouseDown(float mouseX, float mouseY, float previewX, float previewY,
                                   float previewW, float previewH, float roomX, float roomY, const SceneData* room) {
#ifdef HAS_IMGUI
    if (!room || m_isDragging) return;
    
    // Check spawn
    float spawnDist = std::sqrt(std::pow(roomX - room->playerSpawnX, 2) + 
                               std::pow(roomY - room->playerSpawnY, 2));
    if (spawnDist < 15.0f) {
        m_isDragging = true;
        m_draggingSpawn = true;
        m_dragOffsetX = roomX - room->playerSpawnX;
        m_dragOffsetY = roomY - room->playerSpawnY;
        return;
    }
    
    // Check hotspots
    for (int i = room->hotspots.size() - 1; i >= 0; i--) {
        const auto& hs = room->hotspots[i];
        if (roomX >= hs.x && roomX <= hs.x + hs.w &&
            roomY >= hs.y && roomY <= hs.y + hs.h) {
            m_isDragging = true;
            m_draggedHotspotIndex = i;
            m_dragOffsetX = roomX - hs.x;
            m_dragOffsetY = roomY - hs.y;
            m_context.selectedHotspotIndex = i;
            return;
        }
    }
    
    // Check walk area corners (using static helper to avoid MSVC bug)
    int cornerHit = checkWalkAreaCorner(roomX, roomY, room->walkArea, 10.0f);
    if (cornerHit >= 0) {
        m_isDragging = true;
        m_draggingWalkArea = true;
        m_walkAreaCorner = cornerHit;
        return;
    }
#endif
}

void ViewportPanel::handleMouseDrag(float roomX, float roomY) {
#ifdef HAS_IMGUI
    if (!m_isDragging) return;
    
    // Get mutable room data
    auto& rooms = DataLoader::instance().getRooms();
    SceneData* editRoom = nullptr;
    for (auto& r : rooms) {
        if (r.id == m_context.selectedRoomId) {
            editRoom = &r;
            break;
        }
    }
    
    if (!editRoom) return;
    
    // Update based on what's being dragged
    if (m_draggingSpawn) {
        editRoom->playerSpawnX = roomX - m_dragOffsetX;
        editRoom->playerSpawnY = roomY - m_dragOffsetY;
    }
    
    if (m_draggedHotspotIndex >= 0 && m_draggedHotspotIndex < (int)editRoom->hotspots.size()) {
        auto& hs = editRoom->hotspots[m_draggedHotspotIndex];
        hs.x = static_cast<int>(roomX - m_dragOffsetX);
        hs.y = static_cast<int>(roomY - m_dragOffsetY);
    }
    
    if (m_draggingWalkArea) {
        auto& wa = editRoom->walkArea;
        int x = static_cast<int>(roomX);
        int y = static_cast<int>(roomY);
        int corner = m_walkAreaCorner;
        
        // Update based on corner index
        if (corner == 0) { wa.minX = x; wa.minY = y; }
        else if (corner == 1) { wa.maxX = x; wa.minY = y; }
        else if (corner == 2) { wa.maxX = x; wa.maxY = y; }
        else if (corner == 3) { wa.minX = x; wa.maxY = y; }
    }
#endif
}

void ViewportPanel::renderRoomPreview() {
#ifdef HAS_IMGUI
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    
    if (m_context.selectedRoomId.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "Select a room from the Hierarchy panel");
        return;
    }
    
    // Find room data (mutable for editing)
    auto& rooms = DataLoader::instance().getRooms();
    RoomData* room = nullptr;
    for (auto& r : rooms) {
        if (r.id == m_context.selectedRoomId) {
            room = &r;
            break;
        }
    }
    
    if (!room) {
        ImGui::Text("Room not found: %s", m_context.selectedRoomId.c_str());
        return;
    }
    
    ImGui::Text("Room: %s (%s)", room->name.c_str(), room->id.c_str());
    
    // Draw preview area
    ImVec2 previewSize(640 * m_zoom, 400 * m_zoom);
    if (previewSize.x > contentSize.x - 20) {
        float scale = (contentSize.x - 20) / previewSize.x;
        previewSize.x *= scale;
        previewSize.y *= scale;
    }
    
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    
    // Background
    ImGui::GetWindowDrawList()->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + previewSize.x, cursorPos.y + previewSize.y),
        IM_COL32(40, 40, 50, 255));
    
    // Room texture (if loaded)
    if (m_roomTexture) {
        // TODO: Convert SDL_Texture to ImGui texture ID
        ImGui::GetWindowDrawList()->AddRect(
            cursorPos,
            ImVec2(cursorPos.x + previewSize.x, cursorPos.y + previewSize.y),
            IM_COL32(100, 100, 100, 255));
        
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 10, cursorPos.y + 10));
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), 
            "[Background: %s]", room->background.c_str());
    }
    
    float scaleX = previewSize.x / 640.0f;
    float scaleY = previewSize.y / 400.0f;
    
    // Draw walk area
    if (m_showWalkArea) {
        ImVec2 waMin(cursorPos.x + room->walkArea.minX * scaleX, 
                     cursorPos.y + room->walkArea.minY * scaleY);
        ImVec2 waMax(cursorPos.x + room->walkArea.maxX * scaleX, 
                     cursorPos.y + room->walkArea.maxY * scaleY);
        ImGui::GetWindowDrawList()->AddRect(waMin, waMax, IM_COL32(0, 255, 255, 128), 0, 0, 2);
    }
    
    // Draw hotspots
    if (m_showHotspots) {
        for (size_t i = 0; i < room->hotspots.size(); i++) {
            const auto& hs = room->hotspots[i];
            ImVec2 hsMin(cursorPos.x + hs.x * scaleX, cursorPos.y + hs.y * scaleY);
            ImVec2 hsMax(cursorPos.x + (hs.x + hs.w) * scaleX, 
                        cursorPos.y + (hs.y + hs.h) * scaleY);
            
            ImU32 color = (m_context.selectedHotspotIndex == (int)i) 
                ? IM_COL32(255, 255, 0, 200) 
                : IM_COL32(255, 100, 100, 150);
            
            ImGui::GetWindowDrawList()->AddRectFilled(hsMin, hsMax, color);
            ImGui::GetWindowDrawList()->AddRect(hsMin, hsMax, IM_COL32(255, 255, 255, 255));
            
            // Label
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(hsMin.x + 2, hsMin.y + 2),
                IM_COL32(255, 255, 255, 255),
                hs.name.c_str());
        }
    }
    
    // Draw player spawn
    float spawnX = cursorPos.x + room->playerSpawnX * scaleX;
    float spawnY = cursorPos.y + room->playerSpawnY * scaleY;
    ImGui::GetWindowDrawList()->AddCircleFilled(
        ImVec2(spawnX, spawnY), 8, IM_COL32(255, 0, 255, 200));
    ImGui::GetWindowDrawList()->AddText(
        ImVec2(spawnX + 10, spawnY - 8),
        IM_COL32(255, 0, 255, 255), "Spawn");
    
    // Make preview area interactive
    ImGui::SetCursorScreenPos(cursorPos);
    ImGui::InvisibleButton("viewport_canvas", previewSize);
    
    // Calculate room coordinates (reuse scaleX/scaleY from above)
    ImVec2 mousePos = ImGui::GetMousePos();
    float roomX = (mousePos.x - cursorPos.x) / scaleX;
    float roomY = (mousePos.y - cursorPos.y) / scaleY;
    roomX = std::max(0.0f, std::min(640.0f, roomX));
    roomY = std::max(0.0f, std::min(400.0f, roomY));
    
    // Handle mouse down
    if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0)) {
        handleMouseDown(mousePos.x, mousePos.y, cursorPos.x, cursorPos.y, 
                       previewSize.x, previewSize.y, roomX, roomY, room);
    }
    
    // Handle drag
    if (ImGui::IsItemActive() && ImGui::IsMouseDown(0)) {
        handleMouseDrag(roomX, roomY);
    }
    
    // Release drag on mouse up
    if (!ImGui::IsMouseDown(0)) {
        if (m_isDragging) {
            m_context.markDirty();
        }
        m_isDragging = false;
        m_draggedHotspotIndex = -1;
        m_draggingSpawn = false;
        m_draggingWalkArea = false;
    }
    
    // Render scene nodes if we have a scene
    if (m_scene) {
        ImVec2 sceneOffset = cursorPos;  // Start from preview area
        
        // TODO: Render actors in scene
        // renderSceneActors(m_scene, ImGui::GetWindowDrawList(), sceneOffset);
    }
#endif
}
