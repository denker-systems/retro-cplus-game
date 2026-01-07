/**
 * @file SelectTool.h
 * @brief Tool for selecting actors in the viewport
 */
#pragma once

#include "IEditorTool.h"

class SelectTool : public IEditorTool {
public:
    const char* getName() const override { return "Select"; }
    const char* getIcon() const override { return "S"; }
    const char* getTooltip() const override { return "Select actors (S)"; }
    
    bool onMouseDown(float roomX, float roomY, ToolContext& ctx) override;
    bool onMouseUp(float roomX, float roomY, ToolContext& ctx) override;
    bool onMouseDrag(float roomX, float roomY, float deltaX, float deltaY, ToolContext& ctx) override;
    
    void renderOverlay(ImDrawList* drawList, const ToolContext& ctx) override;
    
private:
    bool m_isDragging = false;
    float m_dragOffsetX = 0.0f;
    float m_dragOffsetY = 0.0f;
    engine::ActorObjectExtended* m_draggedActor = nullptr;
};
