/**
 * @file EditorInputHandler.h
 * @brief Centralized input handling for the editor (Windows-style shortcuts)
 */
#pragma once

#include <SDL.h>
#include <functional>
#include <string>
#include <unordered_map>

class EditorContext;
class ViewportPanel;

/**
 * @brief Shortcut action identifiers
 */
enum class EditorAction {
    None,
    // File operations
    Save,           // Ctrl+S
    SaveAs,         // Ctrl+Shift+S
    Open,           // Ctrl+O
    New,            // Ctrl+N
    
    // Edit operations
    Undo,           // Ctrl+Z
    Redo,           // Ctrl+Y or Ctrl+Shift+Z
    Cut,            // Ctrl+X
    Copy,           // Ctrl+C
    Paste,          // Ctrl+V
    Delete,         // Del
    SelectAll,      // Ctrl+A
    Duplicate,      // Ctrl+D
    
    // View operations
    ZoomIn,         // Ctrl++
    ZoomOut,        // Ctrl+-
    ZoomReset,      // Ctrl+0
    ToggleGrid,     // G
    
    // Tool shortcuts
    SelectTool,     // W
    RotateTool,     // E
    ScaleTool,      // R
    
    // Navigation
    Escape,         // Esc
    
    // Debug
    ToggleMute      // M
};

using ActionCallback = std::function<void()>;

/**
 * @brief Centralized editor input handler
 */
class EditorInputHandler {
public:
    explicit EditorInputHandler(EditorContext& context);
    
    void setViewportPanel(ViewportPanel* viewport) { m_viewportPanel = viewport; }
    
    /**
     * @brief Process SDL event and trigger actions
     * @return true if event was handled
     */
    bool handleEvent(const SDL_Event& event);
    
    /**
     * @brief Register callback for an action
     */
    void registerAction(EditorAction action, ActionCallback callback);
    
    /**
     * @brief Check if a modifier key is pressed
     */
    static bool isCtrlPressed(const SDL_Event& event);
    static bool isShiftPressed(const SDL_Event& event);
    static bool isAltPressed(const SDL_Event& event);
    
private:
    EditorAction getActionFromEvent(const SDL_Event& event);
    void executeAction(EditorAction action);
    
    EditorContext& m_context;
    ViewportPanel* m_viewportPanel = nullptr;
    std::unordered_map<EditorAction, ActionCallback> m_callbacks;
};
