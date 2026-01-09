/**
 * @file ViewportBreadcrumbs.h
 * @brief Navigation breadcrumbs for viewport hierarchy
 */
#pragma once

#include <string>

// Forward declarations
namespace engine {
    class World;
    class Level;
    class Scene;
}

namespace editor {
namespace viewport {

/**
 * @class ViewportBreadcrumbs
 * @brief Renders navigation breadcrumbs for World > Level > Scene hierarchy
 */
class ViewportBreadcrumbs {
public:
    ViewportBreadcrumbs() = default;
    ~ViewportBreadcrumbs() = default;
    
    /**
     * @brief Render breadcrumbs
     * @param world Current world
     * @param level Current level (can be null)
     * @param scene Current scene (can be null)
     * @return True if navigation was requested
     */
    bool render(engine::World* world, engine::Level* level, engine::Scene* scene);
    
    /**
     * @brief Check if user clicked to navigate to world
     */
    bool shouldNavigateToWorld() const { return m_navigateToWorld; }
    
    /**
     * @brief Check if user clicked to navigate to level
     */
    bool shouldNavigateToLevel() const { return m_navigateToLevel; }
    
    /**
     * @brief Reset navigation flags
     */
    void resetNavigation();

private:
    bool m_navigateToWorld = false;
    bool m_navigateToLevel = false;
};

} // namespace viewport
} // namespace editor
