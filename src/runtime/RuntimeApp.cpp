/**
 * @file RuntimeApp.cpp
 * @brief Implementation av Runtime Application
 */
#include "RuntimeApp.h"
#include "RuntimeWorld.h"
#include "RuntimeRenderer.h"
#include "engine/world/Scene.h"
#include "engine/actors/Character3DActor.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/physics/PhysicsManager.h"
#include "editor/viewport/3d/EditorCamera3D.h"
#include "engine/input/Input.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"
#include <GL/glew.h>
#include <iostream>

RuntimeApp::RuntimeApp() = default;

RuntimeApp::~RuntimeApp() {
    shutdown();
}

bool RuntimeApp::init() {
    LOG_INFO("=== RetroGame Runtime Starting ===");
    
    if (!initSDL()) {
        LOG_ERROR("Failed to initialize SDL");
        return false;
    }
    
    if (!initOpenGL()) {
        LOG_ERROR("Failed to initialize OpenGL");
        return false;
    }
    
    if (!initPhysics()) {
        LOG_ERROR("Failed to initialize Physics");
        return false;
    }
    
    // Load ALL game data (scenes, dialogs, quests, items, npcs)
    LOG_INFO("[RuntimeApp] Loading game data...");
    auto& dataLoader = DataLoader::instance();
    dataLoader.loadAll("assets/data/");
    LOG_INFO("[RuntimeApp] Loaded: " + 
             std::to_string(dataLoader.getRooms().size()) + " scenes, " +
             std::to_string(dataLoader.getDialogs().size()) + " dialogs, " +
             std::to_string(dataLoader.getQuests().size()) + " quests, " +
             std::to_string(dataLoader.getItems().size()) + " items, " +
             std::to_string(dataLoader.getNPCs().size()) + " npcs");
    
    if (!loadWorld()) {
        LOG_ERROR("Failed to load world");
        return false;
    }
    
    spawnPlayer();
    
    // Initialize renderer
    m_renderer = std::make_unique<RuntimeRenderer>();
    if (!m_renderer->initialize()) {
        LOG_ERROR("Failed to initialize renderer");
        return false;
    }
    
    m_running = true;
    m_lastFrameTime = SDL_GetTicks();
    
    LOG_INFO("=== RetroGame Runtime Initialized ===");
    return true;
}

void RuntimeApp::run() {
    LOG_INFO("=== Starting Game Loop ===");
    
    int frameCount = 0;
    
    while (m_running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
        m_lastFrameTime = currentTime;
        
        // Cap delta time to prevent physics explosions
        if (deltaTime > 0.1f) {
            deltaTime = 0.1f;
        }
        
        // Debug first 5 frames
        if (frameCount < 5) {
            LOG_INFO("[RuntimeApp] Frame " + std::to_string(frameCount) + " - handleInput...");
        }
        handleInput();
        
        if (frameCount < 5) {
            LOG_INFO("[RuntimeApp] Frame " + std::to_string(frameCount) + " - update...");
        }
        update(deltaTime);
        
        if (frameCount < 5) {
            LOG_INFO("[RuntimeApp] Frame " + std::to_string(frameCount) + " - render...");
        }
        render();
        
        frameCount++;
        
        // Log every 60 frames
        if (frameCount % 60 == 0) {
            LOG_INFO("[RuntimeApp] Frame " + std::to_string(frameCount) + ", FPS: " + std::to_string(1.0f / deltaTime));
        }
        
        // Cap at 60 FPS
        Uint32 frameTime = SDL_GetTicks() - currentTime;
        if (frameTime < 16) {
            SDL_Delay(16 - frameTime);
        }
    }
    
    LOG_INFO("=== Game Loop Ended (" + std::to_string(frameCount) + " frames) ===");
}

