/**
 * @file Scene.h
 * @brief Scene - Bottom of hierarchy (UE5-style)
 * 
 * Hierarchy: World → Level → Scene → Actors
 * Scene = ULevel in UE5
 */
#pragma once

#include "WorldContainer.h"
#include "GridTypes.h"
#include "engine/components/CameraComponent.h"
#include <string>
#include <memory>
#include <vector>
#include <SDL.h>

namespace engine {

/**
 * @brief Scene type for categorization and filtering
 */
enum class SceneType {
    // Indoor locations
    Interior,       // Generic indoor (tavern, shop, house)
    Cellar,         // Underground indoor
    
    // Outdoor locations  
    Town,           // Streets, squares, marketplaces
    Forest,         // Woods, paths
    Cave,           // Natural underground
    
    // Special environments
    Underwater,     // Underwater sections
    Dream,          // Dream sequences, visions
    
    // Technical
    Cutscene,       // Non-interactive scenes
    Menu            // UI scenes
};

/** @brief Get display name for SceneType */
inline const char* getSceneTypeName(SceneType type) {
    switch (type) {
        case SceneType::Interior:   return "Interior";
        case SceneType::Cellar:     return "Cellar";
        case SceneType::Town:       return "Town";
        case SceneType::Forest:     return "Forest";
        case SceneType::Cave:       return "Cave";
        case SceneType::Underwater: return "Underwater";
        case SceneType::Dream:      return "Dream";
        case SceneType::Cutscene:   return "Cutscene";
        case SceneType::Menu:       return "Menu";
        default:                    return "Unknown";
    }
}

/** @brief Get icon for SceneType (for UI display) */
inline const char* getSceneTypeIcon(SceneType type) {
    switch (type) {
        case SceneType::Interior:   return "[I]";   // 🏠
        case SceneType::Cellar:     return "[B]";   // 🪜
        case SceneType::Town:       return "[T]";   // 🏘️
        case SceneType::Forest:     return "[F]";   // 🌲
        case SceneType::Cave:       return "[C]";   // 🕳️
        case SceneType::Underwater: return "[U]";   // 🌊
        case SceneType::Dream:      return "[D]";   // 💭
        case SceneType::Cutscene:   return "[X]";   // 🎬
        case SceneType::Menu:       return "[M]";   // 📋
        default:                    return "[?]";
    }
}

/**
 * @brief Scene/Room - Contains Actors
 * 
 * Inherits from WorldContainer
 * Child of Level, Contains Actors
 * Equivalent to ULevel in UE5
 */
class Scene : public WorldContainer {
public:
    Scene() : WorldContainer("Scene") {}
    explicit Scene(const std::string& name) : WorldContainer(name) {}
    Scene(const std::string& name, SceneType type) : WorldContainer(name), m_sceneType(type) {}
    virtual ~Scene() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // getName/setName inherited from WorldContainer
    
    // ═══════════════════════════════════════════════════════════════════
    // SCENE TYPE
    // ═══════════════════════════════════════════════════════════════════
    
    SceneType getSceneType() const { return m_sceneType; }
    void setSceneType(SceneType type) { m_sceneType = type; }
    const char* getSceneTypeName() const { return engine::getSceneTypeName(m_sceneType); }
    const char* getSceneTypeIcon() const { return engine::getSceneTypeIcon(m_sceneType); }
    
    // ═══════════════════════════════════════════════════════════════════
    // CAMERA (NEW: Component-based)
    // ═══════════════════════════════════════════════════════════════════
    
    CameraComponent* getActiveCamera() const { return m_activeCamera; }
    void setActiveCamera(CameraComponent* camera) { m_activeCamera = camera; }
    
    /** @brief Create and set a default camera actor with camera component */
    CameraComponent* createDefaultCamera();
    
    // ═══════════════════════════════════════════════════════════════════
    // SCENE LIFECYCLE
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Called when scene becomes active */
    virtual void onSceneEnter();
    
    /** @brief Called when scene becomes inactive */
    virtual void onSceneExit();
    
    /** @brief Called when scene is paused (overlay pushed) */
    virtual void onScenePause();
    
    /** @brief Called when scene is resumed (overlay popped) */
    virtual void onSceneResume();
    
    // addActor/getActors/findActor inherited from WorldContainer
    // getGridPosition/setGridPosition inherited from WorldContainer
    
    // ═══════════════════════════════════════════════════════════════════
    // CAMERA CONFIG (Scene-specific)
    // ═══════════════════════════════════════════════════════════════════
    
    const CameraConfig& getCameraConfig() const { return m_cameraConfig; }
    void setCameraConfig(const CameraConfig& config) { m_cameraConfig = config; }
    
private:
    bool m_isPaused = false;
    SceneType m_sceneType = SceneType::Interior;  // Default type
    
    // Camera (actors inherited from WorldContainer)
    CameraComponent* m_activeCamera = nullptr;
    std::unique_ptr<ActorObjectExtended> m_cameraActor;
    
    // Scene-specific config
    CameraConfig m_cameraConfig;
};

} // namespace engine
