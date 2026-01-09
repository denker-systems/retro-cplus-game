/**
 * @file RuntimeWorld.h
 * @brief World loader för Runtime - laddar editor-skapad content från JSON
 */
#pragma once

#include <memory>
#include <string>

namespace engine {
    class World;
    class Level;
    class Scene;
    namespace physics {
        class PhysicsManager;
    }
}

/**
 * @brief Laddar och hanterar world från editor-data
 * 
 * Ansvar:
 * - Ladda scenes.json från assets/data/
 * - Skapa World → Level → Scene hierarki
 * - Instantiera actors från JSON
 * - Sätt upp physics bodies
 */
class RuntimeWorld {
public:
    RuntimeWorld();
    ~RuntimeWorld();
    
    /**
     * @brief Ladda world från JSON-filer
     * @param physicsManager Physics manager för att initiera physics bodies
     * @return true om laddning lyckades
     */
    bool loadFromJSON(engine::physics::PhysicsManager* physicsManager);
    
    /**
     * @brief Hämta world
     */
    engine::World* getWorld() const { return m_world.get(); }
    
    /**
     * @brief Hämta aktiv scene
     */
    engine::Scene* getActiveScene() const { return m_activeScene; }
    
    /**
     * @brief Sätt aktiv scene
     */
    void setActiveScene(engine::Scene* scene) { m_activeScene = scene; }
    
private:
    bool loadScenes();
    bool createGroundPlane(engine::physics::PhysicsManager* physicsManager);
    engine::Scene* createSceneFromJSON(const void* sceneData);
    
    std::unique_ptr<engine::World> m_world;
    engine::Scene* m_activeScene = nullptr;
};
