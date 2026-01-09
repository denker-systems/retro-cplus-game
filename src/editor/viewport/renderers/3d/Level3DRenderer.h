/**
 * @file Level3DRenderer.h
 * @brief 3D renderer for Level view (PRIMARY)
 */
#pragma once

#include "../../core/Base3DRenderer.h"

namespace engine {
    class Level;
}

namespace editor {
namespace viewport {

/**
 * @class Level3DRenderer
 * @brief Renders level in 3D (scenes as 3D tiles)
 */
class Level3DRenderer : public Base3DRenderer {
public:
    Level3DRenderer() = default;
    ~Level3DRenderer() override = default;
    
    void setLevel(engine::Level* level) { m_level = level; }

protected:
    void setup3DView() override;
    
    engine::Level* m_level = nullptr;
};

} // namespace viewport
} // namespace editor
