/**
 * @file BaseViewRenderer.h
 * @brief Abstract base class for all viewport renderers
 */
#pragma once

#include "../ViewportTypes.h"

// Forward declarations
struct ImDrawList;
class EditorContext;
class SelectionManager;

namespace editor {
namespace viewport {

/**
 * @class BaseViewRenderer
 * @brief Abstract base for all viewport renderers
 * 
 * Provides common interface and shared functionality for rendering.
 * 3D is primary, 2D is secondary.
 */
class BaseViewRenderer {
public:
    virtual ~BaseViewRenderer() = default;
    
    /**
     * @brief Initialize renderer with context
     */
    virtual void initialize(EditorContext* context, SelectionManager* selectionManager);
    
    /**
     * @brief Update renderer state
     */
    virtual void update(float deltaTime);
    
    /**
     * @brief Render the view
     * @param drawList ImGui draw list (can be null for 3D)
     * @param ctx Render context
     */
    virtual void render(ImDrawList* drawList, const RenderContext& ctx) = 0;
    
    /**
     * @brief Check if double-click occurred
     */
    bool wasDoubleClicked() const { return m_doubleClicked; }
    
    /**
     * @brief Clear double-click flag
     */
    void clearDoubleClick() { m_doubleClicked = false; }

protected:
    EditorContext* m_context = nullptr;
    SelectionManager* m_selectionManager = nullptr;
    bool m_doubleClicked = false;
};

} // namespace viewport
} // namespace editor
