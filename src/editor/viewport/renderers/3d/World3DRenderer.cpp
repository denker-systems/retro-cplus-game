/**
 * @file World3DRenderer.cpp
 * @brief Implementation of 3D world renderer
 */
#include "World3DRenderer.h"
#include "../../3d/Viewport3DPanel.h"
#include "engine/world/World.h"

namespace editor {
namespace viewport {

void World3DRenderer::setup3DView() {
    if (m_viewport3D) {
        m_viewport3D->setViewLevel(View3DLevel::World);
        m_viewport3D->setWorld(m_world);
    }
}

} // namespace viewport
} // namespace editor
