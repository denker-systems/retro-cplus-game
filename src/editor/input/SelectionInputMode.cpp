/**
 * @file SelectionInputMode.cpp
 * @brief Selection input mode implementation
 */
#include "SelectionInputMode.h"
#include "EditorInputController.h"
#include "editor/viewport/IViewportRenderer.h"

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {

void SelectionInputMode::onEnter() {
    m_isBoxSelecting = false;
}

void SelectionInputMode::onExit() {
    m_isBoxSelecting = false;
}

bool SelectionInputMode::processInput(const InputContext& ctx) {
    if (!ctx.isHovered && !m_isBoxSelecting) {
        return false;
    }
    
    bool consumed = false;
    
    // Handle click selection
    handleClick(ctx);
    
    // Handle box selection
    handleBoxSelect(ctx);
    
    // Handle deselection (Escape)
    handleDeselect(ctx);
    
    return consumed;
}

void SelectionInputMode::handleClick(const InputContext& ctx) {
    IViewportRenderer* renderer = EditorInputController::instance().getCurrentRenderer();
    if (!renderer) return;
    
    const InputState& state = EditorInputController::instance().getState();
    
    // LMB click - select
    if (state.isMouseClicked(MouseButton::Left) && ctx.isHovered) {
        // Selection is handled by the renderer's onMouseClick
        // Callbacks are triggered separately
        if (m_selectionCallback) {
            m_selectionCallback(0, ctx.ctrl);
        }
    }
}

void SelectionInputMode::handleBoxSelect(const InputContext& ctx) {
    const InputState& state = EditorInputController::instance().getState();
    
    // Start box select on LMB drag
    if (state.isMouseDown(MouseButton::Left) && !m_isBoxSelecting) {
        // Start box selection if dragging started in viewport
        if (ctx.isHovered && glm::length(ctx.mouseDelta) > 2.0f) {
            m_isBoxSelecting = true;
            m_boxStart = ctx.mousePos;
            m_boxEnd = ctx.mousePos;
        }
    }
    
    // Update box
    if (m_isBoxSelecting) {
        m_boxEnd = ctx.mousePos;
    }
    
    // End box select on LMB release
    if (!state.isMouseDown(MouseButton::Left) && m_isBoxSelecting) {
        m_isBoxSelecting = false;
        
        // TODO: Select all objects within box
        // This requires ViewportRenderer to support box selection query
    }
}

void SelectionInputMode::handleDeselect(const InputContext& ctx) {
    if (!ctx.isFocused) return;
    
    // Escape - deselect all
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        IViewportRenderer* renderer = EditorInputController::instance().getCurrentRenderer();
        if (renderer) {
            renderer->clearSelection();
        }
        if (m_deselectionCallback) {
            m_deselectionCallback();
        }
    }
}

} // namespace editor

#endif // HAS_IMGUI
