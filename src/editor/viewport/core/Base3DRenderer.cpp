/**
 * @file Base3DRenderer.cpp
 * @brief Implementation of 3D base renderer
 */
#include "Base3DRenderer.h"
#include "../3d/Viewport3DPanel.h"
#include <iostream>

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

void Base3DRenderer::update(float deltaTime) {
    BaseViewRenderer::update(deltaTime);
    
    // Update Viewport3DPanel (for camera follow, etc.)
    if (m_viewport3D) {
        m_viewport3D->update(deltaTime);
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

void Base3DRenderer::setPlayMode(editor::EditorPlayMode* playMode) {
    std::cout << "[Base3DRenderer] setPlayMode called - playMode=" 
              << (playMode ? "SET" : "NULL")
              << " m_viewport3D=" << (m_viewport3D ? "SET" : "NULL") << std::endl;
    if (m_viewport3D) {
        std::cout << "[Base3DRenderer] Forwarding to Viewport3DPanel" << std::endl;
        m_viewport3D->setPlayMode(playMode);
    } else {
        std::cout << "[Base3DRenderer] WARNING: m_viewport3D is NULL!" << std::endl;
    }
}

} // namespace viewport
} // namespace editor
