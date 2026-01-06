/**
 * @file Scene.h
 * @brief Scene - Bottom of hierarchy (UE5-style)
 * 
 * Hierarchy: World → Level → Scene → Actors
 * Scene = ULevel in UE5
 */
#pragma once

#include "WorldContainer.h"
#include "engine/components/CameraComponent.h"
#include <string>
#include <memory>
#include <vector>
#include <SDL.h>

namespace engine {

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
    virtual ~Scene() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // getName/setName inherited from WorldContainer
    
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
    
private:
    bool m_isPaused = false;
    
    // Camera (actors inherited from WorldContainer)
    CameraComponent* m_activeCamera = nullptr;
    std::unique_ptr<ActorObjectExtended> m_cameraActor;
};

} // namespace engine