void RuntimeApp::shutdown() {
    LOG_INFO("=== RetroGame Runtime Shutting Down ===");
    
    // Cleanup in reverse order of initialization
    m_renderer.reset();
    m_player.reset();
    m_camera.reset();
    m_input.reset();
    m_physics.reset();
    m_world.reset();
    
    if (m_glContext) {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = nullptr;
    }
    
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    
    SDL_Quit();
    
    LOG_INFO("=== RetroGame Runtime Shutdown Complete ===");
}

bool RuntimeApp::initSDL() {
    LOG_INFO("[RuntimeApp] Initializing SDL...");
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0) {
        LOG_ERROR("SDL_Init failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Set OpenGL attributes BEFORE window creation
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    // Create window
    m_window = SDL_CreateWindow(
        "Retro Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    
    if (!m_window) {
        LOG_ERROR("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Create OpenGL context
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        LOG_ERROR("SDL_GL_CreateContext failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    LOG_INFO("[RuntimeApp] SDL initialized successfully");
    return true;
}

bool RuntimeApp::initOpenGL() {
    LOG_INFO("[RuntimeApp] Initializing OpenGL...");
    
    // Initialize GLEW
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        LOG_ERROR("GLEW initialization failed: " + std::string((const char*)glewGetErrorString(err)));
        return false;
    }
    
    // Log OpenGL info
    LOG_INFO("OpenGL Version: " + std::string((const char*)glGetString(GL_VERSION)));
    LOG_INFO("GLSL Version: " + std::string((const char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
    LOG_INFO("Renderer: " + std::string((const char*)glGetString(GL_RENDERER)));
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Enable backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    
    // Set clear color (dark blue)
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    
    LOG_INFO("[RuntimeApp] OpenGL initialized successfully");
    return true;
}

bool RuntimeApp::initPhysics() {
    LOG_INFO("[RuntimeApp] Initializing Physics...");
    
    m_physics = std::make_unique<engine::physics::PhysicsManager>();
    m_physics->initialize3D();  // Only 3D physics (PhysX)
    
    if (m_physics->isGPUAccelerationAvailable()) {
        LOG_INFO("[RuntimeApp] PhysX GPU acceleration available");
    } else {
        LOG_INFO("[RuntimeApp] PhysX running on CPU");
    }
    
    LOG_INFO("[RuntimeApp] Physics initialized successfully");
    return true;
}

bool RuntimeApp::loadWorld() {
    LOG_INFO("[RuntimeApp] Loading world...");
    
    m_world = std::make_unique<RuntimeWorld>();
    
    if (!m_world->loadFromJSON(m_physics.get())) {
        LOG_ERROR("[RuntimeApp] Failed to load world from JSON");
        return false;
    }
    
    m_activeScene = m_world->getActiveScene();
    
    if (!m_activeScene) {
        LOG_ERROR("[RuntimeApp] No active scene after loading world");
        return false;
    }
    
    LOG_INFO("[RuntimeApp] World loaded successfully - Active scene: " + m_activeScene->getName());
    return true;
}

void RuntimeApp::spawnPlayer() {
    LOG_INFO("[RuntimeApp] Spawning player...");
    
    if (!m_activeScene) {
        LOG_ERROR("Cannot spawn player: No active scene");
        return;
    }
    
    // Find PlayerStart in scene
    engine::PlayerStartActor* playerStart = nullptr;
    for (const auto& actor : m_activeScene->getActors()) {
        if (auto* ps = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
            playerStart = ps;
            break;
        }
    }
    
    if (!playerStart) {
        LOG_WARNING("No PlayerStart found, using default position (0, 2, 0)");
    }
    
    // Create player
    m_player = std::make_unique<engine::Player3DActor>();
    
    // Get spawn position from PlayerStart
    glm::vec3 spawnPos(0.0f, 2.0f, 0.0f);  // Default
    if (playerStart) {
        spawnPos = playerStart->getSpawnPosition();
    }
    
    m_player->setPosition3D(spawnPos);
    m_player->initializeController(m_physics->getWorld3D());
    
    // Create camera
    m_camera = std::make_unique<editor::EditorCamera3D>();
    m_camera->setPosition(spawnPos + glm::vec3(0, 3, 5));
    
    // Create input handler
    m_input = std::make_unique<Input>();
    
    LOG_INFO("[RuntimeApp] Player spawned at (" + 
             std::to_string(spawnPos.x) + ", " + 
             std::to_string(spawnPos.y) + ", " + 
             std::to_string(spawnPos.z) + ")");
}

void RuntimeApp::handleInput() {
    static bool mouseGrabbed = false;
    static int lastMouseX = 0, lastMouseY = 0;
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_running = false;
        }
        
        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                m_running = false;
            }
        }
        
        // Mouse button for mouse look
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.button.button == SDL_BUTTON_RIGHT) {
                mouseGrabbed = true;
                SDL_SetRelativeMouseMode(SDL_TRUE);
                SDL_GetMouseState(&lastMouseX, &lastMouseY);
            }
        }
        
        if (event.type == SDL_MOUSEBUTTONUP) {
            if (event.button.button == SDL_BUTTON_RIGHT) {
                mouseGrabbed = false;
                SDL_SetRelativeMouseMode(SDL_FALSE);
            }
        }
        
        // Mouse motion for look
        if (event.type == SDL_MOUSEMOTION && mouseGrabbed && m_player) {
            float deltaX = event.motion.xrel * 0.1f;
            float deltaY = event.motion.yrel * 0.1f;
            
            m_player->addYaw(deltaX);  // Yaw rotation
        }
    }
    
    // Update input state
    if (m_input) {
        m_input->update();
    }
    
    // Handle WASD movement
    if (m_player && m_input) {
        float forward = 0.0f;
        float right = 0.0f;
        
        if (m_input->isKeyDown(SDL_SCANCODE_W)) forward += 1.0f;
        if (m_input->isKeyDown(SDL_SCANCODE_S)) forward -= 1.0f;
        if (m_input->isKeyDown(SDL_SCANCODE_D)) right += 1.0f;
        if (m_input->isKeyDown(SDL_SCANCODE_A)) right -= 1.0f;
        
        // Jump
        if (m_input->isKeyPressed(SDL_SCANCODE_SPACE)) {
            m_player->jump();
        }
        
        // Set movement direction (convert to 3D vector)
        glm::vec3 moveInput(right, 0.0f, -forward);  // X=right, Z=forward (negated for correct direction)
        m_player->setMoveInput(moveInput);
    }
}

void RuntimeApp::update(float dt) {
    // Update physics
    if (m_physics) {
        m_physics->step(dt);
    }
    
    // Update player
    if (m_player) {
        m_player->update(dt);
    }
    
    // Update camera - editor-style isometric view
    if (m_camera && m_player) {
        glm::vec3 playerPos = m_player->getPosition3D();
        
        // Editor-style camera: elevated, looking down at scene
        // Position camera above and behind, looking down at an angle
        glm::vec3 cameraOffset(0.0f, 8.0f, 8.0f);  // High and back
        
        glm::vec3 targetPos = playerPos + cameraOffset;
        
        // Smooth camera follow
        glm::vec3 currentPos = m_camera->getPosition();
        glm::vec3 newPos = glm::mix(currentPos, targetPos, 5.0f * dt);
        
        m_camera->setPosition(newPos);
        m_camera->setTarget(playerPos);  // Look at player
    }
    
    // Update scene actors
    if (m_activeScene) {
        for (const auto& actor : m_activeScene->getActors()) {
            actor->update(dt);
        }
    }
}

void RuntimeApp::render() {
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!m_camera || !m_player || !m_renderer) {
        SDL_GL_SwapWindow(m_window);
        return;
    }
    
    // Render 3D scene
    m_renderer->render(m_camera.get(), m_player.get(), m_activeScene);
    
    // Swap buffers
    SDL_GL_SwapWindow(m_window);
}
