/**
 * @file SceneManager.h
 * @brief Hanterar scener och sceneövergångar
 */
#pragma once

#include "Scene.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>

/**
 * @brief Singleton för scenehantering
 */
class SceneManager {
public:
    static SceneManager& instance();
    
    /** @brief Registrera en scene */
    void addScene(std::unique_ptr<Scene> scene);
    
    /** @brief Rensa alla scener (för hot reload) */
    void clearScenes();
    
    /** @brief Hämta scene med ID */
    Scene* getScene(const std::string& sceneId);
    
    /** @brief Byt till annan scene */
    bool changeScene(const std::string& sceneId);
    
    /** @brief Hämta aktuell scene */
    Scene* getCurrentScene() { return m_currentScene; }
    std::string getCurrentSceneId() const { return m_currentScene ? m_currentScene->getId() : ""; }
    
    /** @brief Sätt callback för scenebyte */
    void setOnSceneChange(std::function<void(const std::string&)> callback) {
        m_onSceneChange = callback;
    }
    
    /** @brief Hämta spelarens startposition i aktuell scene */
    void getSpawnPosition(float& x, float& y) const {
        x = m_spawnX;
        y = m_spawnY;
    }
    
    /** @brief Sätt spawn-position för nästa scenebyte */
    void setSpawnPosition(float x, float y) {
        m_spawnX = x;
        m_spawnY = y;
    }

private:
    SceneManager() = default;
    
    std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;
    Scene* m_currentScene = nullptr;
    std::function<void(const std::string&)> m_onSceneChange;
    
    float m_spawnX = 320.0f;
    float m_spawnY = 300.0f;
};
