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
#include "engine/core/ActorObjectExtended.h"
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
    
    // View mode toggle: Spatial, Grid
    const char* viewModes[] = { "Spatial View", "Grid View" };
    if (ImGui::Button(viewModes[m_worldViewMode])) {
        m_worldViewMode = (m_worldViewMode + 1) % 2;
    }
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Click to toggle)");
    
    ImGui::Separator();
    
    const auto& levels = m_world->getLevels();
    
    if (levels.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No levels in world");
        if (ImGui::Button("+ Create New Level")) {
            // TODO: Create level dialog
        }
        return;
    }
    
    if (m_worldViewMode == 0) {
        renderWorldSpatialView();
    } else {
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
    }  // end else (Grid View)
    
    ImGui::Separator();
    if (ImGui::Button("+ Create New Level")) {
        // TODO: Create level dialog
    }
#endif
}

void ViewportPanel::renderWorldSpatialView() {
#ifdef HAS_IMGUI
    if (!m_world) return;
    
    const auto& levels = m_world->getLevels();
    
    // Zoom controls
    ImGui::SliderFloat("Zoom", &m_worldSpatialZoom, 0.2f, 2.0f, "%.1f");
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        m_worldSpatialZoom = 1.0f;
        m_worldSpatialPanX = 0.0f;
        m_worldSpatialPanY = 0.0f;
    }
    
    // Canvas
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.y = std::max(canvasSize.y, 300.0f);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Background
    drawList->AddRectFilled(canvasPos, 
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(25, 30, 35, 255));
    
    // Grid
    float cellSize = 100.0f * m_worldSpatialZoom;
    ImU32 gridColor = IM_COL32(45, 50, 55, 255);
    
    for (float x = fmodf(m_worldSpatialPanX, cellSize); x < canvasSize.x; x += cellSize) {
        drawList->AddLine(ImVec2(canvasPos.x + x, canvasPos.y),
                         ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y), gridColor);
    }
    for (float y = fmodf(m_worldSpatialPanY, cellSize); y < canvasSize.y; y += cellSize) {
        drawList->AddLine(ImVec2(canvasPos.x, canvasPos.y + y),
                         ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y), gridColor);
    }
    
    // Origin
    float originX = canvasPos.x + canvasSize.x * 0.5f + m_worldSpatialPanX;
    float originY = canvasPos.y + canvasSize.y * 0.5f + m_worldSpatialPanY;
    drawList->AddCircleFilled(ImVec2(originX, originY), 5.0f, IM_COL32(255, 100, 100, 255));
    
    // Render levels
    for (const auto& level : levels) {
        const auto& gridPos = level->getGridPosition();
        
        float levelX = originX + gridPos.gridX * cellSize;
        float levelY = originY + gridPos.gridY * cellSize;
        float levelW = gridPos.pixelWidth * m_worldSpatialZoom * 0.5f;
        float levelH = gridPos.pixelHeight * m_worldSpatialZoom * 0.5f;
        
        // Level rectangle
        drawList->AddRectFilled(ImVec2(levelX, levelY), ImVec2(levelX + levelW, levelY + levelH),
                               IM_COL32(70, 90, 120, 200), 4.0f);
        drawList->AddRect(ImVec2(levelX, levelY), ImVec2(levelX + levelW, levelY + levelH),
                         IM_COL32(100, 140, 180, 255), 4.0f, 0, 2.0f);
        
        // Name
        drawList->AddText(ImVec2(levelX + 4, levelY + 4), IM_COL32(255, 255, 255, 255), 
                         level->getName().c_str());
        
        // Scene count
        char info[32];
        snprintf(info, sizeof(info), "%zu scenes", level->getScenes().size());
        drawList->AddText(ImVec2(levelX + 4, levelY + 20), IM_COL32(180, 180, 180, 200), info);
    }
    
    // Interaction
    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::InvisibleButton("world_spatial", canvasSize);
    
    if (ImGui::IsItemHovered()) {
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || 
            (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::GetIO().KeyAlt)) {
            m_worldSpatialPanX += ImGui::GetIO().MouseDelta.x;
            m_worldSpatialPanY += ImGui::GetIO().MouseDelta.y;
        }
        
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            m_worldSpatialZoom = std::clamp(m_worldSpatialZoom + wheel * 0.1f, 0.2f, 3.0f);
        }
        
        // Click to select
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyAlt) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            for (const auto& level : levels) {
                const auto& gridPos = level->getGridPosition();
                float levelX = originX + gridPos.gridX * cellSize;
                float levelY = originY + gridPos.gridY * cellSize;
                float levelW = gridPos.pixelWidth * m_worldSpatialZoom * 0.5f;
                float levelH = gridPos.pixelHeight * m_worldSpatialZoom * 0.5f;
                
                if (mousePos.x >= levelX && mousePos.x <= levelX + levelW &&
                    mousePos.y >= levelY && mousePos.y <= levelY + levelH) {
                    setLevel(const_cast<engine::Level*>(level.get()));
                    break;
                }
            }
        }
    }
    
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                     IM_COL32(60, 60, 70, 255));
    
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x + 5, canvasPos.y + canvasSize.y - 20));
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "Middle-mouse to pan, scroll to zoom");
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
    
    // View mode toggle: Spatial, Grid, List
    const char* viewModes[] = { "Spatial View", "Grid View", "List View" };
    if (ImGui::Button(viewModes[m_levelViewMode])) {
        m_levelViewMode = (m_levelViewMode + 1) % 3;
    }
    ImGui::SameLine();
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Click to cycle)");
    
    ImGui::Separator();
    
    const auto& scenes = m_level->getScenes();
    
    if (scenes.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No scenes in this level");
        if (ImGui::Button("+ Create New Scene")) {
            // TODO: Create scene dialog
        }
        return;
    }
    
    // Render based on view mode
    if (m_levelViewMode == 0) {
        renderSpatialView();
    } else if (m_levelViewMode == 1) {
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

void ViewportPanel::renderSpatialView() {
#ifdef HAS_IMGUI
    if (!m_level) return;
    
    const auto& scenes = m_level->getScenes();
    
    // Zoom controls
    ImGui::SliderFloat("Zoom", &m_spatialZoom, 0.2f, 2.0f, "%.1f");
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        m_spatialZoom = 1.0f;
        m_spatialPanX = 0.0f;
        m_spatialPanY = 0.0f;
    }
    
    // Canvas area
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.y = std::max(canvasSize.y, 300.0f);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Background
    drawList->AddRectFilled(canvasPos, 
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(25, 25, 30, 255));
    
    // Grid lines
    float cellSize = 64.0f * m_spatialZoom;
    ImU32 gridColor = IM_COL32(45, 45, 55, 255);
    
    float startX = fmodf(m_spatialPanX, cellSize);
    float startY = fmodf(m_spatialPanY, cellSize);
    
    for (float x = startX; x < canvasSize.x; x += cellSize) {
        drawList->AddLine(
            ImVec2(canvasPos.x + x, canvasPos.y),
            ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y),
            gridColor);
    }
    for (float y = startY; y < canvasSize.y; y += cellSize) {
        drawList->AddLine(
            ImVec2(canvasPos.x, canvasPos.y + y),
            ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y),
            gridColor);
    }
    
    // Origin point (center of canvas + pan offset)
    float originX = canvasPos.x + canvasSize.x * 0.5f + m_spatialPanX;
    float originY = canvasPos.y + canvasSize.y * 0.5f + m_spatialPanY;
    
    // Origin marker
    drawList->AddCircleFilled(ImVec2(originX, originY), 5.0f, IM_COL32(255, 100, 100, 255));
    drawList->AddText(ImVec2(originX + 8, originY - 8), IM_COL32(255, 100, 100, 200), "(0,0)");
    
    // Render scenes at their grid positions
    for (const auto& scene : scenes) {
        const auto& gridPos = scene->getGridPosition();
        
        // Calculate screen position (use drag position if dragging this scene)
        float sceneX, sceneY;
        if (m_draggedScene == scene.get()) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            sceneX = mousePos.x - m_dragStartX;
            sceneY = mousePos.y - m_dragStartY;
        } else {
            sceneX = originX + gridPos.gridX * cellSize;
            sceneY = originY + gridPos.gridY * cellSize;
        }
        float sceneW = (gridPos.pixelWidth / 64.0f) * cellSize;
        float sceneH = (gridPos.pixelHeight / 64.0f) * cellSize;
        
        // Skip if outside visible area
        if (sceneX + sceneW < canvasPos.x || sceneX > canvasPos.x + canvasSize.x ||
            sceneY + sceneH < canvasPos.y || sceneY > canvasPos.y + canvasSize.y) {
            continue;
        }
        
        // Room rectangle - highlight if selected or dragged
        bool isSelected = (m_selectedScene == scene.get());
        bool isDragging = (m_draggedScene == scene.get());
        
        ImU32 roomColor = isDragging ? IM_COL32(80, 120, 160, 180) : 
                          isSelected ? IM_COL32(70, 100, 140, 220) : 
                                       IM_COL32(60, 80, 110, 200);
        ImU32 borderColor = isDragging ? IM_COL32(100, 200, 255, 255) :
                            isSelected ? IM_COL32(100, 180, 255, 255) :
                                         IM_COL32(80, 120, 160, 255);
        
        drawList->AddRectFilled(
            ImVec2(sceneX, sceneY),
            ImVec2(sceneX + sceneW, sceneY + sceneH),
            roomColor, 4.0f);
        drawList->AddRect(
            ImVec2(sceneX, sceneY),
            ImVec2(sceneX + sceneW, sceneY + sceneH),
            borderColor, 4.0f, 0, isSelected || isDragging ? 3.0f : 2.0f);
        
        // Scene name
        const char* name = scene->getName().c_str();
        drawList->AddText(ImVec2(sceneX + 4, sceneY + 4), IM_COL32(255, 255, 255, 255), name);
        
        // Grid position
        char posInfo[32];
        if (isDragging && m_snapToGrid) {
            // Show snap preview position
            int snapX = (int)round((sceneX - originX) / cellSize);
            int snapY = (int)round((sceneY - originY) / cellSize);
            snprintf(posInfo, sizeof(posInfo), "(%d,%d) -> (%d,%d)", gridPos.gridX, gridPos.gridY, snapX, snapY);
        } else {
            snprintf(posInfo, sizeof(posInfo), "(%d,%d)", gridPos.gridX, gridPos.gridY);
        }
        drawList->AddText(ImVec2(sceneX + 4, sceneY + sceneH - 16), IM_COL32(180, 180, 180, 200), posInfo);
        
        // Size info
        char sizeInfo[32];
        snprintf(sizeInfo, sizeof(sizeInfo), "%dx%d", gridPos.pixelWidth, gridPos.pixelHeight);
        ImVec2 sizeTextSize = ImGui::CalcTextSize(sizeInfo);
        drawList->AddText(ImVec2(sceneX + sceneW - sizeTextSize.x - 4, sceneY + 4), 
                         IM_COL32(150, 150, 150, 200), sizeInfo);
    }
    
    // Handle interaction
    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::InvisibleButton("spatial_canvas", canvasSize);
    
    if (ImGui::IsItemHovered() || m_draggedScene) {
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        
        // Pan with middle mouse or Alt+drag
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || 
            (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::GetIO().KeyAlt && !m_draggedScene)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            m_spatialPanX += delta.x;
            m_spatialPanY += delta.y;
        }
        
        // Zoom with scroll
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f && !m_draggedScene) {
            m_spatialZoom = std::clamp(m_spatialZoom + wheel * 0.1f, 0.2f, 3.0f);
        }
        
        // Mouse down - start drag or select
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyAlt) {
            m_selectedScene = nullptr;
            m_draggedScene = nullptr;
            
            for (const auto& scene : scenes) {
                const auto& gridPos = scene->getGridPosition();
                float sceneX = originX + gridPos.gridX * cellSize;
                float sceneY = originY + gridPos.gridY * cellSize;
                float sceneW = (gridPos.pixelWidth / 64.0f) * cellSize;
                float sceneH = (gridPos.pixelHeight / 64.0f) * cellSize;
                
                if (mousePos.x >= sceneX && mousePos.x <= sceneX + sceneW &&
                    mousePos.y >= sceneY && mousePos.y <= sceneY + sceneH) {
                    m_selectedScene = scene.get();
                    m_draggedScene = scene.get();
                    m_dragStartX = mousePos.x - sceneX;
                    m_dragStartY = mousePos.y - sceneY;
                    m_dragStartGridX = gridPos.gridX;
                    m_dragStartGridY = gridPos.gridY;
                    break;
                }
            }
        }
        
        // Mouse released - finish drag
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_draggedScene) {
            // Calculate new grid position
            float sceneX = mousePos.x - m_dragStartX;
            float sceneY = mousePos.y - m_dragStartY;
            
            int newGridX, newGridY;
            if (m_snapToGrid) {
                newGridX = (int)round((sceneX - originX) / cellSize);
                newGridY = (int)round((sceneY - originY) / cellSize);
            } else {
                newGridX = (int)((sceneX - originX) / cellSize);
                newGridY = (int)((sceneY - originY) / cellSize);
            }
            
            // Update scene's grid position
            if (newGridX != m_dragStartGridX || newGridY != m_dragStartGridY) {
                m_draggedScene->setGridPosition(newGridX, newGridY, 
                    m_draggedScene->getWidth(), m_draggedScene->getHeight());
            }
            
            m_draggedScene = nullptr;
        }
        
        // Double-click to open scene
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_selectedScene) {
            setScene(m_selectedScene);
        }
    }
    
    // Cancel drag if mouse leaves or escape pressed
    if (m_draggedScene && (ImGui::IsKeyPressed(ImGuiKey_Escape) || !ImGui::IsMouseDown(ImGuiMouseButton_Left))) {
        m_draggedScene = nullptr;
    }
    
    // Border
    drawList->AddRect(canvasPos, 
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(60, 60, 70, 255));
    
    // Help text
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x + 5, canvasPos.y + canvasSize.y - 20));
    const char* helpText = m_draggedScene ? "Release to place, Escape to cancel" :
                           m_selectedScene ? "Drag to move, Double-click to open" :
                           "Click to select, Middle-mouse to pan, Scroll to zoom";
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", helpText);
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
    
    // Draw grid overlay
    if (m_showSceneGrid) {
        renderSceneGrid(ImGui::GetWindowDrawList(), renderPos, roomSize);
    }
    
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
    if (ImGui::IsItemHovered() || m_draggedActor) {
        ImVec2 mousePos = ImGui::GetMousePos();
        
        // Convert mouse to room coordinates
        float roomX = (mousePos.x - renderPos.x - m_panX) / m_zoom;
        float roomY = (mousePos.y - renderPos.y - m_panY) / m_zoom;
        
        // Left click - select actor
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            m_selectedActor = nullptr;
            m_draggedActor = nullptr;
            
            // Find actor under mouse
            for (auto& actor : m_scene->getActors()) {
                engine::Vec2 pos = actor->getPosition();
                float ax = pos.x;
                float ay = pos.y;
                float aw = 64.0f;  // Default size
                float ah = 64.0f;
                
                // Check sprite size if available
                auto* sprite = actor->getComponent<engine::SpriteComponent>();
                if (sprite) {
                    aw = sprite->getWidth();
                    ah = sprite->getHeight();
                }
                
                if (roomX >= ax && roomX <= ax + aw &&
                    roomY >= ay && roomY <= ay + ah) {
                    m_selectedActor = actor.get();
                    m_draggedActor = actor.get();
                    m_actorDragOffsetX = roomX - ax;
                    m_actorDragOffsetY = roomY - ay;
                    break;
                }
            }
        }
        
        // Drag actor
        if (m_draggedActor && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            float newX = roomX - m_actorDragOffsetX;
            float newY = roomY - m_actorDragOffsetY;
            
            // Snap to grid if enabled
            if (m_snapToGrid) {
                newX = round(newX / m_sceneGridSize) * m_sceneGridSize;
                newY = round(newY / m_sceneGridSize) * m_sceneGridSize;
            }
            
            m_draggedActor->setPosition(newX, newY);
        }
        
        // Release drag
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_draggedActor = nullptr;
        }
        
        // Middle mouse drag - pan view
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            m_panX += delta.x;
            m_panY += delta.y;
        }
        
        // Mouse wheel - zoom
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f && !m_draggedActor) {
            m_zoom = std::max(0.25f, std::min(4.0f, m_zoom + wheel * 0.1f));
        }
    }
    
    // Cancel drag with Escape
    if (m_draggedActor && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        m_draggedActor = nullptr;
    }
    
    // Show info overlay
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 10, cursorPos.y + 10));
    ImGui::BeginGroup();
    ImGui::Text("Scene: %s", m_scene->getName().c_str());
    ImGui::Text("Actors: %zu", m_scene->getActors().size());
    if (m_selectedActor) {
        ImGui::Text("Selected: %s", m_selectedActor->getName().c_str());
    }
    ImGui::Text("Zoom: %.0f%% | Pan: %.0f, %.0f", m_zoom * 100, m_panX, m_panY);
    const char* tip = m_draggedActor ? "Drag to move, Escape to cancel" :
                      m_selectedActor ? "Click to select, drag to move" :
                      "Click actor to select, Middle-mouse to pan";
    ImGui::Text("Tip: %s", tip);
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
        
        // Draw selection highlight
        if (actor.get() == m_selectedActor) {
            engine::Vec2 pos = actor->getPosition();
            float ax = offset.x + pos.x * m_zoom;
            float ay = offset.y + pos.y * m_zoom;
            float aw = 64.0f * m_zoom;
            float ah = 64.0f * m_zoom;
            
            auto* sprite = actor->getComponent<engine::SpriteComponent>();
            if (sprite) {
                aw = sprite->getWidth() * m_zoom;
                ah = sprite->getHeight() * m_zoom;
            }
            
            // Selection border
            ImU32 selColor = m_draggedActor == actor.get() ? 
                             IM_COL32(255, 200, 100, 255) : 
                             IM_COL32(100, 200, 255, 255);
            drawList->AddRect(
                ImVec2(ax - 2, ay - 2),
                ImVec2(ax + aw + 2, ay + ah + 2),
                selColor, 0, 0, 3.0f);
            
            // Corner handles
            float handleSize = 6.0f;
            ImU32 handleColor = IM_COL32(255, 255, 255, 255);
            drawList->AddRectFilled(ImVec2(ax - handleSize/2, ay - handleSize/2), 
                                   ImVec2(ax + handleSize/2, ay + handleSize/2), handleColor);
            drawList->AddRectFilled(ImVec2(ax + aw - handleSize/2, ay - handleSize/2), 
                                   ImVec2(ax + aw + handleSize/2, ay + handleSize/2), handleColor);
            drawList->AddRectFilled(ImVec2(ax - handleSize/2, ay + ah - handleSize/2), 
                                   ImVec2(ax + handleSize/2, ay + ah + handleSize/2), handleColor);
            drawList->AddRectFilled(ImVec2(ax + aw - handleSize/2, ay + ah - handleSize/2), 
                                   ImVec2(ax + aw + handleSize/2, ay + ah + handleSize/2), handleColor);
        }
    }
