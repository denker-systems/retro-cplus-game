/**
 * @file PathfindingComponent.h
 * @brief Pathfinding Component
 */
#pragma once

#include "engine/core/ActorComponent.h"
#include "engine/core/Vec2.h"
#include <vector>

namespace engine {

/**
 * @brief Component for pathfinding
 */
class PathfindingComponent : public ActorComponent {
public:
    PathfindingComponent(const std::string& name = "PathfindingComponent");
    virtual ~PathfindingComponent() = default;
    
    void setTarget(Vec2 target);
    Vec2 getTarget() const { return m_target; }
    
    void setPath(const std::vector<Vec2>& path) { m_path = path; m_currentWaypoint = 0; }
    const std::vector<Vec2>& getPath() const { return m_path; }
    
    bool hasPath() const { return !m_path.empty(); }
    Vec2 getCurrentWaypoint() const;
    
    void clearPath();
    void update(float deltaTime) override;
    
private:
    Vec2 m_target{0, 0};
    std::vector<Vec2> m_path;
    int m_currentWaypoint = 0;
};

} // namespace engine
