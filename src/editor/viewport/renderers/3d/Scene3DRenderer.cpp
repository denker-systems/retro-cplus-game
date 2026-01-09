/**
 * @file Scene3DRenderer.cpp
 * @brief Implementation of 3D scene renderer
 */
#include "Scene3DRenderer.h"
#include "../../3d/Viewport3DPanel.h"
#include "engine/world/Scene.h"

namespace editor {
namespace viewport {

void Scene3DRenderer::setup3DView() {
    if (m_viewport3D) {
        m_viewport3D->setViewLevel(View3DLevel::Scene);
        m_viewport3D->setScene(m_scene);
    }
}

} // namespace viewport
} // namespace editor
