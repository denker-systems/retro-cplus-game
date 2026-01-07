/**
 * @file World.cpp
 * @brief Implementation of World manager
 */
#include "World.h"

namespace engine {

// Constructors now in header

void World::update(float deltaTime) {
    if (m_isTransitioning) {
        updateTransition(deltaTime);
    }
    
    // Update active level (new hierarchy)
    if (m_activeLevel) {
        m_activeLevel->update(deltaTime);
    }
    // Fallback: update active scene (legacy)
    else if (!m_sceneStack.empty()) {
        m_sceneStack.back()->update(deltaTime);
    }
}

void World::render(SDL_Renderer* renderer) {
    if (!renderer) return;
    
    // Render active level (new hierarchy)
    if (m_activeLevel) {
        m_activeLevel->render(renderer);
    }
    // Fallback: render all scenes in stack (legacy)
    else {
        for (Scene* scene : m_sceneStack) {
            if (scene) {
                scene->render(renderer);
            }
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
        }
    }
    
    m_sceneStack.clear();
    m_sceneStack.push_back(newScene);
    
    // Enter new scene
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
    newScene->onSceneEnter();
}

void World::popScene() {
    if (m_sceneStack.empty()) return;
    
    // Exit top scene
    Scene* topScene = m_sceneStack.back();
    if (topScene) {
        topScene->onSceneExit();
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
    // TODO: Implement transition effects (fade, slide, etc.)
}

// ═══════════════════════════════════════════════════════════════════════════
// LEVEL MANAGEMENT
// ═══════════════════════════════════════════════════════════════════════════

void World::addLevel(std::unique_ptr<Level> level) {
    if (!level) return;
    
    std::string levelId = level->getId();
    m_levelMap[levelId] = level.get();
    m_levels.push_back(std::move(level));
}

void World::removeLevel(const std::string& levelId) {
    auto it = m_levelMap.find(levelId);
    if (it == m_levelMap.end()) return;
    
    Level* levelPtr = it->second;
    m_levelMap.erase(it);
    
    // Remove from vector
    for (auto vecIt = m_levels.begin(); vecIt != m_levels.end(); ++vecIt) {
        if (vecIt->get() == levelPtr) {
            m_levels.erase(vecIt);
            
            // Clear active level if removed
            if (m_activeLevel == levelPtr) {
                m_activeLevel = nullptr;
            }
            break;
        }
    }
}

Level* World::getLevel(const std::string& levelId) const {
    auto it = m_levelMap.find(levelId);
    return (it != m_levelMap.end()) ? it->second : nullptr;
}

void World::setActiveLevel(const std::string& levelId) {
    auto it = m_levelMap.find(levelId);
    if (it != m_levelMap.end()) {
        m_activeLevel = it->second;
    }
}

void World::transitionToLevel(const std::string& levelId) {
    Level* newLevel = getLevel(levelId);
    if (!newLevel) return;
    
    // Exit old level
    if (m_activeLevel) {
        m_activeLevel->onLevelExit();
    }
    
    // Switch
    m_activeLevel = newLevel;
    
    // Enter new level
    if (m_activeLevel) {
        m_activeLevel->onLevelEnter();
    }
}

// Removed transitionToScene - use changeScene() instead with scene stack

} // namespace engine
