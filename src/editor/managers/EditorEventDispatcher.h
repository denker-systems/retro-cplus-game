/**
 * @file EditorEventDispatcher.h
 * @brief Handles SDL events and dispatches to appropriate handlers
 */
#pragma once

#include <SDL.h>
#include <memory>

// Forward declarations
class EditorContext;
class EditorInputHandler;

/**
 * @brief Dispatches SDL events to appropriate handlers
 */
class EditorEventDispatcher {
public:
    explicit EditorEventDispatcher(EditorContext& context);
    ~EditorEventDispatcher() = default;
    
    void setInputHandler(EditorInputHandler* handler) { m_inputHandler = handler; }
    
    /**
     * @brief Handle SDL event
     * @return true if event was handled
     */
    bool handleEvent(const SDL_Event& event);
    
private:
    EditorContext& m_context;
    EditorInputHandler* m_inputHandler = nullptr;
};
