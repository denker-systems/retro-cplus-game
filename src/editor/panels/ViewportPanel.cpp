/**
 * @file ViewportPanel.cpp
 * @brief Viewport panel implementation
 */
#include "ViewportPanel.h"
#include "editor/EditorContext.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/data/DataLoader.h"
#include "engine/components/SpriteComponent.h"
#include <SDL_image.h>
#include <algorithm>

// Note: Node.h and Node2D.h have been removed - using Actor system only

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
        renderBreadcrumbs();
        ImGui::Separator();
        
        // Render different views based on breadcrumb level
        if (!m_level) {
            // World level - show all levels
            renderWorldView();
        } else if (!m_scene) {
            // Level level - show all scenes
            renderLevelView();
        } else {
            // Scene level - show scene content
            renderToolbar();
            renderSceneView();
        }
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

// DEPRECATED: renderSceneNode() removed - Node system has been replaced by Actor system
// Use renderSceneActors() instead

void ViewportPanel::renderBreadcrumbs() {
#ifdef HAS_IMGUI
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.4f, 0.5f, 1.0f));
    
    // World level
    if (m_world) {
        if (ImGui::Button("🌍 World")) {
            // Navigate to world view (show all levels)
            m_level = nullptr;
            m_scene = nullptr;
        }
        
        // Level level
        if (m_level) {
            ImGui::SameLine();
            ImGui::TextDisabled(">");
            ImGui::SameLine();
            
            if (ImGui::Button(("📁 " + m_level->getName()).c_str())) {
                // Navigate to level view (show all scenes)
                m_scene = nullptr;
            }
            
            // Scene level
            if (m_scene) {
                ImGui::SameLine();
                ImGui::TextDisabled(">");
                ImGui::SameLine();
                
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.3f, 1.0f));
                ImGui::Button(("🎬 " + m_scene->getName()).c_str());
                ImGui::PopStyleColor();
            }
        }
    } else {
        ImGui::TextDisabled("No World loaded");
    }
    
    ImGui::PopStyleColor(2);
#endif
}

void ViewportPanel::setLevel(engine::Level* level) {
    m_level = level;
    // Don't auto-select scene - let user see Level View first
    m_scene = nullptr;
}

void ViewportPanel::setScene(engine::Scene* scene) {
    m_scene = scene;
}

void ViewportPanel::renderWorldView() {
#ifdef HAS_IMGUI
    if (!m_world) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No World loaded");
        return;
    }
    
    ImGui::Text("Levels in World:");
    ImGui::Separator();
    
    const auto& levels = m_world->getLevels();
    
    if (levels.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "World.getLevels() not yet implemented");
        if (ImGui::Button("+ Create New Level")) {
            // TODO: Create level dialog
        }
        return;
    }
    
    // Grid layout for level cards
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columns = std::max(1, (int)(panelWidth / 250.0f));
    
    ImGui::Columns(columns, nullptr, false);
    
    for (const auto& level : levels) {
        // Level card
        ImVec2 cursorPos = ImGui::GetCursorScreenPos();
        ImVec2 cardSize(230, 120);
        
        // Background
        ImU32 bgColor = IM_COL32(50, 60, 70, 255);
        ImGui::GetWindowDrawList()->AddRectFilled(
            cursorPos,
            ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
            bgColor,
            4.0f);
        
        // Icon area
        ImGui::GetWindowDrawList()->AddRectFilled(
            ImVec2(cursorPos.x + 5, cursorPos.y + 5),
            ImVec2(cursorPos.x + cardSize.x - 5, cursorPos.y + 60),
            IM_COL32(40, 50, 60, 255));
        
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + cardSize.x/2 - 20, cursorPos.y + 25));
        ImGui::Text("📁");
        
        // Level name
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 10, cursorPos.y + 70));
        ImGui::Text("%s", level->getName().c_str());
        
        // Scene count
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 10, cursorPos.y + 90));
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), 
            "%zu scenes", level->getScenes().size());
        
        // Make clickable
        ImGui::SetCursorScreenPos(cursorPos);
        if (ImGui::InvisibleButton(level->getId().c_str(), cardSize)) {
            setLevel(const_cast<engine::Level*>(level.get()));
        }
        
        // Hover effect
        if (ImGui::IsItemHovered()) {
            ImGui::GetWindowDrawList()->AddRect(
                cursorPos,
                ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
                IM_COL32(100, 150, 200, 255),
                4.0f,
                0,
                2.0f);
        }
        
        ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + cardSize.y + 10));
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
    
    ImGui::Separator();
    if (ImGui::Button("+ Create New Level")) {
        // TODO: Create level dialog
    }
#endif
}

