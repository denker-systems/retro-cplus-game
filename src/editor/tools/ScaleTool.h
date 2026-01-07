/**
 * @file ScaleTool.h
 * @brief Tool for scaling actors in the viewport
 */
#pragma once

#include "IEditorTool.h"

class ScaleTool : public IEditorTool {
public:
    const char* getName() const override { return "Scale"; }
    const char* getIcon() const override { return "R"; }
    const char* getTooltip() const override { return "Scale actors (R)"; }
    
    bool onMouseDown(float roomX, float roomY, ToolContext& ctx) override;
    bool onMouseUp(float roomX, float roomY, ToolContext& ctx) override;
    bool onMouseDrag(float roomX, float roomY, float deltaX, float deltaY, ToolContext& ctx) override;
    
    void renderOverlay(ImDrawList* drawList, const ToolContext& ctx) override;
    
    // Check if mouse is over a scale handle
    int getHandleAtPosition(float roomX, float roomY, const ToolContext& ctx);
    
private:
    bool m_isScaling = false;
    int m_scaleHandle = -1;  // 0=TL, 1=TR, 2=BR, 3=BL
    float m_scaleStartX = 0.0f;
    float m_scaleStartY = 0.0f;
    float m_originalWidth = 0.0f;
    float m_originalHeight = 0.0f;
    float m_originalPosX = 0.0f;
    float m_originalPosY = 0.0f;
};
