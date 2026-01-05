/**
 * @file Scene.h
 * @brief Scene container for game levels/rooms
 */
#pragma once

#include "core/Node.h"
#include "Camera2D.h"
#include <string>
#include <memory>

namespace engine {

/**
 * @brief Container for a game level/room
 * 
 * A Scene is the root node for a level. It contains:
 * - All game objects (as child nodes)
 * - Active camera
 * - Scene-specific logic
 */
class Scene : public Node {
public:
    Scene();
    explicit Scene(const std::string& name);
    virtual ~Scene() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // CAMERA
    // ═══════════════════════════════════════════════════════════════════
    
    Camera2D* getActiveCamera() const { return m_activeCamera; }
    void setActiveCamera(Camera2D* camera) { m_activeCamera = camera; }
    
    /** @brief Create and set a default camera */
    Camera2D* createDefaultCamera();
    
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
    
protected:
    Camera2D* m_activeCamera = nullptr;
    bool m_isPaused = false;
};

} // namespace engine
