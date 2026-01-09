/**
 * @file Base2DRenderer.cpp
 * @brief Implementation of 2D base renderer
 */
#include "Base2DRenderer.h"
#include <algorithm>
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

void Base2DRenderer::render(ImDrawList* drawList, const RenderContext& ctx) {
    if (!drawList) return;
    
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    
    ImGui::InvisibleButton("canvas", canvasSize,
        ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight | ImGuiButtonFlags_MouseButtonMiddle);
    
    bool hovered = ImGui::IsItemHovered();
    
    // Background
    drawList->AddRectFilled(canvasPos, ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                            IM_COL32(30, 30, 40, 255));
    
    // Render derived class content
    render2DContent(drawList, ctx);
    
    // Handle input
    if (hovered) {
        handle2DInput(ctx, hovered);
    }
}

void Base2DRenderer::handle2DInput(const RenderContext& ctx, bool hovered) {
    ImGuiIO& io = ImGui::GetIO();
    
    // Zoom
    if (io.MouseWheel != 0) {
        m_zoom *= (io.MouseWheel > 0) ? 1.1f : 0.9f;
        m_zoom = std::clamp(m_zoom, 0.25f, 4.0f);
    }
    
    // Pan with middle mouse
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        m_panX += io.MouseDelta.x;
        m_panY += io.MouseDelta.y;
    }
    
    // Double-click detection
    if (ImGui::IsMouseDoubleClicked(0)) {
        m_doubleClicked = true;
    }
}

void Base2DRenderer::drawGrid(const glm::vec2& canvasPos, const glm::vec2& canvasSize, 
                               float gridSize, float zoom, float panX, float panY) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    if (!drawList) return;
    
    float scaledGridSize = gridSize * zoom;
    
    for (float x = std::fmod(panX, scaledGridSize); x < canvasSize.x; x += scaledGridSize) {
        drawList->AddLine(ImVec2(canvasPos.x + x, canvasPos.y),
                         ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y),
                         IM_COL32(50, 50, 60, 255));
    }
    for (float y = std::fmod(panY, scaledGridSize); y < canvasSize.y; y += scaledGridSize) {
        drawList->AddLine(ImVec2(canvasPos.x, canvasPos.y + y),
                         ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y),
                         IM_COL32(50, 50, 60, 255));
    }
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
