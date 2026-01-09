/**
 * @file SelectionInputMode.h
 * @brief Selection input mode for object picking
 */
#pragma once

#include "IInputMode.h"
#include <functional>

#ifdef HAS_IMGUI

namespace editor {

class IViewportRenderer;

/**
 * @brief Selection callback type
 */
using SelectionCallback = std::function<void(int index, bool addToSelection)>;

/**
 * @brief Selection input mode
 * 
 * Handles object selection:
 * - LMB Click: Select object
 * - Ctrl + LMB: Add to selection
 * - Shift + LMB: Toggle selection
 * - LMB Drag: Box select
 * - Escape: Deselect all
 * - Double-click: Drill down (handled by navigation)
 */
class SelectionInputMode : public IInputMode {
public:
    SelectionInputMode() = default;
    ~SelectionInputMode() override = default;
    
    const char* getName() const override { return "Selection"; }
    
    void onEnter() override;
    void onExit() override;
    
    bool processInput(const InputContext& ctx) override;
    
    // Callbacks
    void setSelectionCallback(SelectionCallback callback) { m_selectionCallback = callback; }
    void setDeselectionCallback(std::function<void()> callback) { m_deselectionCallback = callback; }
    
    // Box selection state
    bool isBoxSelecting() const { return m_isBoxSelecting; }
    glm::vec2 getBoxStart() const { return m_boxStart; }
    glm::vec2 getBoxEnd() const { return m_boxEnd; }

private:
    void handleClick(const InputContext& ctx);
    void handleBoxSelect(const InputContext& ctx);
    void handleDeselect(const InputContext& ctx);
    
    // State
    bool m_isBoxSelecting = false;
    glm::vec2 m_boxStart{0.0f, 0.0f};
    glm::vec2 m_boxEnd{0.0f, 0.0f};
    
    // Callbacks
    SelectionCallback m_selectionCallback;
    std::function<void()> m_deselectionCallback;
};

} // namespace editor

#endif // HAS_IMGUI
