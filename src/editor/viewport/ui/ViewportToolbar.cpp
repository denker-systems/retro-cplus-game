/**
 * @file ViewportToolbar.cpp
 * @brief Implementation of viewport toolbar
 */
#include "ViewportToolbar.h"
#include "editor/core/EditorPlayMode.h"
#include "editor/viewport/3d/EditorCamera3D.h"
#include "editor/tools/ToolManager.h"

#ifdef HAS_IMGUI
#include <imgui.h>
#include <algorithm>

namespace editor {
namespace viewport {

ViewportToolbar::ViewportToolbar() = default;

void ViewportToolbar::render(RenderMode& renderMode, float& zoom, float& panX, float& panY, editor::EditorPlayMode* playMode, editor::EditorCamera3D* camera) {
    renderModeToggle(renderMode, camera);
    
    ImGui::SameLine();
    ImGui::Separator();
    ImGui::SameLine();
    
    renderZoomControls(zoom, panX, panY);
    
    ImGui::SameLine();
    renderViewToggles();
    
    if (playMode) {
        ImGui::SameLine();
        ImGui::Separator();
        ImGui::SameLine();
        renderPlayControls(playMode);
    }
    
    ImGui::Separator();
}

void ViewportToolbar::renderModeToggle(RenderMode& renderMode, editor::EditorCamera3D* camera) {
    // Determine active mode from camera projection (if available)
    bool is2DMode = (camera && camera->getProjectionMode() == ProjectionMode::Orthographic);
    bool is3DMode = !is2DMode;
    
    // 2D Button
    ImGui::PushStyleColor(ImGuiCol_Button, is2DMode ? 
        ImVec4(0.2f, 0.5f, 0.8f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("2D")) {
        renderMode = RenderMode::Mode2D;
        if (camera) {
            camera->setProjectionMode(ProjectionMode::Orthographic);
            camera->setOrthoSize(20.0f);  // Good default for 2D view
        }
    }
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    
    // 3D Button
    ImGui::PushStyleColor(ImGuiCol_Button, is3DMode ? 
        ImVec4(0.2f, 0.5f, 0.8f, 1.0f) : ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    if (ImGui::Button("3D")) {
        renderMode = RenderMode::Mode3D;
        if (camera) {
            camera->setProjectionMode(ProjectionMode::Perspective);
        }
    }
    ImGui::PopStyleColor();
}

void ViewportToolbar::renderZoomControls(float& zoom, float& panX, float& panY) {
    ImGui::Text("Zoom: %.0f%%", zoom * 100);
    ImGui::SameLine();
    if (ImGui::Button("-")) zoom = std::max(0.25f, zoom - 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("+")) zoom = std::min(4.0f, zoom + 0.25f);
    ImGui::SameLine();
    if (ImGui::Button("100%")) { zoom = 1.0f; panX = 0; panY = 0; }
}

void ViewportToolbar::renderViewToggles() {
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Hotspots", &m_showHotspots);
    ImGui::SameLine();
    ImGui::Checkbox("Walk Area", &m_showWalkArea);
    ImGui::SameLine();
    ImGui::Checkbox("Physics", &m_showPhysicsDebug);
}

void ViewportToolbar::renderPlayControls(editor::EditorPlayMode* playMode) {
    if (!playMode) return;
    
    bool isPlaying = playMode->isPlaying();
    bool isPaused = playMode->isPaused();
    bool isStopped = playMode->isStopped();
    
    // Play button
    if (!isPlaying) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    }
    if (ImGui::Button("Play")) {
        // NOTE: Scene is already set via ViewportPanel::setScene()
        // EditorPlayMode should have the active scene
        playMode->play();
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    // Pause button
    if (isPlaying) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.6f, 0.1f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    }
    if (ImGui::Button("Pause")) {
        playMode->pause();
    }
    ImGui::PopStyleColor();
    ImGui::SameLine();
    
    // Stop button
    if (!isStopped) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.7f, 0.2f, 0.2f, 1.0f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
    }
    if (ImGui::Button("Stop")) {
        playMode->stop();
    }
    ImGui::PopStyleColor();
    
    if (!isStopped) {
        ImGui::SameLine();
        ImGui::Text("%.1fs", playMode->getPlayTime());
    }
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
