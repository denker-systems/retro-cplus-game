/**
 * @file ViewportInputHandler.cpp
 * @brief Implementation of input handling
 */
#include "ViewportInputHandler.h"
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

void ViewportInputHandler::processInput(RenderContext& ctx, bool hovered) {
    if (!hovered) return;
    
    handleZoom(ctx);
    handlePan(ctx);
    handleDoubleClick();
}

void ViewportInputHandler::handleZoom(RenderContext& ctx) {
    ImGuiIO& io = ImGui::GetIO();
    
    if (io.MouseWheel != 0) {
        ctx.zoom *= (io.MouseWheel > 0) ? 1.1f : 0.9f;
        ctx.zoom = std::clamp(ctx.zoom, 0.25f, 4.0f);
    }
}

void ViewportInputHandler::handlePan(RenderContext& ctx) {
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        ImGuiIO& io = ImGui::GetIO();
        ctx.panX += io.MouseDelta.x;
        ctx.panY += io.MouseDelta.y;
    }
}

void ViewportInputHandler::handleDoubleClick() {
    if (ImGui::IsMouseDoubleClicked(0)) {
        m_doubleClicked = true;
    }
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
