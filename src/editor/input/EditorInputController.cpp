/**
 * @file EditorInputController.cpp
 * @brief Centralized editor input controller implementation
 */
#include "EditorInputController.h"
#include "CameraInputMode.h"
#include "SelectionInputMode.h"
#include "GizmoInputMode.h"
#include "KeybindingsConfig.h"

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {

EditorInputController& EditorInputController::instance() {
    static EditorInputController s_instance;
    return s_instance;
}

void EditorInputController::initialize() {
    if (m_initialized) return;
    
    // Load keybindings from config
    if (!KeybindingsConfig::instance().load("assets/data/keybindings.json")) {
        // Use defaults if file not found
        KeybindingsConfig::instance().resetToDefaults();
    }
    
    // Push modes in priority order (bottom to top)
    // Camera mode is always active for viewport navigation
    pushMode(std::make_unique<CameraInputMode>());
    
    // Selection mode for object picking
    pushMode(std::make_unique<SelectionInputMode>());
    
    // Gizmo mode for transform shortcuts (W/E/R)
    pushMode(std::make_unique<GizmoInputMode>());
    
    m_initialized = true;
}

void EditorInputController::shutdown() {
    clearModes();
    m_initialized = false;
}

void EditorInputController::beginFrame() {
    m_state.beginFrame();
    m_clicked = false;
    m_doubleClicked = false;
}

void EditorInputController::processSDLEvent(const SDL_Event& event) {
    m_state.processSDLEvent(event);
}

void EditorInputController::processViewportInput(IViewportRenderer* renderer,
                                                  const glm::vec2& viewportPos,
                                                  const glm::vec2& viewportSize,
                                                  bool isHovered,
                                                  bool isFocused,
                                                  float deltaTime) {
    m_currentRenderer = renderer;
    
    // Update state from ImGui
    m_state.updateFromImGui();
    
    // Update context
    updateInputContext(viewportPos, viewportSize, isHovered, isFocused, deltaTime);
    
    // Track clicks
    if (isHovered && m_state.isMouseClicked(MouseButton::Left)) {
        m_clicked = true;
    }
    if (isHovered && m_state.isMouseDoubleClicked(MouseButton::Left)) {
        m_doubleClicked = true;
    }
    
    // Process through mode stack (top-down)
    for (auto it = m_modeStack.rbegin(); it != m_modeStack.rend(); ++it) {
        if ((*it)->processInput(m_context)) {
            break;  // Input consumed
        }
    }
}

void EditorInputController::pushMode(std::unique_ptr<IInputMode> mode) {
    if (mode) {
        mode->onEnter();
        m_modeStack.push_back(std::move(mode));
    }
}

void EditorInputController::popMode() {
    if (!m_modeStack.empty()) {
        m_modeStack.back()->onExit();
        m_modeStack.pop_back();
    }
}

IInputMode* EditorInputController::getCurrentMode() const {
    return m_modeStack.empty() ? nullptr : m_modeStack.back().get();
}

void EditorInputController::clearModes() {
    while (!m_modeStack.empty()) {
        popMode();
    }
}

GizmoInputMode* EditorInputController::getGizmoMode() const {
    for (const auto& mode : m_modeStack) {
        if (auto* gizmo = dynamic_cast<GizmoInputMode*>(mode.get())) {
            return gizmo;
        }
    }
    return nullptr;
}

void EditorInputController::updateInputContext(const glm::vec2& viewportPos,
                                                const glm::vec2& viewportSize,
                                                bool isHovered,
                                                bool isFocused,
                                                float deltaTime) {
    m_context.mousePos = m_state.getMousePos();
    m_context.mouseDelta = m_state.getMouseDelta();
    m_context.mouseWheel = m_state.getMouseWheel();
    
    for (int i = 0; i < 5; i++) {
        m_context.mouseButtons[i] = m_state.isMouseDown(static_cast<MouseButton>(i));
    }
    
    m_context.ctrl = m_state.isCtrlDown();
    m_context.shift = m_state.isShiftDown();
    m_context.alt = m_state.isAltDown();
    
    m_context.viewportPos = viewportPos;
    m_context.viewportSize = viewportSize;
    m_context.isHovered = isHovered;
    m_context.isFocused = isFocused;
    m_context.deltaTime = deltaTime;
}

} // namespace editor

#endif // HAS_IMGUI
