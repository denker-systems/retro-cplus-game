/**
 * @file Level.h
 * @brief Level (Chapter) - Middle of hierarchy
 * 
 * Hierarchy: World → Level → Scene
 */
#pragma once

#include "WorldContainer.h"
#include "Scene.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace engine {

/**
 * @brief Level/Chapter - Contains Scenes
 * 
 * Inherits from WorldContainer
 * Child of World, Parent of Scene
 * 
 * A Level represents a chapter, act, or area in the game.
 * Contains multiple Scenes and manages transitions between them.
 * 
 * Example:
 *   Level "Act 1: Tavern District"
 *     - Scene "tavern_interior"
 *     - Scene "tavern_exterior"  
 *     - Scene "market_square"
 */
class Level : public WorldContainer {
public:
    Level() : WorldContainer("Level"), m_id("level_" + std::to_string(s_nextId++)) {}
    explicit Level(const std::string& id) : WorldContainer(id), m_id(id) {}
    explicit Level(const std::string& id, const std::string& name) : WorldContainer(name), m_id(id) {}
    ~Level() = default;
    
    // ═══════════════════════════════════════════════════════════════════
    // BASIC INFO
    // ═══════════════════════════════════════════════════════════════════
    
    const std::string& getId() const { return m_id; }
    // getName/setName inherited from WorldContainer
    
    const std::string& getDescription() const { return m_description; }
    void setDescription(const std::string& desc) { m_description = desc; }
    
    // ═══════════════════════════════════════════════════════════════════
    // SCENE MANAGEMENT
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Add scene to level */
    void addScene(std::unique_ptr<Scene> scene);
    
    /** @brief Get scene by ID */
    Scene* getScene(const std::string& sceneId) const;
    
    /** @brief Get all scenes */
    const std::vector<std::unique_ptr<Scene>>& getScenes() const { return m_scenes; }
    
    /** @brief Remove scene */
    std::unique_ptr<Scene> removeScene(const std::string& sceneId);
    
    /** @brief Get active scene */
    Scene* getActiveScene() const { return m_activeScene; }
    
    /** @brief Set active scene (doesn't call lifecycle hooks) */
    void setActiveScene(const std::string& sceneId);
    
    /** @brief Transition to scene (calls onExit/onEnter) */
    void transitionToScene(const std::string& sceneId);
    
    // ═══════════════════════════════════════════════════════════════════
    // UPDATE & RENDER
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Update active scene */
    void update(float deltaTime) override;
    
    /** @brief Render active scene */
    void render(SDL_Renderer* renderer) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // LIFECYCLE
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Called when level becomes active */
    virtual void onLevelEnter();
    
    /** @brief Called when level becomes inactive */
    virtual void onLevelExit();
    
    // ═══════════════════════════════════════════════════════════════════
    // LEVEL PROPERTIES
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Background music for this level */
    const std::string& getMusic() const { return m_music; }
    void setMusic(const std::string& music) { m_music = music; }
    
    /** @brief Ambient color/lighting */
    SDL_Color getAmbientColor() const { return m_ambientColor; }
    void setAmbientColor(SDL_Color color) { m_ambientColor = color; }

private:
    std::string m_id;
    std::vector<std::unique_ptr<Scene>> m_scenes;
    static inline int s_nextId = 1;
    std::unordered_map<std::string, Scene*> m_sceneMap;  // Fast lookup
    
    Scene* m_activeScene = nullptr;
    
    // Level properties
    std::string m_description;
    std::string m_music;
    SDL_Color m_ambientColor{255, 255, 255, 255};
};

} // namespace engine
