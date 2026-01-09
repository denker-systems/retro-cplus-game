/**
 * @file World2DRenderer.cpp
 * @brief Implementation of 2D world renderer
 */
#include "World2DRenderer.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

void World2DRenderer::render2DContent(ImDrawList* drawList, const RenderContext& ctx) {
    renderWorldContent(drawList, ctx);
}

void World2DRenderer::renderWorldContent(ImDrawList* drawList, const RenderContext& ctx) {
    if (!m_world || !drawList) return;
    
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    
    // Draw grid
    float baseGridSize = 100.0f;
    drawGrid(glm::vec2(canvasPos.x, canvasPos.y), glm::vec2(canvasSize.x, canvasSize.y),
             baseGridSize, m_zoom, m_panX, m_panY);
    
    // Draw levels
    const auto& levels = m_world->getLevels();
    float gridSize = baseGridSize * m_zoom;
    float levelW = 150.0f * m_zoom;
    float levelH = 100.0f * m_zoom;
    
    for (const auto& level : levels) {
        const auto& gridPos = level->getGridPosition();
        float x = canvasPos.x + gridPos.gridX * gridSize + m_panX + canvasSize.x / 2;
        float y = canvasPos.y + gridPos.gridY * gridSize + m_panY + canvasSize.y / 2;
        
        bool isSelected = (m_selectedLevel == level.get());
        bool isDragged = (m_draggedLevel == level.get());
        
        ImU32 fillColor = isDragged ? IM_COL32(100, 150, 255, 200) :
                          isSelected ? IM_COL32(80, 130, 200, 200) :
                          IM_COL32(60, 100, 150, 200);
        ImU32 borderColor = isSelected ? IM_COL32(255, 200, 50, 255) : IM_COL32(100, 150, 200, 255);
        
        drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + levelW, y + levelH), fillColor, 8.0f);
        drawList->AddRect(ImVec2(x, y), ImVec2(x + levelW, y + levelH), borderColor, 8.0f, 0, 2.0f);
        
        // Label
        ImVec2 textSize = ImGui::CalcTextSize(level->getName().c_str());
        drawList->AddText(ImVec2(x + (levelW - textSize.x) / 2, y + 10), 
                         IM_COL32(255, 255, 255, 255), level->getName().c_str());
    }
}

void World2DRenderer::handle2DInput(const RenderContext& ctx, bool hovered) {
    Base2DRenderer::handle2DInput(ctx, hovered);
    
    if (!m_world) return;
    
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    
    const auto& levels = m_world->getLevels();
    float gridSize = 100.0f * m_zoom;
    float levelW = 150.0f * m_zoom;
    float levelH = 100.0f * m_zoom;
    
    // Selection
    if (ImGui::IsMouseClicked(0)) {
        ImVec2 mousePos = io.MousePos;
        m_selectedLevel = nullptr;
        
        for (const auto& level : levels) {
            const auto& gridPos = level->getGridPosition();
            float x = canvasPos.x + gridPos.gridX * gridSize + m_panX + canvasSize.x / 2;
            float y = canvasPos.y + gridPos.gridY * gridSize + m_panY + canvasSize.y / 2;
            
            if (mousePos.x >= x && mousePos.x <= x + levelW &&
                mousePos.y >= y && mousePos.y <= y + levelH) {
                m_selectedLevel = level.get();
                m_draggedLevel = level.get();
                m_dragStartX = mousePos.x - x;
                m_dragStartY = mousePos.y - y;
                m_dragStartGridX = gridPos.gridX;
                m_dragStartGridY = gridPos.gridY;
                break;
            }
        }
    }
    
    // Drag
    if (m_draggedLevel && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 mousePos = io.MousePos;
        float newX = mousePos.x - canvasPos.x - m_dragStartX - m_panX - canvasSize.x / 2;
        float newY = mousePos.y - canvasPos.y - m_dragStartY - m_panY - canvasSize.y / 2;
        
        int newGridX = static_cast<int>(std::round(newX / gridSize));
        int newGridY = static_cast<int>(std::round(newY / gridSize));
        
        m_draggedLevel->setGridPosition(newGridX, newGridY);
    }
    
    if (ImGui::IsMouseReleased(0)) {
        m_draggedLevel = nullptr;
    }
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
