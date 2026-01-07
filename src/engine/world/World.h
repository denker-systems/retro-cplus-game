/**
 * @file World.h
 * @brief World manager - Top of hierarchy
 * 
 * Hierarchy: World → Level → Scene
 */
#pragma once

#include "WorldContainer.h"
#include "Scene.h"
#include "Level.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <vector>
#include <functional>

namespace engine {

/**
 * @brief Game world - Top of hierarchy
 * 
 * Contains Levels, which contain Scenes
 * Inherits from WorldContainer for shared functionality
 */
class World : public WorldContainer {
public:
    World() : WorldContainer("World") {}
    explicit World(const std::string& name) : WorldContainer(name) {}
    ~World() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // SCENE MANAGEMENT
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Add a scene to the world */
    void addScene(const std::string& name, std::unique_ptr<Scene> scene);
    
    /** @brief Remove a scene */
    void removeScene(const std::string& name);
    
    /** @brief Get a scene by name */
    Scene* getScene(const std::string& name) const;
    
    /** @brief Check if scene exists */
    bool hasScene(const std::string& name) const;
    
    // ═══════════════════════════════════════════════════════════════════
    // SCENE STACK
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Change to a scene (clears stack) */
    void changeScene(const std::string& name);
    
    /** @brief Push scene onto stack (overlay) */
    void pushScene(const std::string& name);
    
    /** @brief Pop top scene from stack */
    void popScene();
    
    /** @brief Get active scene (top of stack) */
    Scene* getActiveScene() const;
    
    /** @brief Get scene stack size */
    size_t getSceneStackSize() const { return m_sceneStack.size(); }
    
    // ═══════════════════════════════════════════════════════════════════
    // LEVEL MANAGEMENT
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Add a level to the world */
    void addLevel(std::unique_ptr<Level> level);
    
    /** @brief Remove a level */
    void removeLevel(const std::string& levelId);
    
    /** @brief Get a level by ID */
    Level* getLevel(const std::string& levelId) const;
    
    /** @brief Get all levels */
    const std::vector<std::unique_ptr<Level>>& getLevels() const { return m_levels; }
    
    /** @brief Set active level */
    void setActiveLevel(const std::string& levelId);
    
    /** @brief Get active level */
    Level* getActiveLevel() const { return m_activeLevel; }
    
    /** @brief Transition to a level (calls lifecycle hooks) */
    void transitionToLevel(const std::string& levelId);
    
    // ═══════════════════════════════════════════════════════════════════
    // TRANSITIONS
    // ═══════════════════════════════════════════════════════════════════
    
    enum class TransitionType {
        None,
        Fade,
        Slide
    };
    
    /** @brief Change scene with transition */
    void changeSceneWithTransition(const std::string& name, TransitionType type, float duration);
    
    /** @brief Check if transition is active */
    bool isTransitioning() const { return m_isTransitioning; }
    
protected:
    void updateTransition(float deltaTime);
    void completeTransition();
    
    // Scenes (legacy - may be deprecated)
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
    std::vector<Scene*> m_sceneStack;
    
    // Levels (new hierarchy)
    std::vector<std::unique_ptr<Level>> m_levels;
    std::unordered_map<std::string, Level*> m_levelMap;
    Level* m_activeLevel = nullptr;
    
    // Transition state
    bool m_isTransitioning = false;
    TransitionType m_transitionType = TransitionType::None;
    float m_transitionTimer = 0.0f;
    float m_transitionDuration = 0.0f;
    std::string m_nextScene;
    std::function<void()> m_transitionCallback;
};

} // namespace engine
