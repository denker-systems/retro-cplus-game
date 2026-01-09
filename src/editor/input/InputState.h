/**
 * @file InputState.h
 * @brief Centralized input state tracking
 */
#pragma once

#include <SDL.h>
#include <array>

#ifdef HAS_IMGUI
#include <glm/glm.hpp>

namespace editor {

/**
 * @brief Modifier key flags
 */
enum class Modifier {
    None = 0,
    Ctrl = 1 << 0,
    Shift = 1 << 1,
    Alt = 1 << 2
};

inline Modifier operator|(Modifier a, Modifier b) {
    return static_cast<Modifier>(static_cast<int>(a) | static_cast<int>(b));
}

inline bool operator&(Modifier a, Modifier b) {
    return (static_cast<int>(a) & static_cast<int>(b)) != 0;
}

/**
 * @brief Mouse button identifiers
 */
enum class MouseButton {
    Left = 0,
    Right = 1,
    Middle = 2,
    X1 = 3,
    X2 = 4,
    Count = 5
};

/**
 * @brief Centralized input state
 */
class InputState {
public:
    InputState() = default;
    
    /**
     * @brief Update state from ImGui IO
     */
    void updateFromImGui();
    
    /**
     * @brief Update state from SDL event
     */
    void processSDLEvent(const SDL_Event& event);
    
    /**
     * @brief Reset per-frame state (call at start of frame)
     */
    void beginFrame();
    
    // Mouse queries
    glm::vec2 getMousePos() const { return m_mousePos; }
    glm::vec2 getMouseDelta() const { return m_mouseDelta; }
    float getMouseWheel() const { return m_mouseWheel; }
    
    bool isMouseDown(MouseButton button) const;
    bool isMouseClicked(MouseButton button) const;
    bool isMouseReleased(MouseButton button) const;
    bool isMouseDoubleClicked(MouseButton button) const;
    
    // Keyboard queries
    bool isKeyDown(SDL_Scancode key) const;
    bool isKeyPressed(SDL_Scancode key) const;
    bool isKeyReleased(SDL_Scancode key) const;
    
    // Modifier queries
    bool isModifierDown(Modifier mod) const;
    bool isCtrlDown() const { return isModifierDown(Modifier::Ctrl); }
    bool isShiftDown() const { return isModifierDown(Modifier::Shift); }
    bool isAltDown() const { return isModifierDown(Modifier::Alt); }
    Modifier getCurrentModifiers() const { return m_modifiers; }

private:
    // Mouse state
    glm::vec2 m_mousePos{0.0f, 0.0f};
    glm::vec2 m_lastMousePos{0.0f, 0.0f};
    glm::vec2 m_mouseDelta{0.0f, 0.0f};
    float m_mouseWheel = 0.0f;
    
    std::array<bool, static_cast<size_t>(MouseButton::Count)> m_mouseDown{};
    std::array<bool, static_cast<size_t>(MouseButton::Count)> m_mouseClicked{};
    std::array<bool, static_cast<size_t>(MouseButton::Count)> m_mouseReleased{};
    std::array<bool, static_cast<size_t>(MouseButton::Count)> m_mouseDoubleClicked{};
    
    // Keyboard state
    std::array<bool, SDL_NUM_SCANCODES> m_keyDown{};
    std::array<bool, SDL_NUM_SCANCODES> m_keyPressed{};
    std::array<bool, SDL_NUM_SCANCODES> m_keyReleased{};
    
    // Modifiers
    Modifier m_modifiers = Modifier::None;
};

} // namespace editor

#endif // HAS_IMGUI
