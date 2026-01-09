/**
 * @file Base3DRenderer.cpp
 * @brief Implementation of 3D base renderer
 */
#include "Base3DRenderer.h"
#include "../3d/Viewport3DPanel.h"

namespace editor {
namespace viewport {

Base3DRenderer::Base3DRenderer() = default;

Base3DRenderer::~Base3DRenderer() = default;

void Base3DRenderer::initialize(EditorContext* context, SelectionManager* selectionManager) {
    BaseViewRenderer::initialize(context, selectionManager);
    
    m_viewport3D = std::make_unique<editor::Viewport3DPanel>();
    if (m_selectionManager) {
        m_viewport3D->setSelectionManager(m_selectionManager);
    }
}

void Base3DRenderer::render(ImDrawList* drawList, const RenderContext& ctx) {
    if (!m_viewport3D) {
        m_viewport3D = std::make_unique<editor::Viewport3DPanel>();
        if (m_selectionManager) {
            m_viewport3D->setSelectionManager(m_selectionManager);
        }
    }
    
    setup3DView();
    m_viewport3D->render();
}

} // namespace viewport
} // namespace editor
