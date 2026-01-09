/**
 * @file Scene3DRenderer.h
 * @brief 3D renderer for Scene view (PRIMARY)
 */
#pragma once

#include "../../core/Base3DRenderer.h"

namespace engine {
    class Scene;
}

namespace editor {
namespace viewport {

/**
 * @class Scene3DRenderer
 * @brief Renders scene in 3D (actors in 3D space)
 */
class Scene3DRenderer : public Base3DRenderer {
public:
    Scene3DRenderer() = default;
    ~Scene3DRenderer() override = default;
    
    void setScene(engine::Scene* scene) { m_scene = scene; }

protected:
    void setup3DView() override;
    
    engine::Scene* m_scene = nullptr;
};

} // namespace viewport
} // namespace editor
