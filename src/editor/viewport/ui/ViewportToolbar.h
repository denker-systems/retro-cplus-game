/**
 * @file ViewportToolbar.h
 * @brief Toolbar for viewport controls
 */
#pragma once

#include "../ViewportTypes.h"

// Forward declarations
class ToolManager;

namespace editor {
    class EditorPlayMode;
    class EditorCamera3D;
}
class ToolManager;

namespace editor {
namespace viewport {

/**
 * @class ViewportToolbar
 * @brief Renders toolbar with 2D/3D toggle, tools, zoom, and play controls
 */
class ViewportToolbar {
public:
    ViewportToolbar();
    ~ViewportToolbar() = default;
    
    /**
     * @brief Render toolbar
     * @param renderMode Current render mode
     * @param zoom Current zoom level
     * @param playMode Play mode controller (can be null)
     */
    void render(RenderMode& renderMode, float& zoom, float& panX, float& panY, editor::EditorPlayMode* playMode, editor::EditorCamera3D* camera = nullptr);
    
    /**
     * @brief Get view toggle states
     */
    bool showGrid() const { return m_showGrid; }
    bool showHotspots() const { return m_showHotspots; }
    bool showWalkArea() const { return m_showWalkArea; }
    bool showPhysicsDebug() const { return m_showPhysicsDebug; }

private:
    void renderModeToggle(RenderMode& renderMode, editor::EditorCamera3D* camera);
    void renderZoomControls(float& zoom, float& panX, float& panY);
    void renderViewToggles();
    void renderPlayControls(editor::EditorPlayMode* playMode);
    
    bool m_showGrid = true;
    bool m_showHotspots = true;
    bool m_showWalkArea = true;
    bool m_showPhysicsDebug = true;
};

} // namespace viewport
} // namespace editor
