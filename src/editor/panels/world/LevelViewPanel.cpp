/**
 * @file LevelViewPanel.cpp
 * @brief Implementation of LevelViewPanel
 */
#include "LevelViewPanel.h"
#include "editor/core/EditorContext.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

LevelViewPanel::LevelViewPanel(EditorContext& context)
    : m_context(context) {
}

void LevelViewPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        if (!m_level) {
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
                "No Level selected");
            ImGui::Text("Select a Level from World View");
            ImGui::End();
            return;
        }
        
        // Header
        ImGui::Text("Level: %s", m_level->getName().c_str());
        ImGui::Text("ID: %s", m_level->getId().c_str());
        ImGui::Separator();
        
        // View toggle - 3 modes: Spatial, Grid, List
        const char* viewNames[] = { "Spatial View", "Grid View", "List View" };
        int currentView = m_showSpatialView ? 0 : (m_showGridView ? 1 : 2);
        
        ImGui::Text("View Mode:");
        ImGui::SameLine();
        if (ImGui::Button(viewNames[currentView])) {
            // Cycle: Spatial -> Grid -> List -> Spatial
            if (m_showSpatialView) {
                m_showSpatialView = false;
                m_showGridView = true;
            } else if (m_showGridView) {
                m_showGridView = false;
            } else {
                m_showSpatialView = true;
            }
        }
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(Click to cycle)");
        
        ImGui::Separator();
        
        // Scene display
        if (m_showSpatialView) {
            renderSpatialGrid();
        } else if (m_showGridView) {
            renderSceneGrid();
        } else {
            // List view (simplified)
            const auto& scenes = m_level->getScenes();
            for (const auto& scene : scenes) {
                bool isSelected = (scene->getName() == m_selectedSceneId);
                
                if (ImGui::Selectable(scene->getName().c_str(), isSelected)) {
                    m_selectedSceneId = scene->getName();
                    m_context.selectedSceneId = scene->getName();
                }
            }
        }
        
        ImGui::Separator();
        
        renderCreateSceneButton();
    }
    ImGui::End();
#endif
}

void LevelViewPanel::renderSceneGrid() {
#ifdef HAS_IMGUI
    const auto& scenes = m_level->getScenes();
    
    if (scenes.empty()) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
            "No scenes in this level");
        ImGui::Text("Click 'Create New Scene' to add one");
        return;
    }
    
    // Grid layout
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columns = (int)(panelWidth / 200.0f);  // 200px per card
    if (columns < 1) columns = 1;
    
    ImGui::Columns(columns, nullptr, false);
    
    for (const auto& scene : scenes) {
        renderSceneCard(scene.get());
        ImGui::NextColumn();
    }
    
    ImGui::Columns(1);
#endif
}

void LevelViewPanel::renderSpatialGrid() {
#ifdef HAS_IMGUI
    const auto& scenes = m_level->getScenes();
    
    // Zoom controls
    ImGui::SliderFloat("Zoom", &m_gridZoom, 0.1f, 2.0f, "%.1f");
    ImGui::SameLine();
    if (ImGui::Button("Reset View")) {
        m_gridZoom = 1.0f;
        m_gridOffsetX = 0.0f;
        m_gridOffsetY = 0.0f;
    }
    
    // Canvas area
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    canvasSize.y = std::max(canvasSize.y, 300.0f);
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Background
    drawList->AddRectFilled(canvasPos, 
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(20, 20, 25, 255));
    
    // Grid lines
    float cellSize = 64.0f * m_gridZoom;  // Base cell = 64px
    ImU32 gridColor = IM_COL32(40, 40, 50, 255);
    
    for (float x = fmodf(m_gridOffsetX, cellSize); x < canvasSize.x; x += cellSize) {
        drawList->AddLine(
            ImVec2(canvasPos.x + x, canvasPos.y),
            ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y),
            gridColor);
    }
    for (float y = fmodf(m_gridOffsetY, cellSize); y < canvasSize.y; y += cellSize) {
        drawList->AddLine(
            ImVec2(canvasPos.x, canvasPos.y + y),
            ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y),
            gridColor);
    }
    
    // Origin marker
    float originX = canvasPos.x + m_gridOffsetX + canvasSize.x * 0.5f;
    float originY = canvasPos.y + m_gridOffsetY + canvasSize.y * 0.5f;
    drawList->AddCircleFilled(ImVec2(originX, originY), 4.0f, IM_COL32(255, 100, 100, 255));
    
    // Render scenes at their grid positions
    float scale = cellSize / 64.0f;  // Pixels per grid unit
    
    for (const auto& scene : scenes) {
        const auto& gridPos = scene->getGridPosition();
        bool isSelected = (scene->getName() == m_selectedSceneId);
        
        // Calculate screen position
        float sceneX = originX + gridPos.gridX * cellSize;
        float sceneY = originY + gridPos.gridY * cellSize;
        float sceneW = (gridPos.pixelWidth / 64.0f) * cellSize;  // Scale based on room size
        float sceneH = (gridPos.pixelHeight / 64.0f) * cellSize;
        
        // Room rectangle
        ImU32 roomColor = isSelected 
            ? IM_COL32(80, 140, 200, 200) 
            : IM_COL32(60, 80, 100, 180);
        ImU32 borderColor = isSelected 
            ? IM_COL32(100, 180, 255, 255) 
            : IM_COL32(80, 100, 120, 255);
        
        drawList->AddRectFilled(
            ImVec2(sceneX, sceneY),
            ImVec2(sceneX + sceneW, sceneY + sceneH),
            roomColor, 4.0f);
        drawList->AddRect(
            ImVec2(sceneX, sceneY),
            ImVec2(sceneX + sceneW, sceneY + sceneH),
            borderColor, 4.0f, 0, 2.0f);
        
        // Scene name
        const char* name = scene->getName().c_str();
        ImVec2 textSize = ImGui::CalcTextSize(name);
        if (textSize.x < sceneW - 4) {
            drawList->AddText(
                ImVec2(sceneX + 4, sceneY + 4),
                IM_COL32(255, 255, 255, 255),
                name);
        }
        
        // Grid position info
        char posInfo[32];
        snprintf(posInfo, sizeof(posInfo), "(%d,%d)", gridPos.gridX, gridPos.gridY);
        drawList->AddText(
            ImVec2(sceneX + 4, sceneY + sceneH - 18),
            IM_COL32(180, 180, 180, 200),
            posInfo);
        
        // Room size info
        char sizeInfo[32];
        snprintf(sizeInfo, sizeof(sizeInfo), "%dx%d", gridPos.pixelWidth, gridPos.pixelHeight);
        drawList->AddText(
            ImVec2(sceneX + sceneW - ImGui::CalcTextSize(sizeInfo).x - 4, sceneY + 4),
            IM_COL32(150, 150, 150, 200),
            sizeInfo);
    }
    
    // Handle interaction
    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::InvisibleButton("spatial_canvas", canvasSize);
    
    if (ImGui::IsItemHovered()) {
        // Pan with middle mouse or drag
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle) || 
            (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && ImGui::GetIO().KeyAlt)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            m_gridOffsetX += delta.x;
            m_gridOffsetY += delta.y;
        }
        
        // Zoom with scroll
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f) {
            m_gridZoom = std::clamp(m_gridZoom + wheel * 0.1f, 0.1f, 3.0f);
        }
        
        // Click to select scene
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::GetIO().KeyAlt) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            
            for (const auto& scene : scenes) {
                const auto& gridPos = scene->getGridPosition();
                float sceneX = originX + gridPos.gridX * cellSize;
                float sceneY = originY + gridPos.gridY * cellSize;
                float sceneW = (gridPos.pixelWidth / 64.0f) * cellSize;
                float sceneH = (gridPos.pixelHeight / 64.0f) * cellSize;
                
                if (mousePos.x >= sceneX && mousePos.x <= sceneX + sceneW &&
                    mousePos.y >= sceneY && mousePos.y <= sceneY + sceneH) {
                    m_selectedSceneId = scene->getName();
                    m_context.selectedSceneId = scene->getName();
                    break;
                }
            }
        }
    }
    
    // Border
    drawList->AddRect(canvasPos, 
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(60, 60, 70, 255));
#endif
}