#endif
}

void ViewportPanel::renderSceneGrid(ImDrawList* drawList, ImVec2 offset, ImVec2 size) {
#ifdef HAS_IMGUI
    float gridSize = m_sceneGridSize * m_zoom;
    ImU32 gridColor = IM_COL32(60, 60, 70, 100);
    ImU32 majorGridColor = IM_COL32(80, 80, 90, 150);
    
    // Draw vertical lines
    for (float x = 0; x <= size.x; x += gridSize) {
        bool major = (int)(x / gridSize) % 4 == 0;
        drawList->AddLine(
            ImVec2(offset.x + x, offset.y),
            ImVec2(offset.x + x, offset.y + size.y),
            major ? majorGridColor : gridColor);
    }
    
    // Draw horizontal lines
    for (float y = 0; y <= size.y; y += gridSize) {
        bool major = (int)(y / gridSize) % 4 == 0;
        drawList->AddLine(
            ImVec2(offset.x, offset.y + y),
            ImVec2(offset.x + size.x, offset.y + y),
            major ? majorGridColor : gridColor);
    }
    
    // Grid size indicator
    char gridInfo[32];
    snprintf(gridInfo, sizeof(gridInfo), "Grid: %dpx", m_sceneGridSize);
    drawList->AddText(ImVec2(offset.x + 5, offset.y + size.y - 18), 
                     IM_COL32(100, 100, 100, 200), gridInfo);
#endif
}
