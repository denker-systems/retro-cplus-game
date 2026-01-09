/**
 * @file Level.cpp
 * @brief Implementation of Level class
 */
#include "Level.h"
#include "Scene.h"

namespace engine {

// Constructors moved to header (inline)

void Level::addScene(std::unique_ptr<Scene> scene) {
    if (!scene) return;
    
    std::string sceneId = scene->getName();
    m_sceneMap[sceneId] = scene.get();
    m_scenes.push_back(std::move(scene));
}

Scene* Level::getScene(const std::string& sceneId) const {
    auto it = m_sceneMap.find(sceneId);
    return (it != m_sceneMap.end()) ? it->second : nullptr;
}

std::unique_ptr<Scene> Level::removeScene(const std::string& sceneId) {
    auto it = m_sceneMap.find(sceneId);
    if (it == m_sceneMap.end()) {
        return nullptr;
    }
    
    Scene* scenePtr = it->second;
    m_sceneMap.erase(it);
    
    // Find and remove from vector
    for (auto vecIt = m_scenes.begin(); vecIt != m_scenes.end(); ++vecIt) {
        if (vecIt->get() == scenePtr) {
            std::unique_ptr<Scene> scene = std::move(*vecIt);
            m_scenes.erase(vecIt);
            
            // Clear active scene if it was removed
            if (m_activeScene == scenePtr) {
                m_activeScene = nullptr;
            }
            
            return scene;
        }
    }
    
    return nullptr;
}

void Level::setActiveScene(const std::string& sceneId) {
    Scene* scene = getScene(sceneId);
    if (scene) {
        m_activeScene = scene;
    }
}

void Level::transitionToScene(const std::string& sceneId) {
    Scene* newScene = getScene(sceneId);
    if (!newScene) return;
    
    // Exit old scene
    if (m_activeScene) {
        m_activeScene->onSceneExit();
    }
    
    // Switch
    m_activeScene = newScene;
    
    // Enter new scene
    if (m_activeScene) {
        m_activeScene->onSceneEnter();
    }
}

void Level::update(float deltaTime) {
    // Update Level-level actors first (chapter-wide actors)
    updateActors(deltaTime);
    
    // Then update active scene
    if (m_activeScene) {
        m_activeScene->update(deltaTime);
    }
}

void Level::render(SDL_Renderer* renderer) {
    // Render Level-level actors first (chapter-wide actors)
    renderActors(renderer);
    
    // Then render active scene
    if (m_activeScene) {
        m_activeScene->render(renderer);
    }
}

void Level::onLevelEnter() {
    // Default: activate first scene if no active scene
    if (!m_activeScene && !m_scenes.empty()) {
        transitionToScene(m_scenes[0]->getName());
    }
}

void Level::onLevelExit() {
    // Exit active scene
    if (m_activeScene) {
        m_activeScene->onSceneExit();
    }
}

} // namespace engine
