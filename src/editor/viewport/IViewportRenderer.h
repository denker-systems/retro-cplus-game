/**
 * @file IViewportRenderer.h
 * @brief Interface for viewport renderers at different hierarchy levels
 */
#pragma once

#include <glm/glm.hpp>

#ifdef HAS_IMGUI
struct ImDrawList;
#endif

namespace engine {
    class World;
    class Level;
    class Scene;
    class ActorObjectExtended;
}

class EditorContext;
class SelectionManager;

namespace editor {

/**
 * @brief Viewport render context passed to renderers
 */
struct ViewportRenderContext {
    glm::vec2 viewportPos{0.0f, 0.0f};
    glm::vec2 viewportSize{0.0f, 0.0f};
    float zoom = 1.0f;
    float panX = 0.0f;
    float panY = 0.0f;
    bool isHovered = false;
    bool isFocused = false;
    float deltaTime = 0.0f;
};

/**
 * @brief Interface for hierarchy-level viewport renderers
 */
class IViewportRenderer {
public:
    virtual ~IViewportRenderer() = default;
    
    /**
     * @brief Initialize the renderer
     */
    virtual void initialize(EditorContext* context, SelectionManager* selectionManager) = 0;
    
    /**
     * @brief Render the viewport content
     */
    virtual void render(ImDrawList* drawList, const ViewportRenderContext& ctx) = 0;
    
    /**
     * @brief Update per-frame logic
     */
    virtual void update(float deltaTime) = 0;
    
    /**
     * @brief Handle mouse click
     * @return true if input was consumed
     */
    virtual bool onMouseClick(float x, float y, const ViewportRenderContext& ctx) = 0;
    
    /**
     * @brief Handle mouse drag
     * @return true if input was consumed
     */
    virtual bool onMouseDrag(float x, float y, float deltaX, float deltaY, const ViewportRenderContext& ctx) = 0;
    
    /**
     * @brief Handle mouse release
     */
    virtual void onMouseRelease(float x, float y, const ViewportRenderContext& ctx) = 0;
    
    /**
     * @brief Handle mouse double-click
     * @return true if navigation should occur
     */
    virtual bool onDoubleClick(float x, float y, const ViewportRenderContext& ctx) = 0;
    
    /**
     * @brief Handle scroll wheel
     */
    virtual void onScroll(float delta, const ViewportRenderContext& ctx) = 0;
    
    /**
     * @brief Get selected item for navigation
     */
    virtual void* getSelectedItem() const = 0;
    
    /**
     * @brief Clear selection state
     */
    virtual void clearSelection() = 0;
    
    /**
     * @brief Get renderer name for debugging
     */
    virtual const char* getName() const = 0;
};

} // namespace editor
