/**
 * @file EditorPlayMode.h
 * @brief Editor Play Mode - Unity/Unreal style play-in-editor
 * 
 * Handles Play/Pause/Stop functionality for testing games directly in editor.
 * Manages physics simulation, game logic, and scene state preservation.
 */
#pragma once

#include "engine/physics/PhysicsManager.h"
#include <memory>
#include <functional>
#include <string>

namespace engine {
    class World;
    class Scene;
    class Player3DActor;
    class PlayerStartActor;
}

namespace editor {

/**
 * @brief Play mode states
 */
enum class PlayState {
    Stopped,    // Editor mode - no simulation
    Playing,    // Game running
    Paused      // Game paused but state preserved
};

/**
 * @brief Editor Play Mode Manager
 * 
 * Usage:
 *   playMode.play();   // Start simulation
 *   playMode.pause();  // Pause simulation
 *   playMode.stop();   // Stop and restore scene
 */
class EditorPlayMode {
public:
    EditorPlayMode();
    ~EditorPlayMode();
    
    // ========================================================================
    // INITIALIZATION
    // ========================================================================
    
    void initialize();
    void shutdown();
    
    void setWorld(engine::World* world) { m_world = world; }
    void setActiveScene(engine::Scene* scene) { m_activeScene = scene; }
    
    // ========================================================================
    // PLAY CONTROLS
    // ========================================================================
    
    void play();
    void pause();
    void stop();
    void step();  // Single frame step (when paused)
    
    // ========================================================================
    // STATE
    // ========================================================================
    
    PlayState getState() const { return m_state; }
    bool isPlaying() const { return m_state == PlayState::Playing; }
    bool isPaused() const { return m_state == PlayState::Paused; }
    bool isStopped() const { return m_state == PlayState::Stopped; }
    
    // ========================================================================
    // UPDATE
    // ========================================================================
    
    void update(float deltaTime);
    
    // ========================================================================
    // PHYSICS ACCESS
    // ========================================================================
    
    engine::physics::PhysicsManager* getPhysicsManager() { return &m_physicsManager; }
    engine::physics::PhysicsWorld3D* getPhysicsWorld3D() { return m_physicsManager.getWorld3D(); }
    engine::physics::PhysicsWorld2D* getPhysicsWorld2D() { return m_physicsManager.getWorld2D(); }
    
    bool isGPUAccelerationAvailable() const { return m_physicsManager.isGPUAccelerationAvailable(); }
    
    // ========================================================================
    // CALLBACKS
    // ========================================================================
    
    std::function<void(PlayState)> onStateChanged;
    std::function<void()> onPlayStarted;
    std::function<void()> onPlayStopped;
    
    // ========================================================================
    // SETTINGS
    // ========================================================================
    
    void setTimeScale(float scale) { m_timeScale = scale; }
    float getTimeScale() const { return m_timeScale; }
    
    void setMaxDeltaTime(float maxDt) { m_maxDeltaTime = maxDt; }
    float getMaxDeltaTime() const { return m_maxDeltaTime; }
    
    // ========================================================================
    // STATS
    // ========================================================================
    
    float getPlayTime() const { return m_playTime; }
    int getFrameCount() const { return m_frameCount; }
    float getAverageFPS() const { return m_frameCount > 0 ? m_frameCount / m_playTime : 0.0f; }
    
    // Player access (for viewport rendering)
    engine::Player3DActor* getPlayer() const { return m_player.get(); }
    
    // Player input
    void handleMouseLook(float deltaX, float deltaY);
    
private:
    void saveSceneState();
    void restoreSceneState();
    void initializePhysicsBodies();
    void cleanupPhysicsBodies();
    void createTestPhysicsActor();
    void spawnPlayerAtStart();
    void updatePlayer(float deltaTime);
    engine::PlayerStartActor* findPlayerStart() const;
    
    // State
    PlayState m_state = PlayState::Stopped;
    engine::World* m_world = nullptr;
    engine::Scene* m_activeScene = nullptr;
    
    // Physics
    engine::physics::PhysicsManager m_physicsManager;
    
    // Timing
    float m_timeScale = 1.0f;
    float m_maxDeltaTime = 1.0f / 30.0f;
    float m_playTime = 0.0f;
    int m_frameCount = 0;
    
    // Scene state backup (for restoring after stop)
    std::string m_sceneStateBackup;
    bool m_hasBackup = false;
    
    // Test physics body (for demonstration)
    void* m_testBody = nullptr;
    
    // Ground plane for physics simulation
    void* m_groundPlane = nullptr;
    
    // Player character (spawned at PlayerStart)
    std::unique_ptr<engine::Player3DActor> m_player;
};

} // namespace editor
