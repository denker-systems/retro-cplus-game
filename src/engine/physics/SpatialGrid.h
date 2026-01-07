/**
 * @file SpatialGrid.h
 * @brief Spatial partitioning grid for efficient collision detection
 */
#pragma once

#include "CollisionShape.h"
#include "PhysicsBody.h"
#include <vector>
#include <unordered_map>

namespace engine {

/**
 * @brief Grid-based spatial partitioning for broad phase collision
 */
class SpatialGrid {
public:
    SpatialGrid(int cellSize = 64);
    ~SpatialGrid() = default;
    
    /** @brief Clear all bodies from grid */
    void clear();
    
    /** @brief Insert body into grid */
    void insert(PhysicsBody* body);
    
    /** @brief Get potential collision candidates for a body */
    std::vector<PhysicsBody*> query(const PhysicsBody* body) const;
    
    /** @brief Get all bodies in a rectangular area */
    std::vector<PhysicsBody*> queryRect(const SDL_Rect& rect) const;
    
private:
    int getCellKey(int x, int y) const;
    void getCellsForBody(const PhysicsBody* body, std::vector<int>& cells) const;
    
    int m_cellSize;
    std::unordered_map<int, std::vector<PhysicsBody*>> m_grid;
};

} // namespace engine
