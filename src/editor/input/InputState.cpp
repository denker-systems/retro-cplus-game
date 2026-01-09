/**
 * @file InputState.cpp
 * @brief Centralized input state implementation
 */
#include "InputState.h"

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {

void InputState::beginFrame() {
    // Reset per-frame state
    m_mouseDelta = glm::vec2(0.0f);
    m_mouseWheel = 0.0f;
    m_mouseClicked.fill(false);
    m_mouseReleased.fill(false);
    m_mouseDoubleClicked.fill(false);
    m_keyPressed.fill(false);
    m_keyReleased.fill(false);
}

void InputState::updateFromImGui() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Update mouse position
    m_lastMousePos = m_mousePos;
    m_mousePos = glm::vec2(io.MousePos.x, io.MousePos.y);
    m_mouseDelta = m_mousePos - m_lastMousePos;
    m_mouseWheel = io.MouseWheel;
    
    // Update mouse buttons
    for (int i = 0; i < static_cast<int>(MouseButton::Count); i++) {
        bool wasDown = m_mouseDown[i];
        m_mouseDown[i] = io.MouseDown[i];
        m_mouseClicked[i] = !wasDown && m_mouseDown[i];
        m_mouseReleased[i] = wasDown && !m_mouseDown[i];
        m_mouseDoubleClicked[i] = ImGui::IsMouseDoubleClicked(i);
    }
    
    // Update modifiers
    m_modifiers = Modifier::None;
    if (io.KeyCtrl) m_modifiers = m_modifiers | Modifier::Ctrl;
    if (io.KeyShift) m_modifiers = m_modifiers | Modifier::Shift;
    if (io.KeyAlt) m_modifiers = m_modifiers | Modifier::Alt;
}

void InputState::processSDLEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (event.key.keysym.scancode < SDL_NUM_SCANCODES) {
                if (!m_keyDown[event.key.keysym.scancode]) {
                    m_keyPressed[event.key.keysym.scancode] = true;
                }
                m_keyDown[event.key.keysym.scancode] = true;
            }
            break;
            
        case SDL_KEYUP:
            if (event.key.keysym.scancode < SDL_NUM_SCANCODES) {
                m_keyDown[event.key.keysym.scancode] = false;
                m_keyReleased[event.key.keysym.scancode] = true;
            }
            break;
    }
}

bool InputState::isMouseDown(MouseButton button) const {
    return m_mouseDown[static_cast<size_t>(button)];
}

bool InputState::isMouseClicked(MouseButton button) const {
    return m_mouseClicked[static_cast<size_t>(button)];
}

bool InputState::isMouseReleased(MouseButton button) const {
    return m_mouseReleased[static_cast<size_t>(button)];
}

bool InputState::isMouseDoubleClicked(MouseButton button) const {
    return m_mouseDoubleClicked[static_cast<size_t>(button)];
}

bool InputState::isKeyDown(SDL_Scancode key) const {
    if (key >= SDL_NUM_SCANCODES) return false;
    return m_keyDown[key];
}

bool InputState::isKeyPressed(SDL_Scancode key) const {
    if (key >= SDL_NUM_SCANCODES) return false;
    return m_keyPressed[key];
}

bool InputState::isKeyReleased(SDL_Scancode key) const {
    if (key >= SDL_NUM_SCANCODES) return false;
    return m_keyReleased[key];
}

bool InputState::isModifierDown(Modifier mod) const {
    return m_modifiers & mod;
}

} // namespace editor

#endif // HAS_IMGUI
