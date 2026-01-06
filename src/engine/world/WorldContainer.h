/**
 * @file WorldContainer.h
 * @brief Base class for World/Level/Scene hierarchy
 * 
 * Hierarchy:
 *   WorldContainer (abstract base)
 *   └── World
 *       └── Level
 *           └── Scene
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"
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
    
    // Actors
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
    
protected:
    std::string m_name;
    std::vector<std::unique_ptr<ActorObjectExtended>> m_actors;
};

} // namespace engine
