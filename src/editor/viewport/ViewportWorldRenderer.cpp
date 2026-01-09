/**
 * @file ViewportWorldRenderer.cpp
 * @brief Implementation of World-level viewport renderer
 */
#include "editor/viewport/ViewportWorldRenderer.h"
#include "editor/core/EditorContext.h"
#include "editor/core/SelectionManager.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include <algorithm>
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {

void ViewportWorldRenderer::initialize(EditorContext* context, SelectionManager* selectionManager) {
    m_context = context;
    m_selectionManager = selectionManager;
}

void ViewportWorldRenderer::update(float deltaTime) {
    // No per-frame updates needed
}

void ViewportWorldRenderer::render(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    if (!m_world || !drawList) return;
    
    if (m_viewMode == 0) {
        renderSpatialView(drawList, ctx);
    } else {
        renderGridView(drawList, ctx);
    }
}

void ViewportWorldRenderer::renderSpatialView(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    const auto& levels = m_world->getLevels();
    
    // Background
    drawList->AddRectFilled(
        ImVec2(ctx.viewportPos.x, ctx.viewportPos.y),
        ImVec2(ctx.viewportPos.x + ctx.viewportSize.x, ctx.viewportPos.y + ctx.viewportSize.y),
        IM_COL32(25, 30, 35, 255));
    
    // Grid
    float cellSize = 100.0f * ctx.zoom;
    ImU32 gridColor = IM_COL32(45, 50, 55, 255);
    
    for (float x = fmodf(ctx.panX, cellSize); x < ctx.viewportSize.x; x += cellSize) {
        drawList->AddLine(
            ImVec2(ctx.viewportPos.x + x, ctx.viewportPos.y),
            ImVec2(ctx.viewportPos.x + x, ctx.viewportPos.y + ctx.viewportSize.y),
            gridColor);
    }
    for (float y = fmodf(ctx.panY, cellSize); y < ctx.viewportSize.y; y += cellSize) {
        drawList->AddLine(
            ImVec2(ctx.viewportPos.x, ctx.viewportPos.y + y),
            ImVec2(ctx.viewportPos.x + ctx.viewportSize.x, ctx.viewportPos.y + y),
            gridColor);
    }
    
    // Origin
    float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
    float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
    drawList->AddCircleFilled(ImVec2(originX, originY), 5.0f, IM_COL32(255, 100, 100, 255));
    
    // Render levels
    for (const auto& level : levels) {
        const auto& gridPos = level->getGridPosition();
        
        float levelX, levelY;
        if (m_draggedLevel == level.get()) {
            ImVec2 mousePos = ImGui::GetIO().MousePos;
            levelX = mousePos.x - m_dragStartX;
            levelY = mousePos.y - m_dragStartY;
        } else {
            levelX = originX + gridPos.gridX * cellSize;
            levelY = originY + gridPos.gridY * cellSize;
        }
        float levelW = gridPos.pixelWidth * ctx.zoom * 0.5f;
        float levelH = gridPos.pixelHeight * ctx.zoom * 0.5f;
        
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
        snprintf(posInfo, sizeof(posInfo), "(%d,%d)", gridPos.gridX, gridPos.gridY);
        drawList->AddText(ImVec2(levelX + 4, levelY + levelH - 16), IM_COL32(180, 180, 180, 200), posInfo);
        
        // Scene count
        char info[32];
        snprintf(info, sizeof(info), "%zu scenes", level->getScenes().size());
        drawList->AddText(ImVec2(levelX + 4, levelY + 20), IM_COL32(180, 180, 180, 200), info);
    }
    
    // Border
    drawList->AddRect(
        ImVec2(ctx.viewportPos.x, ctx.viewportPos.y),
        ImVec2(ctx.viewportPos.x + ctx.viewportSize.x, ctx.viewportPos.y + ctx.viewportSize.y),
        IM_COL32(60, 60, 70, 255));
}

void ViewportWorldRenderer::renderGridView(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    const auto& levels = m_world->getLevels();
    
    float panelWidth = ctx.viewportSize.x;
    int columns = std::max(1, (int)(panelWidth / 250.0f));
    float cardW = 230.0f;
    float cardH = 120.0f;
    float spacing = 10.0f;
    
    int col = 0;
    float startX = ctx.viewportPos.x + 10.0f;
    float startY = ctx.viewportPos.y + 10.0f;
    
    for (const auto& level : levels) {
        float cardX = startX + col * (cardW + spacing);
        float cardY = startY + (col / columns) * (cardH + spacing);
        
        renderLevelCard(drawList, level.get(), cardX, cardY, cardW, cardH);
        
        col++;
        if (col >= columns) col = 0;
    }
}

void ViewportWorldRenderer::renderLevelCard(ImDrawList* drawList, engine::Level* level, 
                                             float x, float y, float w, float h) {
    bool isSelected = (m_selectedLevel == level);
    
    ImU32 bgColor = isSelected ? IM_COL32(60, 80, 100, 255) : IM_COL32(50, 60, 70, 255);
    drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), bgColor, 4.0f);
    
    // Icon area
    drawList->AddRectFilled(ImVec2(x + 5, y + 5), ImVec2(x + w - 5, y + 60), IM_COL32(40, 50, 60, 255));
    
    // Level name
    drawList->AddText(ImVec2(x + 10, y + 70), IM_COL32(255, 255, 255, 255), level->getName().c_str());
    
    // Scene count
    char info[32];
    snprintf(info, sizeof(info), "%zu scenes", level->getScenes().size());
    drawList->AddText(ImVec2(x + 10, y + 90), IM_COL32(150, 150, 150, 255), info);
    
    // Selection border
    if (isSelected) {
        drawList->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), IM_COL32(100, 150, 200, 255), 4.0f, 0, 2.0f);
    }
}

