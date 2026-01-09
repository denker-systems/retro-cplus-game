/**
 * @file Scene3DRenderer.h
 * @brief 3D renderer for Scene view - inherits from Level3DRenderer
 */
#pragma once

#include "Level3DRenderer.h"

namespace engine {
    class Scene;
}

namespace editor {
namespace viewport {

/**
 * @class Scene3DRenderer
 * @brief Renders scene in 3D (actors in 3D space)
 * 
 * Inherits World and Level rendering from parent classes.
 * Hierarchy: Base3DRenderer → World3DRenderer → Level3DRenderer → Scene3DRenderer
 */
class Scene3DRenderer : public Level3DRenderer {
public:
    Scene3DRenderer() = default;
    ~Scene3DRenderer() override = default;
    
    void setScene(engine::Scene* scene) { m_scene = scene; }
    engine::Scene* getScene() const { return m_scene; }

protected:
    void setup3DView() override;
    
    engine::Scene* m_scene = nullptr;
};

} // namespace viewport
} // namespace editor
