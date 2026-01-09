/**
 * @file LauncherApp.cpp
 * @brief Launcher application implementation (Refactored)
 */
#include "LauncherApp.h"
#include "project/ProjectManager.h"
#include "editor/ui/EditorTheme.h"
#include <SDL_image.h>
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <ctime>
#include <string>

#ifdef HAS_IMGUI
#include "imgui/imgui_impl_sdl2.h"
#include "imgui/imgui_impl_sdlrenderer2.h"
#include <imgui.h>
#endif

namespace launcher {

LauncherApp::LauncherApp()
    : m_config(LauncherConfig::instance()) {
}

LauncherApp::~LauncherApp() {
    shutdown();
}

bool LauncherApp::initialize() {
    std::cout << "[LauncherApp] ========================================" << std::endl;
    std::cout << "[LauncherApp] Initializing Retro Engine Launcher..." << std::endl;
    std::cout << "[LauncherApp] ========================================" << std::endl;
    
    // Initialize SDL
    std::cout << "[LauncherApp] Initializing SDL..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "[LauncherApp] ERROR: SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "[LauncherApp] ✓ SDL initialized" << std::endl;
    
    std::cout << "[LauncherApp] Initializing SDL_image..." << std::endl;
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "[LauncherApp] WARNING: IMG_Init failed: " << IMG_GetError() << std::endl;
    } else {
        std::cout << "[LauncherApp] ✓ SDL_image initialized" << std::endl;
    }
    
    // Create window
    std::cout << "[LauncherApp] Creating window: " << m_config.getWindowWidth() << "x" << m_config.getWindowHeight() << std::endl;
    m_window = SDL_CreateWindow(
        m_config.getWindowTitle().c_str(),
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_config.getWindowWidth(),
        m_config.getWindowHeight(),
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    
    if (!m_window) {
        std::cerr << "[LauncherApp] ERROR: SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "[LauncherApp] ✓ Window created" << std::endl;
    
    std::cout << "[LauncherApp] Creating renderer..." << std::endl;
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        std::cerr << "[LauncherApp] ERROR: SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        return false;
    }
    std::cout << "[LauncherApp] ✓ Renderer created" << std::endl;
    
#ifdef HAS_IMGUI
    std::cout << "[LauncherApp] Initializing ImGui..." << std::endl;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    std::cout << "[LauncherApp] ✓ ImGui context created" << std::endl;
    
    std::cout << "[LauncherApp] Loading fonts and applying theme..." << std::endl;
    editor::EditorTheme::LoadFonts();
    editor::EditorTheme::Apply(editor::EditorTheme::Theme::RetroEngine);
    std::cout << "[LauncherApp] ✓ Theme applied" << std::endl;
    
    std::cout << "[LauncherApp] Initializing ImGui backends..." << std::endl;
    ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer2_Init(m_renderer);
    std::cout << "[LauncherApp] ✓ ImGui backends initialized" << std::endl;
#endif
    
    // Initialize services
    std::cout << "[LauncherApp] Initializing ProjectService..." << std::endl;
    m_projectService = std::make_unique<ProjectService>();
    std::cout << "[LauncherApp] Loading projects..." << std::endl;
    m_projectService->loadProjects();
    std::cout << "[LauncherApp] ✓ ProjectService initialized with " << m_projectService->getProjects().size() << " projects" << std::endl;
    
    // Initialize UI components
    std::cout << "[LauncherApp] Initializing UI components..." << std::endl;
    m_header = std::make_unique<LauncherHeader>();
    m_header->initialize(m_renderer);
    std::cout << "[LauncherApp] ✓ Header initialized" << std::endl;
    
    m_projectGrid = std::make_unique<ProjectGrid>();
    m_projectGrid->initialize(m_renderer);
    m_projectGrid->setOnOpenEditor([this](const RetroProject& p) { onOpenEditor(p); });
    m_projectGrid->setOnLaunchGame([this](const RetroProject& p) { onLaunchGame(p); });
    m_projectGrid->setOnDeleteProject([this](const RetroProject& p) { onDeleteProject(p); });
    std::cout << "[LauncherApp] ✓ ProjectGrid initialized" << std::endl;
    
    m_newProjectDialog = std::make_unique<NewProjectDialog>();
    m_newProjectDialog->setOnCreate([this](const std::string& n, const std::string& p, const std::string& t) {
        onCreateProject(n, p, t);
    });
    std::cout << "[LauncherApp] ✓ NewProjectDialog initialized" << std::endl;
    
    m_running = true;
    std::cout << "[LauncherApp] ========================================" << std::endl;
    std::cout << "[LauncherApp] ✓ Launcher initialized successfully!" << std::endl;
    std::cout << "[LauncherApp] ========================================" << std::endl;
    return true;
}

void LauncherApp::run() {
    Uint32 lastTime = SDL_GetTicks();
    
    while (m_running) {
        Uint32 currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        handleEvents();
        update(deltaTime);
        render();
    }
}

void LauncherApp::shutdown() {
#ifdef HAS_IMGUI
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
#endif
    
    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
    
    IMG_Quit();
    SDL_Quit();
}

void LauncherApp::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
#ifdef HAS_IMGUI
        ImGui_ImplSDL2_ProcessEvent(&event);
#endif
        
