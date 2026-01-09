/**
 * @file World3DRenderer.h
 * @brief 3D renderer for World view - base of 3D hierarchy
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
 * 
 * Base of the 3D rendering hierarchy.
 * Hierarchy: Base3DRenderer → World3DRenderer → Level3DRenderer → Scene3DRenderer
 */
class World3DRenderer : public Base3DRenderer {
public:
    World3DRenderer() = default;
    ~World3DRenderer() override = default;
    
    void setWorld(engine::World* world) { m_world = world; }
    engine::World* getWorld() const { return m_world; }

protected:
    void setup3DView() override;
    
    engine::World* m_world = nullptr;
};

} // namespace viewport
} // namespace editor
