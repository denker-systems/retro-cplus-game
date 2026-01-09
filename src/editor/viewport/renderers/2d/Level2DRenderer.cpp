/**
 * @file Level2DRenderer.cpp
 * @brief Implementation of 2D level renderer
 */
#include "Level2DRenderer.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

void Level2DRenderer::render2DContent(ImDrawList* drawList, const RenderContext& ctx) {
    if (m_level) {
        renderLevelContent(drawList, ctx);
    } else {
        // Fallback to parent world rendering
        World2DRenderer::render2DContent(drawList, ctx);
    }
}

void Level2DRenderer::renderLevelContent(ImDrawList* drawList, const RenderContext& ctx) {
    if (!m_level || !drawList) return;
    
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    
    // Draw grid
    float baseGridSize = 120.0f;
    drawGrid(glm::vec2(canvasPos.x, canvasPos.y), glm::vec2(canvasSize.x, canvasSize.y),
             baseGridSize, m_zoom, m_panX, m_panY);
    
    // Draw scenes
    const auto& scenes = m_level->getScenes();
    float gridSize = baseGridSize * m_zoom;
    float sceneW = 100.0f * m_zoom;
    float sceneH = 75.0f * m_zoom;
    
    for (const auto& scene : scenes) {
        const auto& gridPos = scene->getGridPosition();
        float x = canvasPos.x + gridPos.gridX * gridSize + m_panX + canvasSize.x / 2;
        float y = canvasPos.y + gridPos.gridY * gridSize + m_panY + canvasSize.y / 2;
        
        bool isSelected = (m_selectedScene == scene.get());
        bool isDragged = (m_draggedScene == scene.get());
        
        ImU32 fillColor = isDragged ? IM_COL32(100, 200, 100, 200) :
                          isSelected ? IM_COL32(80, 180, 80, 200) :
                          IM_COL32(60, 120, 60, 200);
        ImU32 borderColor = isSelected ? IM_COL32(255, 200, 50, 255) : IM_COL32(100, 180, 100, 255);
        
        drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + sceneW, y + sceneH), fillColor, 4.0f);
        drawList->AddRect(ImVec2(x, y), ImVec2(x + sceneW, y + sceneH), borderColor, 4.0f, 0, 2.0f);
        
        // Label
        drawList->AddText(ImVec2(x + 5, y + 5), IM_COL32(255, 255, 255, 255), scene->getName().c_str());
    }
}

void Level2DRenderer::handle2DInput(const RenderContext& ctx, bool hovered) {
    if (m_level) {
        // Level-specific input
        Base2DRenderer::handle2DInput(ctx, hovered);
        
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();
        
        const auto& scenes = m_level->getScenes();
        float gridSize = 120.0f * m_zoom;
        float sceneW = 100.0f * m_zoom;
        float sceneH = 75.0f * m_zoom;
        
        // Selection
        if (ImGui::IsMouseClicked(0)) {
            ImVec2 mousePos = io.MousePos;
            m_selectedScene = nullptr;
            
            for (const auto& scene : scenes) {
                const auto& gridPos = scene->getGridPosition();
                float x = canvasPos.x + gridPos.gridX * gridSize + m_panX + canvasSize.x / 2;
                float y = canvasPos.y + gridPos.gridY * gridSize + m_panY + canvasSize.y / 2;
                
                if (mousePos.x >= x && mousePos.x <= x + sceneW &&
                    mousePos.y >= y && mousePos.y <= y + sceneH) {
                    m_selectedScene = scene.get();
                    m_draggedScene = scene.get();
                    m_sceneDragStartX = mousePos.x - x;
                    m_sceneDragStartY = mousePos.y - y;
                    m_sceneDragStartGridX = gridPos.gridX;
                    m_sceneDragStartGridY = gridPos.gridY;
                    break;
                }
            }
        }
        
        // Drag
        if (m_draggedScene && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 mousePos = io.MousePos;
            float newX = mousePos.x - canvasPos.x - m_sceneDragStartX - m_panX - canvasSize.x / 2;
            float newY = mousePos.y - canvasPos.y - m_sceneDragStartY - m_panY - canvasSize.y / 2;
            
            int newGridX = static_cast<int>(std::round(newX / gridSize));
            int newGridY = static_cast<int>(std::round(newY / gridSize));
            
            m_draggedScene->setGridPosition(newGridX, newGridY);
        }
        
        if (ImGui::IsMouseReleased(0)) {
            m_draggedScene = nullptr;
        }
    } else {
        // Fallback to parent world input
        World2DRenderer::handle2DInput(ctx, hovered);
    }
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
