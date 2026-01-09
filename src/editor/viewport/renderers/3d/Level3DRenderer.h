/**
 * @file Level3DRenderer.h
 * @brief 3D renderer for Level view - inherits from World3DRenderer
 */
#pragma once

#include "World3DRenderer.h"

namespace engine {
    class Level;
}

namespace editor {
namespace viewport {

/**
 * @class Level3DRenderer
 * @brief Renders level in 3D (scenes as 3D tiles)
 * 
 * Inherits World rendering from World3DRenderer.
 * Hierarchy: Base3DRenderer → World3DRenderer → Level3DRenderer
 */
class Level3DRenderer : public World3DRenderer {
public:
    Level3DRenderer() = default;
    ~Level3DRenderer() override = default;
    
    void setLevel(engine::Level* level) { m_level = level; }
    engine::Level* getLevel() const { return m_level; }

protected:
    void setup3DView() override;
    
    engine::Level* m_level = nullptr;
};

} // namespace viewport
} // namespace editor
