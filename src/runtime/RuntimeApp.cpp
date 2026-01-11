/**
 * @file RuntimeApp.cpp
 * @brief Implementation av Runtime Application
 */
#include "RuntimeApp.h"
#include "RuntimeWorld.h"
#include "RuntimeRenderer.h"
#include "RuntimeDialogSystem.h"
#include "RuntimeQuestSystem.h"
#include "RuntimeInventory.h"
#include "engine/world/Scene.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/actors/Character3DActor.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/NPC3DActor.h"
#include "engine/physics/PhysicsManager.h"
#include "editor/viewport/3d/EditorCamera3D.h"
#include "engine/input/Input.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"
#include <GL/glew.h>
#include <iostream>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_opengl3.h"
#endif

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
    
    // Initialize game systems
    m_dialogSystem = std::make_unique<RuntimeDialogSystem>();
    m_dialogSystem->loadDialogs();
    
    m_questSystem = std::make_unique<RuntimeQuestSystem>();
    m_questSystem->loadQuests();
    
    m_inventory = std::make_unique<RuntimeInventory>();
    m_inventory->loadItemDefinitions();
    
#ifdef HAS_IMGUI
    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
    ImGui_ImplOpenGL3_Init("#version 330");
    
    // Create dialog widget
    m_dialogWidget = std::make_unique<GameDialogWidget>();
    LOG_INFO("[RuntimeApp] ImGui initialized for dialog UI");
#endif
    
    // Set up callbacks
    m_dialogSystem->setActionCallback([this](const std::string& action) {
        LOG_INFO("[RuntimeApp] Dialog action: " + action);
        // Handle quest triggers, item rewards etc
    });
    
    m_questSystem->setQuestCompleteCallback([this](const Quest& quest) {
        LOG_INFO("[RuntimeApp] Quest complete: " + quest.title);
        if (!quest.rewardItem.empty()) {
            m_inventory->addItem(quest.rewardItem);
        }
    });
    
    m_inventory->setOnItemAdded([this](const Item& item) {
        m_questSystem->onCollect(item.id);
    });
    
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
    
