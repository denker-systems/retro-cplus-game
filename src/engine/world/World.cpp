/**
 * @file World.cpp
 * @brief Implementation of World manager
 */
#include "World.h"

namespace engine {

World::World() {}

void World::update(float deltaTime) {
    if (m_isTransitioning) {
        updateTransition(deltaTime);
    }
    
    // Update active scene
    if (!m_sceneStack.empty()) {
        m_sceneStack.back()->update(deltaTime);
    }
}

void World::render(SDL_Renderer* renderer) {
    if (!renderer) return;
    
    // Render all scenes in stack (bottom to top)
    for (Scene* scene : m_sceneStack) {
        if (scene) {
            scene->render(renderer);
        }
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SCENE MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════

void World::addScene(const std::string& name, std::unique_ptr<Scene> scene) {
    if (scene) {
        m_scenes[name] = std::move(scene);
    }
}

void World::removeScene(const std::string& name) {
    // Don't remove if it's in the stack
    for (Scene* scene : m_sceneStack) {
        if (scene && scene->getName() == name) {
            return;
        }
    }
    m_scenes.erase(name);
}

Scene* World::getScene(const std::string& name) const {
    auto it = m_scenes.find(name);
    if (it != m_scenes.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool World::hasScene(const std::string& name) const {
    return m_scenes.find(name) != m_scenes.end();
}

// ═══════════════════════════════════════════════════════════════════════════
// SCENE STACK
// ═══════════════════════════════════════════════════════════════════════════

void World::changeScene(const std::string& name) {
    Scene* newScene = getScene(name);
    if (!newScene) return;
    
    // Exit all current scenes
    for (Scene* scene : m_sceneStack) {
        if (scene) {
            scene->onSceneExit();
            scene->propagateExit();
        }
    }
    
    m_sceneStack.clear();
    m_sceneStack.push_back(newScene);
    
    // Enter new scene
    newScene->propagateEnter();
    newScene->onSceneEnter();
}

void World::pushScene(const std::string& name) {
    Scene* newScene = getScene(name);
    if (!newScene) return;
    
    // Pause current scene
    if (!m_sceneStack.empty() && m_sceneStack.back()) {
        m_sceneStack.back()->onScenePause();
    }
    
    m_sceneStack.push_back(newScene);
    
    // Enter new scene
    newScene->propagateEnter();
    newScene->onSceneEnter();
}

void World::popScene() {
    if (m_sceneStack.empty()) return;
    
    // Exit top scene
    Scene* topScene = m_sceneStack.back();
    if (topScene) {
        topScene->onSceneExit();
        topScene->propagateExit();
    }
    
    m_sceneStack.pop_back();
    
    // Resume previous scene
    if (!m_sceneStack.empty() && m_sceneStack.back()) {
        m_sceneStack.back()->onSceneResume();
    }
}

Scene* World::getActiveScene() const {
    if (!m_sceneStack.empty()) {
        return m_sceneStack.back();
    }
    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// TRANSITIONS
// ═══════════════════════════════════════════════════════════════════════════

void World::changeSceneWithTransition(const std::string& name, TransitionType type, float duration) {
    if (!hasScene(name)) return;
    
    m_isTransitioning = true;
    m_transitionType = type;
    m_transitionDuration = duration;
    m_transitionTimer = 0.0f;
    m_nextScene = name;
    
    // Transition callback will change scene at midpoint
    m_transitionCallback = [this]() {
        changeScene(m_nextScene);
    };
}

void World::updateTransition(float deltaTime) {
    m_transitionTimer += deltaTime;
    
    // At midpoint, execute callback
    if (m_transitionTimer >= m_transitionDuration * 0.5f && m_transitionCallback) {
        m_transitionCallback();
        m_transitionCallback = nullptr;
    }
    
    // Complete transition
    if (m_transitionTimer >= m_transitionDuration) {
        completeTransition();
    }
}

void World::completeTransition() {
    m_isTransitioning = false;
    m_transitionType = TransitionType::None;
    m_transitionTimer = 0.0f;
    m_transitionDuration = 0.0f;
    m_nextScene.clear();
    m_transitionCallback = nullptr;
}

} // namespace engine
