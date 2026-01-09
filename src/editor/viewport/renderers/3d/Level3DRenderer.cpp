/**
 * @file Level3DRenderer.cpp
 * @brief Implementation of 3D level renderer
 */
#include "Level3DRenderer.h"
#include "../../3d/Viewport3DPanel.h"
#include "engine/world/Level.h"

namespace editor {
namespace viewport {

void Level3DRenderer::setup3DView() {
    if (m_viewport3D) {
        m_viewport3D->setViewLevel(View3DLevel::Level);
        m_viewport3D->setLevel(m_level);
    }
}

} // namespace viewport
} // namespace editor
