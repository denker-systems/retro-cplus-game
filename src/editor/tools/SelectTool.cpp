/**
 * @file SelectTool.cpp
 * @brief Implementation of SelectTool
 */
#include "SelectTool.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/world/Scene.h"
#include "engine/components/SpriteComponent.h"
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

bool SelectTool::onMouseDown(float roomX, float roomY, ToolContext& ctx) {
    if (!ctx.scene) return false;
    
    m_draggedActor = nullptr;
    ctx.selectedActor = nullptr;
    
    // Find actor under mouse
    for (auto& actor : ctx.scene->getActors()) {
        engine::Vec2 pos = actor->getPosition();
        float ax = pos.x;
        float ay = pos.y;
        float aw = 64.0f;
        float ah = 64.0f;
        
        auto* sprite = actor->getComponent<engine::SpriteComponent>();
        if (sprite) {
            aw = sprite->getWidth();
            ah = sprite->getHeight();
        }
        
        if (roomX >= ax && roomX <= ax + aw &&
            roomY >= ay && roomY <= ay + ah) {
            ctx.selectedActor = actor.get();
            m_draggedActor = actor.get();
            m_dragOffsetX = roomX - ax;
            m_dragOffsetY = roomY - ay;
            m_isDragging = true;
            return true;
        }
    }
    
    return false;
}

bool SelectTool::onMouseUp(float roomX, float roomY, ToolContext& ctx) {
    m_isDragging = false;
    m_draggedActor = nullptr;
    return false;
}

bool SelectTool::onMouseDrag(float roomX, float roomY, float deltaX, float deltaY, ToolContext& ctx) {
    if (!m_isDragging || !m_draggedActor) return false;
    
    float newX = roomX - m_dragOffsetX;
    float newY = roomY - m_dragOffsetY;
    
    // Snap to grid
    if (ctx.snapToGrid) {
        newX = round(newX / ctx.gridSize) * ctx.gridSize;
        newY = round(newY / ctx.gridSize) * ctx.gridSize;
    }
    
    m_draggedActor->setPosition(newX, newY);
    return true;
}

void SelectTool::renderOverlay(ImDrawList* drawList, const ToolContext& ctx) {
#ifdef HAS_IMGUI
    if (!ctx.selectedActor) return;
    
    engine::Vec2 pos = ctx.selectedActor->getPosition();
    float ax = ctx.viewportX + pos.x * ctx.zoom;
    float ay = ctx.viewportY + pos.y * ctx.zoom;
    float aw = 64.0f * ctx.zoom;
    float ah = 64.0f * ctx.zoom;
    
    auto* sprite = ctx.selectedActor->getComponent<engine::SpriteComponent>();
    if (sprite) {
        aw = sprite->getWidth() * ctx.zoom;
        ah = sprite->getHeight() * ctx.zoom;
    }
    
    // Selection border
    ImU32 selColor = m_isDragging ? IM_COL32(255, 200, 100, 255) : IM_COL32(100, 200, 255, 255);
    drawList->AddRect(ImVec2(ax - 2, ay - 2), ImVec2(ax + aw + 2, ay + ah + 2), selColor, 0, 0, 3.0f);
    
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
#endif
}