void LevelViewPanel::renderSceneCard(engine::Scene* scene) {
#ifdef HAS_IMGUI
    if (!scene) return;
    
    bool isSelected = (scene->getName() == m_selectedSceneId);
    
    // Card background
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    ImVec2 cardSize(180, 150);
    
    ImU32 bgColor = isSelected 
        ? IM_COL32(80, 120, 160, 255) 
        : IM_COL32(50, 50, 60, 255);
    
    ImGui::GetWindowDrawList()->AddRectFilled(
        cursorPos,
        ImVec2(cursorPos.x + cardSize.x, cursorPos.y + cardSize.y),
        bgColor,
        4.0f);
    
    // Thumbnail area (placeholder)
    ImVec2 thumbSize(cardSize.x - 10, 100);
    ImGui::GetWindowDrawList()->AddRectFilled(
        ImVec2(cursorPos.x + 5, cursorPos.y + 5),
        ImVec2(cursorPos.x + cardSize.x - 5, cursorPos.y + 105),
        IM_COL32(30, 30, 40, 255));
    
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + cardSize.x/2 - 30, cursorPos.y + 50));
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
        m_selectedSceneId = scene->getName();
        m_context.selectedSceneId = scene->getName();
    }
    
    // Double-click to open in Viewport
    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0)) {
        // TODO: Focus ViewportPanel with this scene
    }
    
    // Advance cursor
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x, cursorPos.y + cardSize.y + 10));
#endif
}

void LevelViewPanel::renderCreateSceneButton() {
#ifdef HAS_IMGUI
    if (ImGui::Button("+ Create New Scene")) {
        m_showCreateDialog = true;
        m_newSceneId[0] = '\0';
        m_newSceneName[0] = '\0';
    }
    
    if (m_showCreateDialog) {
        ImGui::OpenPopup("Create Scene");
    }
    
    if (ImGui::BeginPopupModal("Create Scene", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Create a new Scene in this Level");
        ImGui::Separator();
        
        ImGui::InputText("ID", m_newSceneId, sizeof(m_newSceneId));
        ImGui::InputText("Name", m_newSceneName, sizeof(m_newSceneName));
        
        ImGui::Separator();
        
        if (ImGui::Button("Create", ImVec2(120, 0))) {
            // TODO: Create scene in Level
            // auto scene = std::make_unique<Scene>(m_newSceneId);
            // m_level->addScene(std::move(scene));
            
            m_showCreateDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            m_showCreateDialog = false;
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
#endif
}

void LevelViewPanel::renderSceneProperties(engine::Scene* scene) {
#ifdef HAS_IMGUI
    if (!scene) return;
    
    ImGui::Separator();
    ImGui::Text("Scene Properties:");
    
    ImGui::Text("Name: %s", scene->getName().c_str());
    ImGui::Text("Actors: %zu", scene->getActors().size());
    
    if (ImGui::Button("Open in Viewport")) {
        // TODO: Open ViewportPanel with this scene
        m_context.selectedSceneId = scene->getName();
    }
    
    ImGui::SameLine();
    if (ImGui::Button("Delete Scene")) {
        // TODO: Confirm dialog
    }
#endif
}
