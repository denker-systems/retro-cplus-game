/**
 * @file EditorDockspace.h
 * @brief Dockspace management for the editor
 */
#pragma once

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

/**
 * @brief Manages the ImGui dockspace and default layout
 */
class EditorDockspace {
public:
    EditorDockspace() = default;
    
    // Setup and render dockspace, returns dockspace ID
    void render(float menuBarHeight, float screenW, float screenH);
    
    // Check if this is the first render (for layout setup)
    bool isFirstTime() const { return m_firstTime; }
    
private:
    void setupDefaultLayout(float screenW, float screenH, float menuBarHeight);
    
    bool m_firstTime = true;
#ifdef HAS_IMGUI
    ImGuiID m_dockspaceId = 0;
#endif
};
