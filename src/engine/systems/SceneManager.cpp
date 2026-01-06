/**
 * @file SceneManager.cpp
 * @brief Implementation av SceneManager
 */
#include "SceneManager.h"
#include "utils/Logger.h"
#include <iostream>

SceneManager& SceneManager::instance() {
    static SceneManager instance;
    return instance;
}

void SceneManager::addScene(std::unique_ptr<engine::Scene> scene) {
    std::string id = scene->getId();
    LOG_INFO("Adding scene: " + id);
    m_scenes[id] = std::move(scene);
}

void SceneManager::clearScenes() {
    LOG_INFO("Clearing all scenes for hot reload");
    m_currentScene = nullptr;
    m_scenes.clear();
}

engine::Scene* SceneManager::getScene(const std::string& sceneId) {
    auto it = m_scenes.find(sceneId);
    if (it == m_scenes.end()) {
        LOG_WARNING("Scene not found: " + sceneId);
        return nullptr;
    }
    return it->second.get();
}

bool SceneManager::changeScene(const std::string& sceneId) {
    LOG_INFO("changeScene called: " + sceneId);
    engine::Scene* newScene = getScene(sceneId);
    if (!newScene) {
        LOG_ERROR("Failed to change scene - not found: " + sceneId);
        return false;
    }
    
    LOG_INFO("Changing to scene: " + sceneId);
    m_currentScene = newScene;
    
    if (m_onSceneChange) {
        LOG_DEBUG("Calling onSceneChange callback");
        m_onSceneChange(sceneId);
    }
    
    return true;
}
