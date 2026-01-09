/**
 * @file GizmoInputMode.cpp
 * @brief Transform gizmo input mode implementation
 */
#include "GizmoInputMode.h"
#include "EditorInputController.h"

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {

void GizmoInputMode::onEnter() {
    m_snapping = false;
}

void GizmoInputMode::onExit() {
    m_snapping = false;
}

bool GizmoInputMode::processInput(const InputContext& ctx) {
    if (!ctx.isFocused) {
        return false;
    }
    
    // Update snapping state from Ctrl key
    m_snapping = ctx.ctrl;
    
    // Handle gizmo shortcuts
    handleGizmoShortcuts(ctx);
    
    return false;  // Don't consume - let other modes process
}

void GizmoInputMode::handleGizmoShortcuts(const InputContext& ctx) {
    // Only process if no text input is active
    if (ImGui::GetIO().WantTextInput) return;
    
    // W - Translate
    if (ImGui::IsKeyPressed(ImGuiKey_W) && !ctx.ctrl && !ctx.shift && !ctx.alt) {
        m_gizmoType = GizmoType::Translate;
    }
    
    // E - Rotate
    if (ImGui::IsKeyPressed(ImGuiKey_E) && !ctx.ctrl && !ctx.shift && !ctx.alt) {
        m_gizmoType = GizmoType::Rotate;
    }
    
    // R - Scale
    if (ImGui::IsKeyPressed(ImGuiKey_R) && !ctx.ctrl && !ctx.shift && !ctx.alt) {
        m_gizmoType = GizmoType::Scale;
    }
    
    // Space - Toggle transform space
    if (ImGui::IsKeyPressed(ImGuiKey_Space) && !ctx.ctrl && !ctx.shift && !ctx.alt) {
        m_transformSpace = (m_transformSpace == TransformSpace::Local) 
            ? TransformSpace::World 
            : TransformSpace::Local;
    }
}

} // namespace editor

#endif // HAS_IMGUI
