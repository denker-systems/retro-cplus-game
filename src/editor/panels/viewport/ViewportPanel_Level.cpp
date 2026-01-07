/**
 * @file ViewportPanel_Level.cpp
 * @brief ViewportPanel Level and Spatial view rendering
 */
#include "ViewportPanel.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include <algorithm>
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

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
