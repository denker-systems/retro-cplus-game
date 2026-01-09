/**
 * @file Base3DRenderer.h
 * @brief Base class for 3D viewport rendering (PRIMARY)
 */
#pragma once

#include "BaseViewRenderer.h"
#include <memory>

namespace editor {
    class Viewport3DPanel;
}

namespace editor {
namespace viewport {

/**
 * @class Base3DRenderer
 * @brief Base for 3D rendering (PRIMARY rendering mode)
 * 
 * 3D is the default/primary rendering mode.
 * Uses Viewport3DPanel for OpenGL rendering.
 */
class Base3DRenderer : public BaseViewRenderer {
public:
    Base3DRenderer();
    virtual ~Base3DRenderer();
    
    void initialize(EditorContext* context, SelectionManager* selectionManager) override;
    void render(ImDrawList* drawList, const RenderContext& ctx) override;

protected:
    /**
     * @brief Setup 3D view level (World/Level/Scene)
     */
    virtual void setup3DView() = 0;
    
    std::unique_ptr<editor::Viewport3DPanel> m_viewport3D;
};

} // namespace viewport
} // namespace editor
