/**
 * @file WorldContainer.h
 * @brief Base class for World/Level/Scene hierarchy
 * 
 * Hierarchy:
 *   WorldContainer (abstract base)
 *   └── World (grid of Levels)
 *       └── Level (grid of Scenes)
 *           └── Scene (grid of Actors)
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
#include "GridTypes.h"
#include <string>
#include <memory>
#include <vector>
#include <SDL.h>

namespace engine {

/**
 * @brief Base class for World, Level, and Scene
 * 
 * Provides shared functionality:
 * - Name management
 * - Actor container
 * - Spatial grid positioning
 * - Update/Render lifecycle
 */
class WorldContainer {
public:
    explicit WorldContainer(const std::string& name) : m_name(name) {}
    virtual ~WorldContainer() = default;
    
    // Lifecycle
    virtual void update(float deltaTime) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    
    // Identity
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }
    
    // ═══════════════════════════════════════════════════════════════════
    // SPATIAL GRID (inherited by World, Level, Scene)
    // ═══════════════════════════════════════════════════════════════════
    
    const GridPosition& getGridPosition() const { return m_gridPosition; }
    void setGridPosition(const GridPosition& pos) { m_gridPosition = pos; }
    void setGridPosition(int x, int y, int w = 1, int h = 1) { 
        m_gridPosition = {x, y, w, h}; 
    }
    
    int getGridX() const { return m_gridPosition.gridX; }
    int getGridY() const { return m_gridPosition.gridY; }
    int getWidth() const { return m_gridPosition.pixelWidth; }
    int getHeight() const { return m_gridPosition.pixelHeight; }
    
    void setSize(int w, int h) { 
        m_gridPosition.pixelWidth = w; 
        m_gridPosition.pixelHeight = h; 
    }
    
    // ═══════════════════════════════════════════════════════════════════
    // ACTORS
    // ═══════════════════════════════════════════════════════════════════
    
    void addActor(std::unique_ptr<ActorObjectExtended> actor) {
        if (actor) {
            m_actors.push_back(std::move(actor));
        }
    }
    
    const std::vector<std::unique_ptr<ActorObjectExtended>>& getActors() const {
        return m_actors;
    }
    
    ActorObjectExtended* findActor(const std::string& name) const {
        for (auto& actor : m_actors) {
            if (actor->getName() == name) {
                return actor.get();
            }
        }
        return nullptr;
    }
    
    bool removeActor(ActorObjectExtended* actor) {
        for (auto it = m_actors.begin(); it != m_actors.end(); ++it) {
            if (it->get() == actor) {
                m_actors.erase(it);
                return true;
            }
        }
        return false;
    }
    
protected:
    std::string m_name;
    std::vector<std::unique_ptr<ActorObjectExtended>> m_actors;
    GridPosition m_gridPosition = {0, 0, 640, 400};  // Default size
};

} // namespace engine
