/**
 * @file ScaleTool.cpp
 * @brief Implementation of ScaleTool
 */
#include "ScaleTool.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/world/Scene.h"
#include "engine/components/SpriteComponent.h"
#include <cmath>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

int ScaleTool::getHandleAtPosition(float roomX, float roomY, const ToolContext& ctx) {
    if (!ctx.selectedActor) return -1;
    
    engine::Vec2 pos = ctx.selectedActor->getPosition();
    float ax = pos.x;
    float ay = pos.y;
    float aw = 64.0f;
    float ah = 64.0f;
    
    auto* sprite = ctx.selectedActor->getComponent<engine::SpriteComponent>();
    if (sprite) {
        aw = sprite->getWidth();
        ah = sprite->getHeight();
    }
    
    float handleRadius = 8.0f;
    float corners[4][2] = {
        {ax, ay},           // TL
        {ax + aw, ay},      // TR
        {ax + aw, ay + ah}, // BR
        {ax, ay + ah}       // BL
    };
    
    for (int i = 0; i < 4; i++) {
        float dx = roomX - corners[i][0];
        float dy = roomY - corners[i][1];
        if (std::sqrt(dx*dx + dy*dy) < handleRadius) {
            return i;
        }
    }
    return -1;
}

bool ScaleTool::onMouseDown(float roomX, float roomY, ToolContext& ctx) {
    if (!ctx.selectedActor) return false;
    
    int handle = getHandleAtPosition(roomX, roomY, ctx);
    if (handle >= 0) {
        m_isScaling = true;
        m_scaleHandle = handle;
        m_scaleStartX = roomX;
        m_scaleStartY = roomY;
        
        auto* sprite = ctx.selectedActor->getComponent<engine::SpriteComponent>();
        if (sprite) {
            m_originalWidth = sprite->getWidth();
            m_originalHeight = sprite->getHeight();
        } else {
            m_originalWidth = 64.0f;
            m_originalHeight = 64.0f;
        }
        return true;
    }
    return false;
}

bool ScaleTool::onMouseUp(float roomX, float roomY, ToolContext& ctx) {
    m_isScaling = false;
    m_scaleHandle = -1;
    return false;
}

bool ScaleTool::onMouseDrag(float roomX, float roomY, float deltaX, float deltaY, ToolContext& ctx) {
    if (!m_isScaling || !ctx.selectedActor) return false;
    
    float dx = roomX - m_scaleStartX;
    float dy = roomY - m_scaleStartY;
    
    float newWidth = m_originalWidth;
    float newHeight = m_originalHeight;
    
    switch (m_scaleHandle) {
        case 0: // TL
            newWidth = std::max(16.0f, m_originalWidth - dx);
            newHeight = std::max(16.0f, m_originalHeight - dy);
            break;
        case 1: // TR
            newWidth = std::max(16.0f, m_originalWidth + dx);
            newHeight = std::max(16.0f, m_originalHeight - dy);
            break;
        case 2: // BR
            newWidth = std::max(16.0f, m_originalWidth + dx);
            newHeight = std::max(16.0f, m_originalHeight + dy);
            break;
        case 3: // BL
            newWidth = std::max(16.0f, m_originalWidth - dx);
            newHeight = std::max(16.0f, m_originalHeight + dy);
            break;
    }
    
    auto* sprite = ctx.selectedActor->getComponent<engine::SpriteComponent>();
    if (sprite) {
        sprite->setSize(static_cast<int>(newWidth), static_cast<int>(newHeight));
    }
    return true;
}

void ScaleTool::renderOverlay(ImDrawList* drawList, const ToolContext& ctx) {
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
    ImU32 selColor = m_isScaling ? IM_COL32(255, 150, 50, 255) : IM_COL32(255, 200, 100, 255);
    drawList->AddRect(ImVec2(ax - 2, ay - 2), ImVec2(ax + aw + 2, ay + ah + 2), selColor, 0, 0, 3.0f);
    
    // Scale handles (larger for scale tool)
    float handleSize = 10.0f;
    ImU32 handleColor = IM_COL32(255, 200, 50, 255);
    ImU32 activeColor = IM_COL32(255, 100, 50, 255);
    
    auto drawHandle = [&](int idx, float hx, float hy) {
        ImU32 col = (m_isScaling && m_scaleHandle == idx) ? activeColor : handleColor;
        drawList->AddRectFilled(
            ImVec2(hx - handleSize/2, hy - handleSize/2),
            ImVec2(hx + handleSize/2, hy + handleSize/2), col);
        drawList->AddRect(
            ImVec2(hx - handleSize/2, hy - handleSize/2),
            ImVec2(hx + handleSize/2, hy + handleSize/2), IM_COL32(0, 0, 0, 255));
    };
    
    drawHandle(0, ax, ay);
    drawHandle(1, ax + aw, ay);
    drawHandle(2, ax + aw, ay + ah);
    drawHandle(3, ax, ay + ah);
#endif
}
