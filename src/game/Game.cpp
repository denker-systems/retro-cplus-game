/**
 * @file Game.cpp
 * @brief Implementation av huvudspelloop och SDL-initiering
 */
#include "Game.h"
#include "VideoSettings.h"
#include "states/IState.h"
#include "graphics/TextureManager.h"
#include "graphics/FontManager.h"
#include "audio/AudioManager.h"
#include "utils/Logger.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>

Game::Game() = default;

Game::~Game() {
    quit();
}

bool Game::init(const std::string& title, int width, int height) {
    // Initiera Logger först
    Logger::instance().init("assets/logs/game.log");
    LOG_INFO("=== Game Starting ===");
    
    // Initiera SDL (med gamecontroller-stöd)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        LOG_ERROR(std::string("SDL init failed: ") + SDL_GetError());
        return false;
    }

    // Initiera SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        LOG_ERROR(std::string("SDL_image init failed: ") + IMG_GetError());
        return false;
    }
    LOG_INFO("SDL_image initialized");

    // Initiera SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        LOG_ERROR(std::string("SDL_mixer init failed: ") + Mix_GetError());
        return false;
    }
    LOG_INFO("SDL_mixer initialized");
    
    // Detektera optimal upplösning
    VideoSettings::instance().detectOptimalSettings();

    // Skapa fönster - windowed mode för både Game och Editor
    bool isEditor = (title.find("Editor") != std::string::npos);
    Uint32 windowFlags = SDL_WINDOW_RESIZABLE;
    
    // Editor needs OpenGL for 3D viewport
    // Uses GLTextureManager for proper OpenGL texture loading
    if (isEditor) {
        windowFlags |= SDL_WINDOW_OPENGL;
        m_useOpenGL = true;
        
        // Set OpenGL attributes BEFORE window creation
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    }
    
    // Game: 3x native (1920x1200) eller max skärmstorlek
    // Editor: max 1600x900
    int windowWidth, windowHeight;
    
    if (isEditor) {
        windowWidth = std::min(1600, VideoSettings::instance().getWidth());
        windowHeight = std::min(900, VideoSettings::instance().getHeight());
    } else {
        // Game: 3x native resolution (640x400 * 3 = 1920x1200)
        windowWidth = std::min(GAME_WIDTH * 3, VideoSettings::instance().getWidth() - 100);
        windowHeight = std::min(GAME_HEIGHT * 3, VideoSettings::instance().getHeight() - 100);
    }
    
    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        windowFlags
    );

    if (!m_window) {
        LOG_ERROR(std::string("Window creation failed: ") + SDL_GetError());
        return false;
    }
    LOG_INFO("Window created");

    // Skapa renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!m_renderer) {
        LOG_ERROR(std::string("Renderer creation failed: ") + SDL_GetError());
        return false;
    }
    LOG_INFO("Renderer created");
    
    // Beräkna viewport och skala
    calculateViewport();
    
    // Endast för spelet: Använd logisk upplösning för automatisk skalning
    // Editorn behöver faktisk pixel-upplösning för ImGui
    if (!isEditor) {
        SDL_RenderSetLogicalSize(m_renderer, GAME_WIDTH, GAME_HEIGHT);
    }
    
    // Aktivera linjär skalning för mjukare grafik
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    // Initiera managers
    TextureManager::instance().init(m_renderer);
    AudioManager::instance().init();
    FontManager::instance().init();
    
    // Sätt skala för DPI-korrekt font-rendering
    FontManager::instance().setScale(m_scale);
    
    // Ladda fonter - FontManager skalar upp internt för skarp text
    FontManager::instance().loadFont("default", "assets/fonts/arial.ttf", 12);
    FontManager::instance().loadFont("title", "assets/fonts/arial.ttf", 24);
    
    LOG_INFO("Scale: " + std::to_string(m_scale) + " (fonts scaled for sharp rendering)");

    // Note: Editor will call changeState() to set EditorState
    // No default state needed - editor-driven

    m_running = true;
    m_lastFrameTime = SDL_GetTicks();

    LOG_INFO("Game initialized successfully!");
    return true;
}

