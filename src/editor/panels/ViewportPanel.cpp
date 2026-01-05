/**
 * @file ViewportPanel.cpp
 * @brief Viewport panel implementation
 */
#include "ViewportPanel.h"
#include "../EditorContext.h"
#include "../../data/DataLoader.h"
#include <SDL_image.h>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

ViewportPanel::ViewportPanel(EditorContext& context)
    : m_context(context) {
}

ViewportPanel::~ViewportPanel() {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
    }
}

void ViewportPanel::update(float deltaTime) {
    // Load room preview if selection changed
    if (m_context.selectedRoomId != m_loadedRoomId) {
        loadRoomPreview();
    }
}

void ViewportPanel::loadRoomPreview() {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
        m_roomTexture = nullptr;
    }
    
    m_loadedRoomId = m_context.selectedRoomId;
    
    if (m_loadedRoomId.empty() || !m_renderer) return;
    
    // Find room data
    const auto& rooms = DataLoader::instance().getRooms();
    const RoomData* room = nullptr;
    for (const auto& r : rooms) {
        if (r.id == m_loadedRoomId) {
            room = &r;
            break;
        }
    }
    
    if (!room) return;
    
    // Load background texture
    std::string path = "assets/backgrounds/" + room->background;
    m_roomTexture = IMG_LoadTexture(m_renderer, path.c_str());
}

void ViewportPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        renderToolbar();
        ImGui::Separator();
        renderRoomPreview();
    }
    ImGui::End();
#endif
}

void ViewportPanel::renderToolbar() {
#ifdef HAS_IMGUI
    // Zoom controls
    ImGui::Text("Zoom: %.0f%%", m_zoom * 100);
    ImGui::SameLine();
    if (ImGui::Button("-")) m_zoom = std::max(0.25f, m_zoom - 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("+")) m_zoom = std::min(4.0f, m_zoom + 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("100%")) { m_zoom = 1.0f; m_panX = 0; m_panY = 0; }
    
    ImGui::SameLine();
    ImGui::Spacing();
    ImGui::SameLine();
    
    // View toggles
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Hotspots", &m_showHotspots);
    ImGui::SameLine();
    ImGui::Checkbox("Walk Area", &m_showWalkArea);
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
    
    // Find room data
    const auto& rooms = DataLoader::instance().getRooms();
    const RoomData* room = nullptr;
    for (const auto& r : rooms) {
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
    
    // Reserve space for preview
    ImGui::Dummy(previewSize);
#endif
}
