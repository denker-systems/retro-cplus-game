/**
 * @file Scene3DRenderer.cpp
 * @brief Implementation of 3D scene renderer
 * 
 * Scene3DRenderer inherits from Level3DRenderer → World3DRenderer.
 * It determines the correct view level based on what data is set.
 */
#include "Scene3DRenderer.h"
#include "../../3d/Viewport3DPanel.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"

namespace editor {
namespace viewport {

void Scene3DRenderer::setup3DView() {
    if (!m_viewport3D) return;
    
    // Determine view level based on hierarchy - inherited members from parent classes
    // Scene3DRenderer has: m_world (from World3DRenderer), m_level (from Level3DRenderer), m_scene
    if (m_scene) {
        // Scene level - show actors
        m_viewport3D->setViewLevel(View3DLevel::Scene);
        m_viewport3D->setWorld(m_world);
        m_viewport3D->setLevel(m_level);
        m_viewport3D->setScene(m_scene);
    } else if (m_level) {
        // Level level - show scenes as tiles
        m_viewport3D->setViewLevel(View3DLevel::Level);
        m_viewport3D->setWorld(m_world);
        m_viewport3D->setLevel(m_level);
        m_viewport3D->setScene(nullptr);
    } else if (m_world) {
        // World level - show levels as boxes
        m_viewport3D->setViewLevel(View3DLevel::World);
        m_viewport3D->setWorld(m_world);
        m_viewport3D->setLevel(nullptr);
        m_viewport3D->setScene(nullptr);
    }
}

} // namespace viewport
} // namespace editor
