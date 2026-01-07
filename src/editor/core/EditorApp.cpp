/**
 * @file EditorApp.cpp
 * @brief Implementation av standalone editor application
 */
#include "EditorApp.h"
#include "editor/core/EditorState.h"
#include "engine/graphics/FontManager.h"
#include "engine/graphics/TextureManager.h"
#include "engine/audio/AudioManager.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>

EditorApp::EditorApp() = default;

EditorApp::~EditorApp() {
    shutdown();
}

bool EditorApp::init(const std::string& title, int width, int height) {
    LOG_INFO("EditorApp::init()");
    
    // Init SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        LOG_ERROR("SDL_Init failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Init SDL_image
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        LOG_ERROR("IMG_Init failed: " + std::string(IMG_GetError()));
        return false;
    }
    
    // Init SDL_ttf
    if (TTF_Init() < 0) {
        LOG_ERROR("TTF_Init failed: " + std::string(TTF_GetError()));
        return false;
    }
    
    // Create fullscreen window
    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_FULLSCREEN_DESKTOP
    );
    
    if (!m_window) {
        LOG_ERROR("Window creation failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Create renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!m_renderer) {
        LOG_ERROR("Renderer creation failed: " + std::string(SDL_GetError()));
        return false;
    }
    
    // Använd logisk upplösning för enkel koordinat-hantering
    SDL_RenderSetLogicalSize(m_renderer, 640, 400);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
    
    // Init managers
    TextureManager::instance().init(m_renderer);
    FontManager::instance().init();
    
    // Försök ladda PressStart2P, fallback till arial
    if (!FontManager::instance().loadFont("default", "assets/fonts/PressStart2P.ttf", 8)) {
        LOG_INFO("PressStart2P not found, using arial.ttf");
        FontManager::instance().loadFont("default", "assets/fonts/arial.ttf", 12);
    }
    
    AudioManager::instance().init();
    
    // Load game data for editing
    DataLoader::instance().loadAll();
    
    // Create EditorState (innehåller alla befintliga editor-features)
    m_editorState = std::make_unique<EditorState>();
    
    // Sätt EditorApp som "game" för EditorState (för kompatibilitet)
    m_editorState->setGame(reinterpret_cast<Game*>(this));
    
    m_editorState->enter();
    
    m_running = true;
    m_lastTime = SDL_GetTicks();
    
    LOG_INFO("EditorApp initialized successfully");
    return true;
}

void EditorApp::run() {
    LOG_INFO("EditorApp::run() - Starting main loop");
    
    while (m_running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - m_lastTime) / 1000.0f;
        m_lastTime = currentTime;
        
        handleEvents();
        update(deltaTime);
        render();
    }
}

void EditorApp::handleEvents() {
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
        
        // Forward to EditorState
        if (m_editorState) {
            m_editorState->handleEvent(event);
        }
    }
}

void EditorApp::update(float deltaTime) {
    if (m_editorState) {
        m_editorState->update(deltaTime);
    }
}

void EditorApp::render() {
    SDL_SetRenderDrawColor(m_renderer, 30, 30, 40, 255);
    SDL_RenderClear(m_renderer);
    
    // Render EditorState (innehåller alla befintliga editor-features)
    if (m_editorState) {
        m_editorState->render(m_renderer);
    }
    
    SDL_RenderPresent(m_renderer);
}

void EditorApp::shutdown() {
    LOG_INFO("EditorApp::shutdown()");
    
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
    
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}
