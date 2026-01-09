/**
 * @file IInputMode.h
 * @brief Interface for editor input modes (Strategy Pattern)
 */
#pragma once

#include <string>

#ifdef HAS_IMGUI
#include <glm/glm.hpp>

namespace editor {

/**
 * @brief Input context passed to input modes
 */
struct InputContext {
    // Mouse state
    glm::vec2 mousePos{0.0f, 0.0f};
    glm::vec2 mouseDelta{0.0f, 0.0f};
    float mouseWheel = 0.0f;
    bool mouseButtons[5] = {false, false, false, false, false};  // Left, Right, Middle, X1, X2
    
    // Modifier keys
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    
    // Viewport info
    glm::vec2 viewportPos{0.0f, 0.0f};
    glm::vec2 viewportSize{0.0f, 0.0f};
    bool isHovered = false;
    bool isFocused = false;
    
    // Delta time
    float deltaTime = 0.0f;
};

/**
 * @brief Interface for input modes
 * 
 * Each mode handles a specific type of input (camera, selection, gizmo, etc.)
 */
class IInputMode {
public:
    virtual ~IInputMode() = default;
    
    /**
     * @brief Get the name of this input mode
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Called when this mode becomes active
     */
    virtual void onEnter() {}
    
    /**
     * @brief Called when this mode is deactivated
     */
    virtual void onExit() {}
    
    /**
     * @brief Process input for this mode
     * @param ctx Input context with current state
     * @return true if input was consumed
     */
    virtual bool processInput(const InputContext& ctx) = 0;
    
    /**
     * @brief Check if mode wants to capture mouse
     */
    virtual bool wantCaptureMouse() const { return false; }
    
    /**
     * @brief Check if mode wants to capture keyboard
     */
    virtual bool wantCaptureKeyboard() const { return false; }
};

} // namespace editor

#endif // HAS_IMGUI