#ifdef HAS_IMGUI
    // Shutdown ImGui
    m_dialogWidget.reset();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif
    
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
    
    // Get display resolution for fullscreen
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {
        m_windowWidth = displayMode.w;
        m_windowHeight = displayMode.h;
        LOG_INFO("[RuntimeApp] Display resolution: " + std::to_string(m_windowWidth) + "x" + std::to_string(m_windowHeight));
    } else {
        LOG_WARNING("[RuntimeApp] Could not get display mode, using default 1920x1080");
        m_windowWidth = 1920;
        m_windowHeight = 1080;
    }
    
    // Set OpenGL attributes BEFORE window creation
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    // Create fullscreen desktop window
    m_window = SDL_CreateWindow(
        "Retro Game",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_windowWidth,
        m_windowHeight,
        SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP
    );
    
    if (!m_window) {
        LOG_ERROR("SDL_CreateWindow failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Get actual window size (may differ on high-DPI displays)
    SDL_GL_GetDrawableSize(m_window, &m_windowWidth, &m_windowHeight);
    LOG_INFO("[RuntimeApp] Actual drawable size: " + std::to_string(m_windowWidth) + "x" + std::to_string(m_windowHeight));
    
    // Create OpenGL context
    m_glContext = SDL_GL_CreateContext(m_window);
    if (!m_glContext) {
        LOG_ERROR("SDL_GL_CreateContext failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Enable VSync
    SDL_GL_SetSwapInterval(1);
    
    LOG_INFO("[RuntimeApp] SDL initialized successfully (fullscreen)");
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
    
    // Get initial background
    auto& dataLoader = DataLoader::instance();
    for (const auto& room : dataLoader.getRooms()) {
        if (room.id == m_activeScene->getName() || room.name == m_activeScene->getName()) {
            m_activeSceneBackground = room.background;
            LOG_INFO("[RuntimeApp] Scene background: " + m_activeSceneBackground);
            break;
        }
    }
    
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
    
    // Update input state FIRST (copy current to previous)
    if (m_input) {
        m_input->update();
    }
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
#ifdef HAS_IMGUI
        // Forward to ImGui
        ImGui_ImplSDL2_ProcessEvent(&event);
#endif
        
        // Forward to input system for key state tracking
        if (m_input) {
            m_input->handleEvent(event);
        }
        
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
        
        // Interact (E key)
        if (m_input->isKeyPressed(SDL_SCANCODE_E)) {
            handleInteraction();
        }
        
        // Toggle Quest Log (J key)
        if (m_input->isKeyPressed(SDL_SCANCODE_J)) {
            m_showQuestLog = !m_showQuestLog;
            LOG_INFO("[RuntimeApp] Quest Log: " + std::string(m_showQuestLog ? "OPEN" : "CLOSED"));
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
    // Set viewport to current window size
    glViewport(0, 0, m_windowWidth, m_windowHeight);
    
    // Clear screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (!m_camera || !m_player || !m_renderer) {
        SDL_GL_SwapWindow(m_window);
        return;
    }
    
    // Get exits for current scene
    std::vector<ExitData> currentExits;
    if (m_world && m_activeScene) {
        std::string sceneId = m_activeScene->getName();
        std::transform(sceneId.begin(), sceneId.end(), sceneId.begin(), ::tolower);
        currentExits = m_world->getExitsForScene(sceneId);
    }
    
    // Render 3D scene with background and exits
    m_renderer->render(m_camera.get(), m_player.get(), m_activeScene, m_activeSceneBackground, true, &currentExits);
    
#ifdef HAS_IMGUI
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    // Render dialog UI if dialog is active
    if (m_dialogSystem && m_dialogSystem->isActive() && m_dialogWidget) {
        // Convert choices to widget format
        std::vector<std::pair<int, std::string>> choices;
        for (const auto& choice : m_dialogSystem->getCurrentChoices()) {
            choices.push_back({choice.nextNodeId, choice.text});
        }
        
        m_dialogWidget->setDialog(
            m_dialogSystem->getCurrentSpeaker(),
            m_dialogSystem->getCurrentText(),
            choices
        );
        
        int selectedChoice = m_dialogWidget->render(m_windowWidth, m_windowHeight);
        if (selectedChoice >= 0) {
            m_dialogSystem->selectChoice(selectedChoice);
        }
    } else if (m_dialogWidget) {
        m_dialogWidget->clear();
    }
    
    // Render Quest Log if open
    if (m_showQuestLog && m_questSystem) {
        m_renderer->renderQuestLog(m_questSystem.get(), m_windowWidth, m_windowHeight);
    }
    
    // Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
    
    // Swap buffers
    SDL_GL_SwapWindow(m_window);
}

void RuntimeApp::changeScene(const std::string& sceneId) {
    if (!m_world || !m_world->getWorld()) return;
    
    // First check active level
    auto* activeLevel = m_world->getActiveLevel();
    if (activeLevel) {
        for (const auto& scene : activeLevel->getScenes()) {
            if (scene->getName() == sceneId || scene->getId() == sceneId) {
                m_activeScene = scene.get();
                m_world->setActiveScene(m_activeScene);
                
                // Get background from DataLoader
                auto& dataLoader = DataLoader::instance();
                for (const auto& room : dataLoader.getRooms()) {
                    if (room.id == sceneId) {
                        m_activeSceneBackground = room.background;
                        break;
                    }
                }
                
                LOG_INFO("[RuntimeApp] Changed to scene: " + sceneId + " (same level)");
                return;
            }
        }
    }
    
    // Fallback: search all levels
    for (const auto& level : m_world->getWorld()->getLevels()) {
        for (const auto& scene : level->getScenes()) {
            if (scene->getName() == sceneId || scene->getId() == sceneId) {
                m_activeScene = scene.get();
                m_world->setActiveScene(m_activeScene);
                m_world->setActiveLevel(level.get());
                
                auto& dataLoader = DataLoader::instance();
                for (const auto& room : dataLoader.getRooms()) {
                    if (room.id == sceneId) {
                        m_activeSceneBackground = room.background;
                        break;
                    }
                }
                
                LOG_INFO("[RuntimeApp] Changed to scene: " + sceneId);
                return;
            }
        }
    }
    
    LOG_WARNING("[RuntimeApp] Scene not found: " + sceneId);
}

void RuntimeApp::changeLevel(const std::string& levelId, const std::string& sceneId) {
    if (!m_world) return;
    
    LOG_INFO("[RuntimeApp] Changing level: " + levelId + " -> scene: " + sceneId);
    
    // Get target level
    auto* targetLevel = m_world->getLevel(levelId);
    if (!targetLevel) {
        LOG_WARNING("[RuntimeApp] Level not found: " + levelId);
        return;
    }
    
    // Call lifecycle hooks on old level
    auto* oldLevel = m_world->getActiveLevel();
    if (oldLevel) {
        oldLevel->onLevelExit();
        LOG_INFO("[RuntimeApp] Exited level: " + oldLevel->getId());
    }
    
    // Switch to new level
    m_world->setActiveLevel(targetLevel);
    targetLevel->onLevelEnter();
    LOG_INFO("[RuntimeApp] Entered level: " + levelId);
    
    // Find and switch to target scene
    for (const auto& scene : targetLevel->getScenes()) {
        if (scene->getName() == sceneId || scene->getId() == sceneId) {
            m_activeScene = scene.get();
            m_world->setActiveScene(m_activeScene);
            
            // Get background
            auto& dataLoader = DataLoader::instance();
            for (const auto& room : dataLoader.getRooms()) {
                if (room.id == sceneId) {
                    m_activeSceneBackground = room.background;
                    break;
                }
            }
            
            LOG_INFO("[RuntimeApp] Changed to scene: " + sceneId + " in level: " + levelId);
            return;
        }
    }
    
    // Fallback to first scene in level
    if (!targetLevel->getScenes().empty()) {
        auto& firstScene = targetLevel->getScenes()[0];
        m_activeScene = firstScene.get();
        m_world->setActiveScene(m_activeScene);
        LOG_INFO("[RuntimeApp] Scene not found, using first scene: " + firstScene->getName());
    }
}

void RuntimeApp::handleInteraction() {
    if (!m_player || !m_activeScene) return;
    
    LOG_INFO("[RuntimeApp] E pressed - checking interaction...");
    
    // If in dialog, advance or close
    if (m_dialogSystem && m_dialogSystem->isActive()) {
        LOG_INFO("[RuntimeApp] Dialog active, advancing...");
        if (m_dialogSystem->getCurrentChoices().empty()) {
            m_dialogSystem->advance();
        }
        return;
    }
    
    glm::vec3 playerPos = m_player->getPosition3D();
    LOG_INFO("[RuntimeApp] Player pos: (" + std::to_string(playerPos.x) + ", " + 
             std::to_string(playerPos.y) + ", " + std::to_string(playerPos.z) + ")");
    
    // Check for nearby NPCs
    int npcCount = 0;
    for (const auto& actor : m_activeScene->getActors()) {
        if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
            npcCount++;
            glm::vec3 npcPos = npc->getPosition3D();
            float distance = glm::length(playerPos - npcPos);
            
            LOG_INFO("[RuntimeApp] NPC '" + npc->getName() + "' at (" + 
                     std::to_string(npcPos.x) + ", " + std::to_string(npcPos.y) + ", " + 
                     std::to_string(npcPos.z) + ") distance: " + std::to_string(distance) +
                     " dialogId: " + npc->getDialogId());
            
            // Within interaction range (3 units - increased)
            if (distance < 3.0f) {
                std::string dialogId = npc->getDialogId();
                if (!dialogId.empty() && m_dialogSystem) {
                    LOG_INFO("[RuntimeApp] Starting dialog: " + dialogId);
                    m_dialogSystem->startDialog(dialogId);
                    
                    // Track quest progress
                    if (m_questSystem) {
                        m_questSystem->onTalk(npc->getName());
                    }
                } else {
                    LOG_INFO("[RuntimeApp] No dialogId or dialogSystem for " + npc->getName());
                }
                return;
            }
        }
    }
    
    LOG_INFO("[RuntimeApp] Found " + std::to_string(npcCount) + " NPCs, none in range");
    
    // Check for nearby exits/doors
    if (m_world && m_activeScene) {
        std::string currentSceneId = m_activeScene->getName();
        // Convert to lowercase for matching
        std::transform(currentSceneId.begin(), currentSceneId.end(), currentSceneId.begin(), ::tolower);
        
        auto exits = m_world->getExitsForScene(currentSceneId);
        for (const auto& exit : exits) {
            float distance = glm::length(playerPos - exit.position);
            LOG_INFO("[RuntimeApp] Exit '" + exit.name + "' at (" + 
                     std::to_string(exit.position.x) + ", " + std::to_string(exit.position.y) + ", " + 
                     std::to_string(exit.position.z) + ") distance: " + std::to_string(distance) +
                     " -> " + (exit.isLevelTransition() ? exit.targetLevel + "/" : "") + exit.targetScene);
            
            if (distance < exit.interactRadius) {
                if (exit.isLevelTransition()) {
                    LOG_INFO("[RuntimeApp] Level transition: " + exit.name + " -> " + exit.targetLevel + "/" + exit.targetScene);
                    changeLevel(exit.targetLevel, exit.targetScene);
                } else {
                    LOG_INFO("[RuntimeApp] Scene transition: " + exit.name + " -> " + exit.targetScene);
                    changeScene(exit.targetScene);
                }
                return;
            }
        }
        LOG_INFO("[RuntimeApp] No exits in range");
    }
}

std::string RuntimeApp::getSceneBackground() const {
    return m_activeSceneBackground;
}
