/**
 * @file SpatialGrid.cpp
 * @brief Implementation of SpatialGrid
 */
#include "SpatialGrid.h"
#include <algorithm>

namespace engine {

SpatialGrid::SpatialGrid(int cellSize) : m_cellSize(cellSize) {}

void SpatialGrid::clear() {
    m_grid.clear();
}

int SpatialGrid::getCellKey(int x, int y) const {
    // Simple hash for grid coordinates
    return (x << 16) | (y & 0xFFFF);
}

void SpatialGrid::getCellsForBody(const PhysicsBody* body, std::vector<int>& cells) const {
    if (!body || body->getShapes().empty()) return;
    
    // Get bounding box from first shape
    SDL_Rect bounds = body->getShapes()[0]->getBounds();
    
    // Calculate grid cells
    int minX = bounds.x / m_cellSize;
    int minY = bounds.y / m_cellSize;
    int maxX = (bounds.x + bounds.w) / m_cellSize;
    int maxY = (bounds.y + bounds.h) / m_cellSize;
    
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            cells.push_back(getCellKey(x, y));
        }
    }
}

void SpatialGrid::insert(PhysicsBody* body) {
    if (!body) return;
    
    std::vector<int> cells;
    getCellsForBody(body, cells);
    
    for (int cellKey : cells) {
        m_grid[cellKey].push_back(body);
    }
}

std::vector<PhysicsBody*> SpatialGrid::query(const PhysicsBody* body) const {
    std::vector<PhysicsBody*> result;
    if (!body) return result;
    
    std::vector<int> cells;
    getCellsForBody(body, cells);
    
    for (int cellKey : cells) {
        auto it = m_grid.find(cellKey);
        if (it != m_grid.end()) {
            for (PhysicsBody* other : it->second) {
                if (other != body) {
                    // Avoid duplicates
                    if (std::find(result.begin(), result.end(), other) == result.end()) {
                        result.push_back(other);
                    }
                }
            }
        }
    }
    
    return result;
}

std::vector<PhysicsBody*> SpatialGrid::queryRect(const SDL_Rect& rect) const {
    std::vector<PhysicsBody*> result;
    
    int minX = rect.x / m_cellSize;
    int minY = rect.y / m_cellSize;
    int maxX = (rect.x + rect.w) / m_cellSize;
    int maxY = (rect.y + rect.h) / m_cellSize;
    
    for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
            int cellKey = getCellKey(x, y);
            auto it = m_grid.find(cellKey);
            if (it != m_grid.end()) {
                for (PhysicsBody* body : it->second) {
                    if (std::find(result.begin(), result.end(), body) == result.end()) {
                        result.push_back(body);
                    }
                }
            }
        }
    }
    
    return result;
}

} // namespace engine
