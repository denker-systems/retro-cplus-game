/**
 * @file ViewportPanel_World.cpp
 * @brief ViewportPanel World view rendering
 */
#include "ViewportPanel.h"
#include "editor/viewport/Viewport3DPanel.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include <algorithm>
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

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
        
        // Calculate position (use drag position if dragging)
        float levelX, levelY;
        if (m_draggedLevel == level.get()) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            levelX = mousePos.x - m_dragLevelStartX;
            levelY = mousePos.y - m_dragLevelStartY;
        } else {
            levelX = originX + gridPos.gridX * cellSize;
            levelY = originY + gridPos.gridY * cellSize;
        }
        float levelW = gridPos.pixelWidth * m_worldSpatialZoom * 0.5f;
        float levelH = gridPos.pixelHeight * m_worldSpatialZoom * 0.5f;
        
        // Level rectangle - highlight if selected or dragged
        bool isSelected = (m_selectedLevel == level.get());
        bool isDragging = (m_draggedLevel == level.get());
        
        ImU32 fillColor = isDragging ? IM_COL32(80, 110, 150, 180) :
                          isSelected ? IM_COL32(70, 100, 140, 220) :
                                       IM_COL32(70, 90, 120, 200);
        ImU32 borderColor = isDragging ? IM_COL32(100, 200, 255, 255) :
                            isSelected ? IM_COL32(100, 180, 255, 255) :
                                         IM_COL32(100, 140, 180, 255);
        
        drawList->AddRectFilled(ImVec2(levelX, levelY), ImVec2(levelX + levelW, levelY + levelH),
                               fillColor, 4.0f);
        drawList->AddRect(ImVec2(levelX, levelY), ImVec2(levelX + levelW, levelY + levelH),
                         borderColor, 4.0f, 0, isSelected || isDragging ? 3.0f : 2.0f);
        
        // Name
        drawList->AddText(ImVec2(levelX + 4, levelY + 4), IM_COL32(255, 255, 255, 255), 
                         level->getName().c_str());
        
        // Grid position info
        char posInfo[32];
        if (isDragging) {
            int snapX = (int)round((levelX - originX) / cellSize);
            int snapY = (int)round((levelY - originY) / cellSize);
            snprintf(posInfo, sizeof(posInfo), "(%d,%d) -> (%d,%d)", gridPos.gridX, gridPos.gridY, snapX, snapY);
        } else {
            snprintf(posInfo, sizeof(posInfo), "(%d,%d)", gridPos.gridX, gridPos.gridY);
        }
        drawList->AddText(ImVec2(levelX + 4, levelY + levelH - 16), IM_COL32(180, 180, 180, 200), posInfo);
        
        // Scene count
        char info[32];
        snprintf(info, sizeof(info), "%zu scenes", level->getScenes().size());
        drawList->AddText(ImVec2(levelX + 4, levelY + 20), IM_COL32(180, 180, 180, 200), info);
    }
    
    // Handle interaction
    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::InvisibleButton("world_spatial", canvasSize);
    
    if (ImGui::IsItemHovered() || m_draggedLevel) {
        ImVec2 mousePos = ImGui::GetIO().MousePos;
        
        // Pan with middle mouse or Alt+drag
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || 
            (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::GetIO().KeyAlt && !m_draggedLevel)) {
            m_worldSpatialPanX += ImGui::GetIO().MouseDelta.x;
            m_worldSpatialPanY += ImGui::GetIO().MouseDelta.y;
        }
        
        // Zoom with scroll
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f && !m_draggedLevel) {
            m_worldSpatialZoom = std::clamp(m_worldSpatialZoom + wheel * 0.1f, 0.2f, 3.0f);
        }
        
        // Mouse down - start drag or select (same as Level view)
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyAlt) {
            m_selectedLevel = nullptr;
            m_draggedLevel = nullptr;
            
            for (const auto& level : levels) {
                const auto& gridPos = level->getGridPosition();
                float levelX = originX + gridPos.gridX * cellSize;
                float levelY = originY + gridPos.gridY * cellSize;
                float levelW = gridPos.pixelWidth * m_worldSpatialZoom * 0.5f;
                float levelH = gridPos.pixelHeight * m_worldSpatialZoom * 0.5f;
                
                if (mousePos.x >= levelX && mousePos.x <= levelX + levelW &&
                    mousePos.y >= levelY && mousePos.y <= levelY + levelH) {
                    m_selectedLevel = level.get();
                    m_draggedLevel = level.get();
                    m_dragLevelStartX = mousePos.x - levelX;
                    m_dragLevelStartY = mousePos.y - levelY;
                    m_dragLevelStartGridX = gridPos.gridX;
                    m_dragLevelStartGridY = gridPos.gridY;
                    break;
                }
            }
        }
        
        // Mouse released - finish drag
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_draggedLevel) {
            float levelX = mousePos.x - m_dragLevelStartX;
            float levelY = mousePos.y - m_dragLevelStartY;
            
            int newGridX = (int)round((levelX - originX) / cellSize);
            int newGridY = (int)round((levelY - originY) / cellSize);
            
            if (newGridX != m_dragLevelStartGridX || newGridY != m_dragLevelStartGridY) {
                m_draggedLevel->setGridPosition(newGridX, newGridY,
                    m_draggedLevel->getWidth(), m_draggedLevel->getHeight());
            }
            
            m_draggedLevel = nullptr;
        }
        
        // Double-click to enter level (same as Level view)
        if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && m_selectedLevel) {
            setLevel(const_cast<engine::Level*>(m_selectedLevel));
        }
    }
    
    // Cancel drag
    if (m_draggedLevel && (ImGui::IsKeyPressed(ImGuiKey_Escape) || !ImGui::IsMouseDown(ImGuiMouseButton_Left))) {
        m_draggedLevel = nullptr;
    }
    
    // Border
    drawList->AddRect(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                     IM_COL32(60, 60, 70, 255));
    
    // Help text (same style as Level view)
    ImGui::SetCursorScreenPos(ImVec2(canvasPos.x + 5, canvasPos.y + canvasSize.y - 20));
    const char* helpText = m_draggedLevel ? "Release to place, Escape to cancel" :
                           m_selectedLevel ? "Drag to move, Double-click to enter" :
                           "Click to select, Middle-mouse to pan, Scroll to zoom";
    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", helpText);
#endif
}

void ViewportPanel::renderWorld3D() {
#ifdef HAS_IMGUI
    if (!m_world) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No World loaded");
        return;
    }
    
    // Use shared 3D viewport for World view
    if (!m_viewport3D) {
        m_viewport3D = std::make_unique<editor::Viewport3DPanel>();
        m_viewport3D->setSelectionManager(m_selectionManager);
    }
    
    // Configure for World view
    m_viewport3D->setViewLevel(editor::View3DLevel::World);
    m_viewport3D->setWorld(m_world);
    
    // Render 3D viewport
    m_viewport3D->render();
    
    // Handle double-click navigation
    if (m_viewport3D->wasDoubleClicked()) {
        int selectedIdx = m_viewport3D->getSelectedIndex();
        const auto& levels = m_world->getLevels();
        if (selectedIdx >= 0 && selectedIdx < (int)levels.size()) {
            setLevel(levels[selectedIdx].get());  // Use setLevel to sync with SelectionManager
            // Reset selection for new view
            m_viewport3D->resetSelection();
        }
        m_viewport3D->clearDoubleClick();
    }
#endif
}
