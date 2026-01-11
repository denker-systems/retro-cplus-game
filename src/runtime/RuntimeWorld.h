/**
 * @file RuntimeWorld.h
 * @brief World loader för Runtime - laddar editor-skapad content från JSON
 */
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

/**
 * @brief Exit/Door data för scene och level transitions
 * 
 * Stödjer både:
 * - Scene transitions (inom samma level)
 * - Level transitions (till annat level + scene)
 */
struct ExitData {
    std::string id;
    std::string name;
    
    // Nuvarande position i hierarkin
    std::string levelId;        // Vilken level denna exit tillhör
    std::string sceneId;        // Vilken scene denna exit tillhör
    
    // Mål för transition
    std::string targetLevel;    // Mållevel (tom = samma level)
    std::string targetScene;    // Målscen att byta till
    std::string targetSpawn;    // Spawn point i målscen (optional)
    
    glm::vec3 position;         // 3D position
    float interactRadius = 2.0f;
    
    // Helper: Är detta en level transition?
    bool isLevelTransition() const { return !targetLevel.empty() && targetLevel != levelId; }
};

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
    
    /**
     * @brief Hämta aktiv level
     */
    engine::Level* getActiveLevel() const { return m_activeLevel; }
    
    /**
     * @brief Sätt aktiv level
     */
    void setActiveLevel(engine::Level* level) { m_activeLevel = level; }
    
    /**
     * @brief Hämta level by ID
     */
    engine::Level* getLevel(const std::string& levelId) const;
    
    /**
     * @brief Hämta alla exits för en specifik scene
     */
    std::vector<ExitData> getExitsForScene(const std::string& sceneId) const;
    
    /**
     * @brief Hämta alla exits
     */
    const std::vector<ExitData>& getAllExits() const { return m_exits; }
    
private:
    bool loadScenes();
    bool createGroundPlane(engine::physics::PhysicsManager* physicsManager);
    void createNPCColliders(engine::physics::PhysicsManager* physicsManager);
    engine::Scene* createSceneFromJSON(const void* sceneData);
    void parseExitsForScene(const void* sceneData, const std::string& levelId, const std::string& sceneId);
    
    std::unique_ptr<engine::World> m_world;
    engine::physics::PhysicsManager* m_physicsManager = nullptr;
    engine::Level* m_activeLevel = nullptr;
    engine::Scene* m_activeScene = nullptr;
    std::vector<ExitData> m_exits;
};