bool ViewportWorldRenderer::onMouseClick(float x, float y, const ViewportRenderContext& ctx) {
    m_selectedLevel = findLevelAt(x, y, ctx);
    
    if (m_selectedLevel) {
        m_draggedLevel = m_selectedLevel;
        
        const auto& gridPos = m_selectedLevel->getGridPosition();
        float cellSize = 100.0f * ctx.zoom;
        float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
        float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
        float levelX = originX + gridPos.gridX * cellSize;
        float levelY = originY + gridPos.gridY * cellSize;
        
        m_dragStartX = x - levelX;
        m_dragStartY = y - levelY;
        m_dragStartGridX = gridPos.gridX;
        m_dragStartGridY = gridPos.gridY;
        
        return true;
    }
    
    return false;
}

bool ViewportWorldRenderer::onMouseDrag(float x, float y, float deltaX, float deltaY, 
                                         const ViewportRenderContext& ctx) {
    return m_draggedLevel != nullptr;
}

void ViewportWorldRenderer::onMouseRelease(float x, float y, const ViewportRenderContext& ctx) {
    if (m_draggedLevel) {
        float cellSize = 100.0f * ctx.zoom;
        float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
        float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
        
        float levelX = x - m_dragStartX;
        float levelY = y - m_dragStartY;
        
        int newGridX = (int)round((levelX - originX) / cellSize);
        int newGridY = (int)round((levelY - originY) / cellSize);
        
        if (newGridX != m_dragStartGridX || newGridY != m_dragStartGridY) {
            m_draggedLevel->setGridPosition(newGridX, newGridY,
                m_draggedLevel->getWidth(), m_draggedLevel->getHeight());
        }
        
        m_draggedLevel = nullptr;
    }
}

bool ViewportWorldRenderer::onDoubleClick(float x, float y, const ViewportRenderContext& ctx) {
    engine::Level* level = findLevelAt(x, y, ctx);
    if (level) {
        m_selectedLevel = level;
        return true;  // Signal navigation
    }
    return false;
}

void ViewportWorldRenderer::onScroll(float delta, const ViewportRenderContext& ctx) {
    // Zoom handled by parent
}

void ViewportWorldRenderer::clearSelection() {
    m_selectedLevel = nullptr;
    m_draggedLevel = nullptr;
}

engine::Level* ViewportWorldRenderer::findLevelAt(float x, float y, const ViewportRenderContext& ctx) {
    if (!m_world) return nullptr;
    
    const auto& levels = m_world->getLevels();
    float cellSize = 100.0f * ctx.zoom;
    float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
    float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
    
    for (const auto& level : levels) {
        const auto& gridPos = level->getGridPosition();
        float levelX = originX + gridPos.gridX * cellSize;
        float levelY = originY + gridPos.gridY * cellSize;
        float levelW = gridPos.pixelWidth * ctx.zoom * 0.5f;
        float levelH = gridPos.pixelHeight * ctx.zoom * 0.5f;
        
        if (x >= levelX && x <= levelX + levelW && y >= levelY && y <= levelY + levelH) {
            return level.get();
        }
    }
    
    return nullptr;
}

} // namespace editor

#endif // HAS_IMGUI