void ViewportPanel::renderLevelView() {
#ifdef HAS_IMGUI
    if (!m_level) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Level selected");
        return;
    }
    
    ImGui::Text("Level: %s", m_level->getName().c_str());
    ImGui::Text("ID: %s", m_level->getId().c_str());
    ImGui::Separator();
    
    // Toggle between grid and list view
    static bool gridView = true;
    if (ImGui::Button(gridView ? "Grid View" : "List View")) {
        gridView = !gridView;
    }
    
    ImGui::Separator();
    
    const auto& scenes = m_level->getScenes();
    
    if (scenes.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No scenes in this level");
        if (ImGui::Button("+ Create New Scene")) {
            // TODO: Create scene dialog
        }
        return;
    }
    
    if (gridView) {
        // Grid layout
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columns = std::max(1, (int)(panelWidth / 200.0f));
        
        ImGui::Columns(columns, nullptr, false);
        
        for (const auto& scene : scenes) {
            // Scene card
            ImVec2 cursorPos = ImGui::GetCursorScreenPos();
            ImVec2 cardSize(180, 150);
            
            // Background
            ImU32 bgColor = IM_COL32(50, 60, 70, 255);
            ImGui::GetWindowDrawList()->AddRectFilled(
                cursorPos,
                ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
                bgColor,
                4.0f);
            
            // Preview area (placeholder)
            ImGui::GetWindowDrawList()->AddRectFilled(
                ImVec2(cursorPos.x + 5, cursorPos.y + 5),
                ImVec2(cursorPos.x + cardSize.x - 5, cursorPos.y + 105),
                IM_COL32(30, 35, 40, 255));
            
            ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + cardSize.x/2 - 20, cursorPos.y + 50));
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Preview");
            
            // Scene name
            ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 5, cursorPos.y + 110));
            ImGui::Text("%s", scene->getName().c_str());
            
            // Actor count
            ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 5, cursorPos.y + 125));
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), 
                "%zu actors", scene->getActors().size());
            
            // Make clickable
            ImGui::SetCursorScreenPos(cursorPos);
            if (ImGui::InvisibleButton(scene->getName().c_str(), cardSize)) {
                setScene(scene.get());
            }
            
            // Hover effect
            if (ImGui::IsItemHovered()) {
                ImGui::GetWindowDrawList()->AddRect(
                    cursorPos,
                    ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
                    IM_COL32(100, 200, 100, 255),
                    4.0f,
                    0,
                    2.0f);
            }
            
            // Double-click to open
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
                setScene(scene.get());
            }
            
            ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + cardSize.y + 10));
            ImGui::NextColumn();
        }
        
        ImGui::Columns(1);
    } else {
        // List view
        for (const auto& scene : scenes) {
            if (ImGui::Selectable(scene->getName().c_str())) {
                setScene(scene.get());
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), 
                "(%zu actors)", scene->getActors().size());
        }
    }
    
    ImGui::Separator();
    if (ImGui::Button("+ Create New Scene")) {
        // TODO: Create scene dialog
    }
#endif
}

void ViewportPanel::renderSceneView() {
#ifdef HAS_IMGUI
    if (!m_scene) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Scene selected");
        return;
    }
    
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    
    // Use fixed room size (640x400) instead of contentSize
    ImVec2 roomSize(640.0f * m_zoom, 400.0f * m_zoom);
    
    // Center the room in the viewport if it's smaller
    ImVec2 renderPos = cursorPos;
    if (roomSize.x < contentSize.x) {
        renderPos.x += (contentSize.x - roomSize.x) / 2.0f;
    }
    if (roomSize.y < contentSize.y) {
        renderPos.y += (contentSize.y - roomSize.y) / 2.0f;
    }
    
    // Background (room boundaries)
    ImGui::GetWindowDrawList()->AddRectFilled(
        renderPos,
        ImVec2(renderPos.x + roomSize.x, renderPos.y + roomSize.y),
        IM_COL32(40, 40, 50, 255));
    
    // Draw room border
    ImGui::GetWindowDrawList()->AddRect(
        renderPos,
        ImVec2(renderPos.x + roomSize.x, renderPos.y + roomSize.y),
        IM_COL32(100, 100, 100, 255), 0, 0, 1.0f);
    
    // Render actors directly from Scene
    if (m_scene) {
        renderSceneActors(ImGui::GetWindowDrawList(), 
                         ImVec2(renderPos.x + m_panX, renderPos.y + m_panY));
    }
    
    // Make area interactive (use full content size for interaction)
    ImGui::SetCursorScreenPos(cursorPos);
    ImGui::InvisibleButton("scene_canvas", contentSize);
    
    // Handle mouse interactions
    if (ImGui::IsItemHovered()) {
        ImVec2 mousePos = ImGui::GetMousePos();
        
        // Left click - select/drag nodes
        if (ImGui::IsMouseClicked(0)) {
            // TODO: Implement node selection and drag start
        }
        
        // Middle mouse drag - pan view
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            m_panX += delta.x;
            m_panY += delta.y;
        }
        
        // Mouse wheel - zoom
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            m_zoom = std::max(0.25f, std::min(4.0f, m_zoom + wheel * 0.1f));
        }
    }
    
    // Show info overlay
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 10, cursorPos.y + 10));
    ImGui::BeginGroup();
    ImGui::Text("Scene: %s", m_scene->getName().c_str());
    ImGui::Text("Actors: %zu", m_scene->getActors().size());
    ImGui::Text("Zoom: %.0f%% | Pan: %.0f, %.0f", m_zoom * 100, m_panX, m_panY);
    ImGui::Text("Tip: Middle-mouse to pan, scroll to zoom");
    ImGui::EndGroup();