        if (event.type == SDL_QUIT) {
            m_running = false;
        }
        
        if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
            m_running = false;
        }
    }
}

void LauncherApp::update(float deltaTime) {
    // Update logic
}

void LauncherApp::render() {
#ifdef HAS_IMGUI
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
    
    // Render UI components
    ImGuiIO& io = ImGui::GetIO();
    
    int headerAction = m_header->render(io.DisplaySize.x);
    if (headerAction == 1) {
        onNewProject();
    } else if (headerAction == 2) {
        // Show about dialog (TODO)
    }
    
    m_projectGrid->render(m_projectService->getProjects(), io.DisplaySize.x, io.DisplaySize.y, m_header->getHeight());
    m_newProjectDialog->render();
    
    // Render
    ImGui::Render();
    SDL_SetRenderDrawColor(m_renderer, 13, 13, 15, 255);
    SDL_RenderClear(m_renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), m_renderer);
    SDL_RenderPresent(m_renderer);
#endif
}

void LauncherApp::onNewProject() {
    std::cout << "[LauncherApp] onNewProject() called" << std::endl;
    std::string defaultPath = m_config.getDefaultProjectsPath();
    std::cout << "[LauncherApp] Default project path: " << defaultPath << std::endl;
    m_newProjectDialog->setDefaultPath(defaultPath);
    m_newProjectDialog->show();
    std::cout << "[LauncherApp] New project dialog shown" << std::endl;
}

void LauncherApp::onOpenEditor(const RetroProject& project) {
    std::cout << "[LauncherApp] ========================================" << std::endl;
    std::cout << "[LauncherApp] onOpenEditor() called" << std::endl;
    std::cout << "[LauncherApp] Project: " << project.name << std::endl;
    std::cout << "[LauncherApp] Path: " << project.path << std::endl;
    
    // Write current project info to temp file for editor to read
    try {
        std::ofstream projectFile(".current_project");
        if (projectFile.is_open()) {
            projectFile << project.path << std::endl;
            projectFile << project.name << std::endl;
            projectFile.close();
            std::cout << "[LauncherApp] ✓ Wrote project info to .current_project" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "[LauncherApp] Failed to write project info: " << e.what() << std::endl;
    }
    
    // Launch editor (runs from its own directory)
    std::string command = "start RetroEditor.exe";
    
    std::cout << "[LauncherApp] Executing: " << command << std::endl;
    std::cout << "[LauncherApp] ========================================" << std::endl;
    
    system(command.c_str());
}

void LauncherApp::onLaunchGame(const RetroProject& project) {
    std::cout << "[LauncherApp] onLaunchGame() called for: " << project.name << std::endl;
    std::cout << "[LauncherApp] Project path: " << project.path << std::endl;
    std::string command = "start RetroGame.exe \"" + project.path + "\"";
    std::cout << "[LauncherApp] Executing command: " << command << std::endl;
    system(command.c_str());
}

void LauncherApp::onDeleteProject(const RetroProject& project) {
    std::cout << "[LauncherApp] ========================================" << std::endl;
    std::cout << "[LauncherApp] onDeleteProject() called" << std::endl;
    std::cout << "[LauncherApp] Project: " << project.name << std::endl;
    std::cout << "[LauncherApp] Path: " << project.path << std::endl;
    
    try {
        // Remove from project list
        m_projectService->removeProject(project.path);
        std::cout << "[LauncherApp] ✓ Project removed from list" << std::endl;
        
        // Delete project files from disk
        if (std::filesystem::exists(project.path)) {
            std::cout << "[LauncherApp] Deleting project directory: " << project.path << std::endl;
            std::uintmax_t removed = std::filesystem::remove_all(project.path);
            std::cout << "[LauncherApp] ✓ Deleted " << removed << " files/directories" << std::endl;
        } else {
            std::cout << "[LauncherApp] WARNING: Project directory not found: " << project.path << std::endl;
        }
        
        std::cout << "[LauncherApp] ✓ Project deleted successfully" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[LauncherApp] ERROR: Failed to delete project: " << e.what() << std::endl;
    }
    
    std::cout << "[LauncherApp] ========================================" << std::endl;
}

void LauncherApp::onCreateProject(const std::string& name, const std::string& path, const std::string& templateName) {
    std::cout << "[LauncherApp] onCreateProject() called" << std::endl;
    std::cout << "[LauncherApp] Name: " << name << std::endl;
    std::cout << "[LauncherApp] Path: " << path << std::endl;
    std::cout << "[LauncherApp] Template: " << templateName << std::endl;
    
    try {
        RetroProject project = ProjectManager::createProject(name, path, templateName);
        std::cout << "[LauncherApp] ✓ Project created at: " << project.path << std::endl;
        m_projectService->addProject(project);
        std::cout << "[LauncherApp] ✓ Project added to list" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[LauncherApp] ERROR: Failed to create project: " << e.what() << std::endl;
    }
}

} // namespace launcher
