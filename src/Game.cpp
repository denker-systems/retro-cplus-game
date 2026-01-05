/**
 * @file Game.cpp
 * @brief Implementation av huvudspelloop och SDL-initiering
 */
#include "Game.h"
#include "states/StateManager.h"
#include "states/IState.h"
#include "states/MenuState.h"
#include "graphics/TextureManager.h"
#include "graphics/FontManager.h"
#include "audio/AudioManager.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <iostream>

Game::Game() = default;

Game::~Game() {
    quit();
}

bool Game::init(const std::string& title, int width, int height) {
    // Initiera SDL (med gamecontroller-stöd)
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL init failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Initiera SDL_image
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image init failed: " << IMG_GetError() << std::endl;
        return false;
    }

    // Initiera SDL_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer init failed: " << Mix_GetError() << std::endl;
        return false;
    }

    // Skapa fullscreen fönster
    m_window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        0, 0,  // Storlek ignoreras för fullscreen desktop
        SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if (!m_window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Skapa renderer
    m_renderer = SDL_CreateRenderer(m_window, -1, 
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!m_renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return false;
    }
    
    // Sätt logisk upplösning för skalning
    SDL_RenderSetLogicalSize(m_renderer, GAME_WIDTH, GAME_HEIGHT);
    
    // Beräkna viewport för letterboxing
    calculateViewport();

    // Initiera managers
    TextureManager::instance().init(m_renderer);
    AudioManager::instance().init();
    FontManager::instance().init();
    FontManager::instance().loadFont("default", "assets/fonts/arial.ttf", 18);
    FontManager::instance().loadFont("title", "assets/fonts/arial.ttf", 32);

    // Skapa StateManager och starta med MenuState
    m_stateManager = std::make_unique<StateManager>();
    auto menuState = std::make_unique<MenuState>();
    menuState->setGame(this);
    m_stateManager->pushState(std::move(menuState));

    m_running = true;
    m_lastFrameTime = SDL_GetTicks();

    std::cout << "Game initialized successfully!" << std::endl;
    return true;
}

void Game::run() {
    while (m_running && !m_stateManager->isEmpty()) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - m_lastFrameTime) / 1000.0f;
        m_lastFrameTime = currentTime;

        handleEvents();
        m_stateManager->processPendingChanges();  // Process deferred state changes
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
        m_stateManager->handleEvent(event);
    }
}

void Game::update(float deltaTime) {
    m_stateManager->update(deltaTime);
}

void Game::render() {
    m_stateManager->render(m_renderer);
    SDL_RenderPresent(m_renderer);
}

void Game::pushState(std::unique_ptr<IState> state) {
    state->setGame(this);
    m_stateManager->pushState(std::move(state));
}

void Game::popState() {
    m_stateManager->popState();
}

void Game::changeState(std::unique_ptr<IState> state) {
    state->setGame(this);
    m_stateManager->changeState(std::move(state));
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
    std::cout << "Viewport: " << m_viewport.w << "x" << m_viewport.h 
              << " at (" << m_viewport.x << "," << m_viewport.y << ")" << std::endl;
}

void Game::screenToGame(int screenX, int screenY, int& gameX, int& gameY) const {
    gameX = static_cast<int>((screenX - m_viewport.x) / m_scale);
    gameY = static_cast<int>((screenY - m_viewport.y) / m_scale);
}

void Game::quit() {
    m_stateManager.reset();
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
}
