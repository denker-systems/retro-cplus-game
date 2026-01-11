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
class RuntimeDialogSystem;
class RuntimeQuestSystem;
class RuntimeInventory;

#ifdef HAS_IMGUI
// Full include needed for unique_ptr destructor
#include "engine/ui/GameDialogWidget.h"
#endif

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
    
    // Scene/Level management
    void changeScene(const std::string& sceneId);
    void changeLevel(const std::string& levelId, const std::string& sceneId);
    void handleInteraction();
    std::string getSceneBackground() const;
    
    // Core systems
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_glContext = nullptr;
    bool m_running = false;
    Uint32 m_lastFrameTime = 0;
    
    // Game world
    std::unique_ptr<RuntimeWorld> m_world;
    engine::Scene* m_activeScene = nullptr;
    std::string m_activeSceneBackground;
    
    // Dialog state
    bool m_inDialog = false;
    std::string m_currentDialogId;
    
    // UI state
    bool m_showQuestLog = false;
    
    // Physics
    std::unique_ptr<engine::physics::PhysicsManager> m_physics;
    
    // Player
    std::unique_ptr<engine::Player3DActor> m_player;
    std::unique_ptr<editor::EditorCamera3D> m_camera;
    
    // Input
    std::unique_ptr<Input> m_input;
    
    // Renderer
    std::unique_ptr<RuntimeRenderer> m_renderer;
    
    // Game systems
    std::unique_ptr<RuntimeDialogSystem> m_dialogSystem;
    std::unique_ptr<RuntimeQuestSystem> m_questSystem;
    std::unique_ptr<RuntimeInventory> m_inventory;
    
    // UI widgets
#ifdef HAS_IMGUI
    std::unique_ptr<GameDialogWidget> m_dialogWidget;
#endif
    
    // Window settings (dynamic, set at runtime)
    int m_windowWidth = 1920;
    int m_windowHeight = 1080;
};
