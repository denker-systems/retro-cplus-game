/**
 * @file World.h
 * @brief World manager for multiple scenes
 */
#pragma once

#include "Scene.h"
#include <memory>
#include <unordered_map>
#include <string>
#include <functional>

namespace engine {

/**
 * @brief Manages multiple scenes and transitions
 * 
 * Features:
 * - Scene stack (for overlays)
 * - Scene transitions
 * - Global update/render
 */
class World {
public:
    World();
    ~World() = default;
    
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    
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
    
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
    std::vector<Scene*> m_sceneStack;
    
    // Transition state
    bool m_isTransitioning = false;
    TransitionType m_transitionType = TransitionType::None;
    float m_transitionTimer = 0.0f;
    float m_transitionDuration = 0.0f;
    std::string m_nextScene;
    std::function<void()> m_transitionCallback;
};

} // namespace engine
