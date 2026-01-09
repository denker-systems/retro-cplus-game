/**
 * @file ViewportLevelRenderer.cpp
 * @brief Implementation of Level-level viewport renderer
 */
#include "editor/viewport/ViewportLevelRenderer.h"
#include "editor/core/EditorContext.h"
#include "editor/core/SelectionManager.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include <algorithm>
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {

void ViewportLevelRenderer::initialize(EditorContext* context, SelectionManager* selectionManager) {
    m_context = context;
    m_selectionManager = selectionManager;
}

void ViewportLevelRenderer::update(float deltaTime) {
}

void ViewportLevelRenderer::render(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    if (!m_level || !drawList) return;
    
    if (m_viewMode == 0) {
        renderSpatialView(drawList, ctx);
    } else {
        renderGridView(drawList, ctx);
    }
}

void ViewportLevelRenderer::renderSpatialView(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    const auto& scenes = m_level->getScenes();
    
    // Background
    drawList->AddRectFilled(
        ImVec2(ctx.viewportPos.x, ctx.viewportPos.y),
        ImVec2(ctx.viewportPos.x + ctx.viewportSize.x, ctx.viewportPos.y + ctx.viewportSize.y),
        IM_COL32(25, 25, 30, 255));
    
    // Grid
    float cellSize = 64.0f * ctx.zoom;
    ImU32 gridColor = IM_COL32(45, 45, 55, 255);
    
    float startX = fmodf(ctx.panX, cellSize);
    float startY = fmodf(ctx.panY, cellSize);
    
    for (float x = startX; x < ctx.viewportSize.x; x += cellSize) {
        drawList->AddLine(
            ImVec2(ctx.viewportPos.x + x, ctx.viewportPos.y),
            ImVec2(ctx.viewportPos.x + x, ctx.viewportPos.y + ctx.viewportSize.y),
            gridColor);
    }
    for (float y = startY; y < ctx.viewportSize.y; y += cellSize) {
        drawList->AddLine(
            ImVec2(ctx.viewportPos.x, ctx.viewportPos.y + y),
            ImVec2(ctx.viewportPos.x + ctx.viewportSize.x, ctx.viewportPos.y + y),
            gridColor);
    }
    
    // Origin
    float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
    float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
    drawList->AddCircleFilled(ImVec2(originX, originY), 5.0f, IM_COL32(255, 100, 100, 255));
    drawList->AddText(ImVec2(originX + 8, originY - 8), IM_COL32(255, 100, 100, 200), "(0,0)");
    
    // Render scenes
    for (const auto& scene : scenes) {
        const auto& gridPos = scene->getGridPosition();
        
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
        
        bool isSelected = (m_selectedScene == scene.get());
        bool isDragging = (m_draggedScene == scene.get());
        
        ImU32 roomColor = isDragging ? IM_COL32(80, 120, 160, 180) : 
                          isSelected ? IM_COL32(70, 100, 140, 220) : 
                                       IM_COL32(60, 80, 110, 200);
        ImU32 borderColor = isDragging ? IM_COL32(100, 200, 255, 255) :
                            isSelected ? IM_COL32(100, 180, 255, 255) :
                                         IM_COL32(80, 120, 160, 255);
        
        drawList->AddRectFilled(ImVec2(sceneX, sceneY), ImVec2(sceneX + sceneW, sceneY + sceneH),
                               roomColor, 4.0f);
        drawList->AddRect(ImVec2(sceneX, sceneY), ImVec2(sceneX + sceneW, sceneY + sceneH),
                         borderColor, 4.0f, 0, isSelected || isDragging ? 3.0f : 2.0f);
        
        // Scene name
        drawList->AddText(ImVec2(sceneX + 4, sceneY + 4), IM_COL32(255, 255, 255, 255), 
                         scene->getName().c_str());
        
        // Grid position
        char posInfo[32];
        snprintf(posInfo, sizeof(posInfo), "(%d,%d)", gridPos.gridX, gridPos.gridY);
        drawList->AddText(ImVec2(sceneX + 4, sceneY + sceneH - 16), IM_COL32(180, 180, 180, 200), posInfo);
        
        // Size info
        char sizeInfo[32];
        snprintf(sizeInfo, sizeof(sizeInfo), "%dx%d", gridPos.pixelWidth, gridPos.pixelHeight);
        ImVec2 sizeTextSize = ImGui::CalcTextSize(sizeInfo);
        drawList->AddText(ImVec2(sceneX + sceneW - sizeTextSize.x - 4, sceneY + 4), 
                         IM_COL32(150, 150, 150, 200), sizeInfo);
    }
    
    // Border
    drawList->AddRect(
        ImVec2(ctx.viewportPos.x, ctx.viewportPos.y),
        ImVec2(ctx.viewportPos.x + ctx.viewportSize.x, ctx.viewportPos.y + ctx.viewportSize.y),
        IM_COL32(60, 60, 70, 255));
}

void ViewportLevelRenderer::renderGridView(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    const auto& scenes = m_level->getScenes();
    
    float panelWidth = ctx.viewportSize.x;
    int columns = std::max(1, (int)(panelWidth / 200.0f));
    float cardW = 180.0f;
    float cardH = 150.0f;
    float spacing = 10.0f;
    
    int col = 0;
    int row = 0;
    float startX = ctx.viewportPos.x + 10.0f;
    float startY = ctx.viewportPos.y + 10.0f;
    
    for (const auto& scene : scenes) {
        float cardX = startX + col * (cardW + spacing);
        float cardY = startY + row * (cardH + spacing);
        
        bool isSelected = (m_selectedScene == scene.get());
        
        ImU32 bgColor = isSelected ? IM_COL32(60, 80, 100, 255) : IM_COL32(50, 60, 70, 255);
        drawList->AddRectFilled(ImVec2(cardX, cardY), ImVec2(cardX + cardW, cardY + cardH), bgColor, 4.0f);
        
        // Preview area
        drawList->AddRectFilled(ImVec2(cardX + 5, cardY + 5), ImVec2(cardX + cardW - 5, cardY + 105),
                               IM_COL32(30, 35, 40, 255));
        
        // Scene name
        drawList->AddText(ImVec2(cardX + 5, cardY + 110), IM_COL32(255, 255, 255, 255), 
                         scene->getName().c_str());
        
        // Actor count
        char info[32];
        snprintf(info, sizeof(info), "%zu actors", scene->getActors().size());
        drawList->AddText(ImVec2(cardX + 5, cardY + 125), IM_COL32(150, 150, 150, 255), info);
        
        if (isSelected) {
            drawList->AddRect(ImVec2(cardX, cardY), ImVec2(cardX + cardW, cardY + cardH),
                             IM_COL32(100, 200, 100, 255), 4.0f, 0, 2.0f);
        }
        
        col++;
        if (col >= columns) {
            col = 0;
            row++;
        }
    }
}

bool ViewportLevelRenderer::onMouseClick(float x, float y, const ViewportRenderContext& ctx) {
    m_selectedScene = findSceneAt(x, y, ctx);
    
    if (m_selectedScene) {
        m_draggedScene = m_selectedScene;
        
        const auto& gridPos = m_selectedScene->getGridPosition();
        float cellSize = 64.0f * ctx.zoom;
        float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
        float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
        float sceneX = originX + gridPos.gridX * cellSize;
        float sceneY = originY + gridPos.gridY * cellSize;
        
        m_dragStartX = x - sceneX;
        m_dragStartY = y - sceneY;
        m_dragStartGridX = gridPos.gridX;
        m_dragStartGridY = gridPos.gridY;
        
        return true;
    }
    
    return false;
}

bool ViewportLevelRenderer::onMouseDrag(float x, float y, float deltaX, float deltaY,
                                         const ViewportRenderContext& ctx) {
    return m_draggedScene != nullptr;
}

void ViewportLevelRenderer::onMouseRelease(float x, float y, const ViewportRenderContext& ctx) {
    if (m_draggedScene) {
        float cellSize = 64.0f * ctx.zoom;
        float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
        float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
        
        float sceneX = x - m_dragStartX;
        float sceneY = y - m_dragStartY;
        
        int newGridX, newGridY;
        if (m_snapToGrid) {
            newGridX = (int)round((sceneX - originX) / cellSize);
            newGridY = (int)round((sceneY - originY) / cellSize);
        } else {
            newGridX = (int)((sceneX - originX) / cellSize);
            newGridY = (int)((sceneY - originY) / cellSize);
        }
        
        if (newGridX != m_dragStartGridX || newGridY != m_dragStartGridY) {
            m_draggedScene->setGridPosition(newGridX, newGridY,
                m_draggedScene->getWidth(), m_draggedScene->getHeight());
        }
        
        m_draggedScene = nullptr;
    }
}

bool ViewportLevelRenderer::onDoubleClick(float x, float y, const ViewportRenderContext& ctx) {
    engine::Scene* scene = findSceneAt(x, y, ctx);
    if (scene) {
        m_selectedScene = scene;
        return true;  // Signal navigation
    }
    return false;
}

void ViewportLevelRenderer::onScroll(float delta, const ViewportRenderContext& ctx) {
}

void ViewportLevelRenderer::clearSelection() {
    m_selectedScene = nullptr;
    m_draggedScene = nullptr;
}

engine::Scene* ViewportLevelRenderer::findSceneAt(float x, float y, const ViewportRenderContext& ctx) {
    if (!m_level) return nullptr;
    
    const auto& scenes = m_level->getScenes();
    float cellSize = 64.0f * ctx.zoom;
    float originX = ctx.viewportPos.x + ctx.viewportSize.x * 0.5f + ctx.panX;
    float originY = ctx.viewportPos.y + ctx.viewportSize.y * 0.5f + ctx.panY;
    
    for (const auto& scene : scenes) {
        const auto& gridPos = scene->getGridPosition();
        float sceneX = originX + gridPos.gridX * cellSize;
        float sceneY = originY + gridPos.gridY * cellSize;
        float sceneW = (gridPos.pixelWidth / 64.0f) * cellSize;
        float sceneH = (gridPos.pixelHeight / 64.0f) * cellSize;
        
        if (x >= sceneX && x <= sceneX + sceneW && y >= sceneY && y <= sceneY + sceneH) {
            return scene.get();
        }
    }
    
    return nullptr;
}

} // namespace editor

#endif // HAS_IMGUI
