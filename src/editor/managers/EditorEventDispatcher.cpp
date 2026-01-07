/**
 * @file EditorEventDispatcher.cpp
 * @brief Event dispatcher implementation
 */
#include "EditorEventDispatcher.h"
#include "editor/core/EditorContext.h"
#include "editor/input/EditorInputHandler.h"
#include "editor/data/EditorDataManager.h"
#include "engine/audio/AudioManager.h"
#include "engine/utils/Logger.h"

#ifdef HAS_IMGUI
#include "editor/core/ImGuiManager.h"
#include <imgui.h>
#endif

EditorEventDispatcher::EditorEventDispatcher(EditorContext& context)
    : m_context(context) {
}

bool EditorEventDispatcher::handleEvent(const SDL_Event& event) {
#ifdef HAS_IMGUI
    // Let ImGui process events first
    ImGuiManager::instance().processEvent(event);
    
    // Handle global shortcuts via EditorInputHandler BEFORE ImGui capture check
    if (event.type == SDL_KEYDOWN && m_inputHandler) {
        // Ctrl+S to save (global - works even when ImGui has focus)
        if (event.key.keysym.scancode == SDL_SCANCODE_S && (event.key.keysym.mod & KMOD_CTRL)) {
            LOG_INFO("Ctrl+S pressed - saving...");
            m_context.saveToFiles();
            m_context.isDirty = false;
            AudioManager::instance().playSound("select");
            return true;
        }
        
        // Delete, Ctrl+Z, Ctrl+Y, Ctrl+D - forward to input handler
        if (event.key.keysym.scancode == SDL_SCANCODE_DELETE ||
            (event.key.keysym.scancode == SDL_SCANCODE_Z && (event.key.keysym.mod & KMOD_CTRL)) ||
            (event.key.keysym.scancode == SDL_SCANCODE_Y && (event.key.keysym.mod & KMOD_CTRL)) ||
            (event.key.keysym.scancode == SDL_SCANCODE_D && (event.key.keysym.mod & KMOD_CTRL))) {
            if (m_inputHandler->handleEvent(event)) {
                return true;
            }
        }
    }
    
    // If ImGui wants input, skip rest of handling
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
        return false;
    }
#endif
    
    // Handle other events
    if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_M:
                AudioManager::instance().toggleMute();
                return true;
            default:
                break;
        }
    }
    
    return false;
}