void Game::run() {
    while (m_running && m_currentState) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
        m_lastFrameTime = currentTime;

        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_running = false;
        }
        
        // Global mute toggle (M key)
        if (event.type == SDL_KEYDOWN && event.key.keysym.scancode == SDL_SCANCODE_M) {
            AudioManager::instance().toggleMute();
        }
        
        if (m_currentState) {
            m_currentState->handleEvent(event);
        }
    }
}

void Game::update(float deltaTime) {
    if (m_currentState) {
        m_currentState->update(deltaTime);
    }
}

void Game::render() {
    // When using OpenGL, ImGuiManager handles clear and swap
    if (m_useOpenGL) {
        if (m_currentState) {
            m_currentState->render(m_renderer);
        }
        return;
    }
    
    // SDL Renderer path
    SDL_SetRenderDrawColor(m_renderer, 0, 0, 0, 255);
    SDL_RenderClear(m_renderer);
    if (m_currentState) {
        m_currentState->render(m_renderer);
    }
    SDL_RenderPresent(m_renderer);
}

void Game::pushState(std::unique_ptr<IState> state) {
    // Simplified: Just set current state (no stack)
    state->setGame(this);
    if (m_currentState) {
        m_currentState->exit();
    }
    m_currentState = std::move(state);
    m_currentState->enter();
}

void Game::popState() {
    // Simplified: Clear current state
    if (m_currentState) {
        m_currentState->exit();
        m_currentState.reset();
    }
}

void Game::changeState(std::unique_ptr<IState> state) {
    state->setGame(this);
    if (m_currentState) {
        m_currentState->exit();
    }
    m_currentState = std::move(state);
    m_currentState->enter();
}

void Game::calculateViewport() {
    int windowW, windowH;
    SDL_GetWindowSize(m_window, &windowW, &windowH);
    
    float windowAspect = static_cast<float>(windowW) / windowH;
    float gameAspect = static_cast<float>(GAME_WIDTH) / GAME_HEIGHT;
    
    if (windowAspect > gameAspect) {
        // Skärmen är bredare - letterbox på sidorna
        m_viewport.h = windowH;
        m_viewport.w = static_cast<int>(windowH * gameAspect);
        m_viewport.x = (windowW - m_viewport.w) / 2;
        m_viewport.y = 0;
    } else {
        // Skärmen är högre - letterbox top/bottom
        m_viewport.w = windowW;
        m_viewport.h = static_cast<int>(windowW / gameAspect);
        m_viewport.x = 0;
        m_viewport.y = (windowH - m_viewport.h) / 2;
    }
    
    m_scale = static_cast<float>(m_viewport.w) / GAME_WIDTH;
    LOG_INFO("Viewport: " + std::to_string(m_viewport.w) + "x" + std::to_string(m_viewport.h) 
             + " at (" + std::to_string(m_viewport.x) + "," + std::to_string(m_viewport.y) + ")");
}

void Game::screenToGame(int screenX, int screenY, int& gameX, int& gameY) const {
    gameX = static_cast<int>((screenX - m_viewport.x) / m_scale);
    gameY = static_cast<int>((screenY - m_viewport.y) / m_scale);
}

int Game::getScreenWidth() const {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    return w;
}

int Game::getScreenHeight() const {
    int w, h;
    SDL_GetWindowSize(m_window, &w, &h);
    return h;
}

void Game::quit() {
    LOG_INFO("=== Game Shutting Down ===");
    if (m_currentState) {
        m_currentState->exit();
        m_currentState.reset();
    }
    AudioManager::instance().shutdown();
    FontManager::instance().shutdown();
    TextureManager::instance().shutdown();

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }

    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }

    Mix_Quit();
    IMG_Quit();
    SDL_Quit();
    
    Logger::instance().shutdown();
}
