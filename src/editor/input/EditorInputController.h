/**
 * @file EditorInputController.h
 * @brief Centralized editor input controller (singleton)
 */
#pragma once

#include "IInputMode.h"
#include "InputState.h"
#include <memory>
#include <vector>
#include <functional>
#include <string>

#ifdef HAS_IMGUI

namespace editor {

class IViewportRenderer;
class EditorCamera3D;
class GizmoInputMode;

/**
 * @brief Centralized editor input controller
 * 
 * Manages all editor input through a mode stack.
 * Each mode handles specific input (camera, selection, gizmo, etc.)
 */
class EditorInputController {
public:
    static EditorInputController& instance();
    
    /**
     * @brief Initialize the controller
     */
    void initialize();
    
    /**
     * @brief Shutdown and cleanup
     */
    void shutdown();
    
    /**
     * @brief Begin frame - reset per-frame state
     */
    void beginFrame();
    
    /**
     * @brief Process SDL event
     */
    void processSDLEvent(const SDL_Event& event);
    
    /**
     * @brief Process input for current viewport
     * @param renderer Current viewport renderer
     * @param viewportPos Top-left of viewport
     * @param viewportSize Size of viewport
     * @param isHovered Is mouse over viewport
     * @param isFocused Is viewport focused
     * @param deltaTime Frame delta time
     */
    void processViewportInput(IViewportRenderer* renderer,
                              const glm::vec2& viewportPos,
                              const glm::vec2& viewportSize,
                              bool isHovered,
                              bool isFocused,
                              float deltaTime);
    
    // Mode management
    
    /**
     * @brief Push a new input mode onto the stack
     */
    void pushMode(std::unique_ptr<IInputMode> mode);
    
    /**
     * @brief Pop the current mode from stack
     */
    void popMode();
    
    /**
     * @brief Get current active mode
     */
    IInputMode* getCurrentMode() const;
    
    /**
     * @brief Clear all modes
     */
    void clearModes();
    
    // State access
    
    /**
     * @brief Get current input state
     */
    const InputState& getState() const { return m_state; }
    
    /**
     * @brief Get current viewport renderer
     */
    IViewportRenderer* getCurrentRenderer() const { return m_currentRenderer; }
    
    /**
     * @brief Get gizmo input mode (for syncing gizmo settings)
     */
    GizmoInputMode* getGizmoMode() const;
    
    // Click state (for external queries)
    bool wasClicked() const { return m_clicked; }
    bool wasDoubleClicked() const { return m_doubleClicked; }
    void clearClickState() { m_clicked = false; m_doubleClicked = false; }
    
private:
    EditorInputController() = default;
    ~EditorInputController() = default;
    EditorInputController(const EditorInputController&) = delete;
    EditorInputController& operator=(const EditorInputController&) = delete;
    
    void updateInputContext(const glm::vec2& viewportPos,
                           const glm::vec2& viewportSize,
                           bool isHovered,
                           bool isFocused,
                           float deltaTime);
    
    InputState m_state;
    InputContext m_context;
    std::vector<std::unique_ptr<IInputMode>> m_modeStack;
    
    IViewportRenderer* m_currentRenderer = nullptr;
    
    bool m_clicked = false;
    bool m_doubleClicked = false;
    bool m_initialized = false;
};

} // namespace editor

#endif // HAS_IMGUI
