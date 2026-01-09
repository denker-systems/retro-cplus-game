/**
 * @file ViewportTypes.h
 * @brief Shared types for viewport system
 */
#pragma once

#include <glm/glm.hpp>

namespace editor {
namespace viewport {

/**
 * @brief Viewport rendering mode
 */
enum class RenderMode {
    Mode2D,
    Mode3D
};

/**
 * @brief Hierarchy level in world structure
 */
enum class HierarchyLevel {
    World,
    Level,
    Scene
};

/**
 * @brief View mode for world/level views
 */
enum class ViewMode {
    Spatial,
    Grid,
    List
};

/**
 * @brief Rendering context passed to renderers
 */
struct RenderContext {
    glm::vec2 viewportPos{0.0f, 0.0f};
    glm::vec2 viewportSize{0.0f, 0.0f};
    float zoom = 1.0f;
    float panX = 0.0f;
    float panY = 0.0f;
    RenderMode renderMode = RenderMode::Mode2D;
    
    bool showGrid = true;
    bool showHotspots = true;
    bool showWalkArea = true;
    bool showPhysicsDebug = true;
};

/**
 * @brief Input event data
 */
struct InputEvent {
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    bool leftButton = false;
    bool rightButton = false;
    bool middleButton = false;
    bool ctrlKey = false;
    bool shiftKey = false;
    bool altKey = false;
};

} // namespace viewport
} // namespace editor
