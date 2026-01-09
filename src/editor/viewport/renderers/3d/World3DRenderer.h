/**
 * @file World3DRenderer.h
 * @brief 3D renderer for World view (PRIMARY)
 */
#pragma once

#include "../../core/Base3DRenderer.h"

namespace engine {
    class World;
}

namespace editor {
namespace viewport {

/**
 * @class World3DRenderer
 * @brief Renders world in 3D (levels as 3D boxes)
 */
class World3DRenderer : public Base3DRenderer {
public:
    World3DRenderer() = default;
    ~World3DRenderer() override = default;
    
    void setWorld(engine::World* world) { m_world = world; }

protected:
    void setup3DView() override;
    
    engine::World* m_world = nullptr;
};

} // namespace viewport
} // namespace editor
