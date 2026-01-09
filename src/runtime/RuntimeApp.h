/**
 * @file RuntimeApp.h
 * @brief Retro Game Runtime Application - Ren 3D player
 * 
 * Laddar och kör editor-skapad content. Ingen legacy 2D-kod, bara 3D från grunden.
 */
#pragma once

#include <SDL.h>
#include <memory>
#include <string>

// Forward declarations
namespace engine {
    class Scene;
    class Player3DActor;
    namespace physics {
        class PhysicsManager;
    }
}

namespace editor {
    class EditorCamera3D;
}

class Input;
class RuntimeWorld;
class RuntimeRenderer;

/**
 * @brief Runtime application för RetroGame.exe
 * 
 * Ansvar:
 * - SDL + OpenGL initialization
 * - PhysicsManager (PhysX 3D)
 * - World/Scene loading från JSON
 * - Player spawning vid PlayerStart
 * - Input handling (WASD + mouse look)
 * - 3D rendering
 * - Game loop
 */
class RuntimeApp {
public:
    RuntimeApp();
    ~RuntimeApp();
    
    /**
     * @brief Initiera SDL, OpenGL, Physics och ladda world
     * @return true om initiering lyckades
     */
    bool init();
    
    /**
     * @brief Kör game loop (blockerar tills spelet avslutas)
     */
    void run();
    
    /**
     * @brief Frigör alla resurser
     */
    void shutdown();

private:
    // Initialization
    bool initSDL();
    bool initOpenGL();
    bool initPhysics();
    bool loadWorld();
    void spawnPlayer();
    
    // Game loop
    void handleInput();
    void update(float dt);
    void render();
    
    // Core systems
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    bool m_running = false;
    Uint32 m_lastFrameTime = 0;
    
    // Game world
    std::unique_ptr<RuntimeWorld> m_world;
    engine::Scene* m_activeScene = nullptr;
    
    // Physics
    std::unique_ptr<engine::physics::PhysicsManager> m_physics;
    
    // Player
    std::unique_ptr<engine::Player3DActor> m_player;
    std::unique_ptr<editor::EditorCamera3D> m_camera;
    
    // Input
    std::unique_ptr<Input> m_input;
    
    // Renderer
    std::unique_ptr<RuntimeRenderer> m_renderer;
    
    // Window settings
    static constexpr int WINDOW_WIDTH = 1920;
    static constexpr int WINDOW_HEIGHT = 1080;
};
