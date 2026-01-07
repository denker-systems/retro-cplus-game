/**
 * @file EditorInputHandler.cpp
 * @brief Centralized input handling implementation
 */
#include "EditorInputHandler.h"
#include "editor/core/EditorContext.h"
#include "editor/core/CommandManager.h"
#include "editor/panels/viewport/ViewportPanel.h"
#include "engine/utils/Logger.h"

EditorInputHandler::EditorInputHandler(EditorContext& context)
    : m_context(context) {
}

bool EditorInputHandler::isCtrlPressed(const SDL_Event& event) {
    return (event.key.keysym.mod & KMOD_CTRL) != 0;
}

bool EditorInputHandler::isShiftPressed(const SDL_Event& event) {
    return (event.key.keysym.mod & KMOD_SHIFT) != 0;
}

bool EditorInputHandler::isAltPressed(const SDL_Event& event) {
    return (event.key.keysym.mod & KMOD_ALT) != 0;
}

EditorAction EditorInputHandler::getActionFromEvent(const SDL_Event& event) {
    if (event.type != SDL_KEYDOWN) {
        return EditorAction::None;
    }
    
    bool ctrl = isCtrlPressed(event);
    bool shift = isShiftPressed(event);
    SDL_Scancode key = event.key.keysym.scancode;
    
    // Ctrl + key combinations
    if (ctrl) {
        switch (key) {
            case SDL_SCANCODE_S:
                return shift ? EditorAction::SaveAs : EditorAction::Save;
            case SDL_SCANCODE_Z:
                return shift ? EditorAction::Redo : EditorAction::Undo;
            case SDL_SCANCODE_Y:
                return EditorAction::Redo;
            case SDL_SCANCODE_X:
                return EditorAction::Cut;
            case SDL_SCANCODE_C:
                return EditorAction::Copy;
            case SDL_SCANCODE_V:
                return EditorAction::Paste;
            case SDL_SCANCODE_A:
                return EditorAction::SelectAll;
            case SDL_SCANCODE_D:
                return EditorAction::Duplicate;
            case SDL_SCANCODE_O:
                return EditorAction::Open;
            case SDL_SCANCODE_N:
                return EditorAction::New;
            case SDL_SCANCODE_EQUALS:
            case SDL_SCANCODE_KP_PLUS:
                return EditorAction::ZoomIn;
            case SDL_SCANCODE_MINUS:
            case SDL_SCANCODE_KP_MINUS:
                return EditorAction::ZoomOut;
            case SDL_SCANCODE_0:
            case SDL_SCANCODE_KP_0:
                return EditorAction::ZoomReset;
            default:
                break;
        }
    }
    
    // Single key shortcuts
    switch (key) {
        case SDL_SCANCODE_DELETE:
            return EditorAction::Delete;
        case SDL_SCANCODE_ESCAPE:
            return EditorAction::Escape;
        case SDL_SCANCODE_G:
            return EditorAction::ToggleGrid;
        case SDL_SCANCODE_W:
            return EditorAction::SelectTool;
        case SDL_SCANCODE_E:
            return EditorAction::RotateTool;
        case SDL_SCANCODE_R:
            return EditorAction::ScaleTool;
        case SDL_SCANCODE_M:
            return EditorAction::ToggleMute;
        default:
            break;
    }
    
    return EditorAction::None;
}

void EditorInputHandler::registerAction(EditorAction action, ActionCallback callback) {
    m_callbacks[action] = callback;
}

void EditorInputHandler::executeAction(EditorAction action) {
    auto it = m_callbacks.find(action);
    if (it != m_callbacks.end() && it->second) {
        it->second();
    }
}

bool EditorInputHandler::handleEvent(const SDL_Event& event) {
    EditorAction action = getActionFromEvent(event);
    
    if (action == EditorAction::None) {
        return false;
    }
    
    LOG_DEBUG("EditorInputHandler: Action triggered - " + std::to_string(static_cast<int>(action)));
    
    // Handle built-in actions
    switch (action) {
        case EditorAction::Save:
            if (m_callbacks.count(action)) {
                executeAction(action);
            }
            return true;
            
        case EditorAction::Undo:
            if (CommandManager::instance().canUndo()) {
                CommandManager::instance().undo();
                LOG_DEBUG("EditorInputHandler: Undo performed");
            }
            executeAction(action);
            return true;
            
        case EditorAction::Redo:
            if (CommandManager::instance().canRedo()) {
                CommandManager::instance().redo();
                LOG_DEBUG("EditorInputHandler: Redo performed");
            }
            executeAction(action);
            return true;
            
        case EditorAction::Delete:
            if (m_viewportPanel) {
                m_viewportPanel->deleteSelectedActor();
            }
            executeAction(action);
            return true;
            
        case EditorAction::Duplicate:
            if (m_viewportPanel) {
                m_viewportPanel->duplicateSelectedActor();
            }
            executeAction(action);
            return true;
            
        default:
            executeAction(action);
            return true;
    }
    
    return false;
}