#endif
}

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
                                   float previewW, float previewH, float roomX, float roomY, const RoomData* room) {
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
    RoomData* editRoom = nullptr;
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

void ViewportPanel::renderSceneActors(ImDrawList* drawList, ImVec2 offset) {
#ifdef HAS_IMGUI
    if (!m_scene || !drawList) return;
    
    const auto& actors = m_scene->getActors();
    
    for (const auto& actor : actors) {
        if (!actor || !actor->isActive()) continue;
        
        // Get actor transform
        engine::Vec2 pos = actor->getPosition();
        float worldX = offset.x + pos.x * m_zoom;
        float worldY = offset.y + pos.y * m_zoom;
        
        // Try to get SpriteComponent
        auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
        if (spriteComp) {
            SDL_Texture* tex = spriteComp->getTexture();
            if (tex) {
                int w = spriteComp->getWidth();
                int h = spriteComp->getHeight();
                
                // Special handling for background images
                if (actor->getName() == "Background") {
                    // Calculate aspect ratio
                    float aspectRatio = (float)w / (float)h;
                    
                    // Fit to room size (640x400) while preserving aspect ratio
                    float roomWidth = 640.0f * m_zoom;
                    float roomHeight = 400.0f * m_zoom;
                    
                    float scaleX = roomWidth / w;
                    float scaleY = roomHeight / h;
                    float scale = std::min(scaleX, scaleY);
                    
                    float scaledW = w * scale;
                    float scaledH = h * scale;
                    
                    // Center the background
                    float offsetX = (roomWidth - scaledW) / 2.0f;
                    float offsetY = (roomHeight - scaledH) / 2.0f;
                    
                    // Render centered background
                    drawList->AddImage(
                        (ImTextureID)(intptr_t)tex,
                        ImVec2(worldX + offsetX, worldY + offsetY),
                        ImVec2(worldX + offsetX + scaledW, worldY + offsetY + scaledH)
                    );
                } else {
                    // Normal rendering for other sprites
                    drawList->AddImage(
                        (ImTextureID)(intptr_t)tex,
                        ImVec2(worldX, worldY),
                        ImVec2(worldX + w * m_zoom, worldY + h * m_zoom)
                    );
                }
            }
        }
        
        // Debug visualization for actors without sprites
        if (!spriteComp) {
            std::string name = actor->getName();
            
            // Different colors for different actor types
            ImU32 color = IM_COL32(200, 200, 200, 255);
            
            if (name == "Player") {
                color = IM_COL32(255, 100, 100, 255);
                // Draw player as a filled rectangle with border
                drawList->AddRectFilled(
                    ImVec2(worldX - 16 * m_zoom, worldY - 24 * m_zoom),
                    ImVec2(worldX + 16 * m_zoom, worldY + 24 * m_zoom),
                    color
                );
                drawList->AddRect(
                    ImVec2(worldX - 16 * m_zoom, worldY - 24 * m_zoom),
                    ImVec2(worldX + 16 * m_zoom, worldY + 24 * m_zoom),
                    IM_COL32(255, 255, 255, 255), 0, 0, 2.0f
                );
                drawList->AddText(ImVec2(worldX - 20, worldY + 30 * m_zoom), 
                                IM_COL32(255, 255, 255, 255), 
                                "Player");
            }
            else if (name == "PlayerSpawn") {
                color = IM_COL32(255, 0, 255, 255);
                drawList->AddCircleFilled(ImVec2(worldX, worldY), 8.0f * m_zoom, color);
                drawList->AddText(ImVec2(worldX + 10, worldY - 8), color, "Spawn");
            }
            else if (name == "WalkArea") {
                color = IM_COL32(100, 255, 100, 200);
                // Draw box around typical walk area size
                drawList->AddRect(
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + 640 * m_zoom, worldY + 400 * m_zoom),
                    color, 0, 0, 2.0f
                );
            }
            else {
                // Interactive actors (hotspots)
                color = IM_COL32(100, 255, 255, 255);
                drawList->AddRect(
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + 64 * m_zoom, worldY + 64 * m_zoom),
                    color, 0, 0, 2.0f
                );
                drawList->AddText(ImVec2(worldX + 5, worldY + 5), 
                                IM_COL32(255, 255, 255, 255), 
                                name.c_str());
            }
        }
    }
#endif
}
