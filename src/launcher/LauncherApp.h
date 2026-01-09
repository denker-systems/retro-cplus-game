/**
 * @file LauncherApp.h
 * @brief Retro Engine Launcher Application (Refactored)
 */
#pragma once

#include <SDL.h>
#include <memory>
#include "project/ProjectService.h"
#include "ui/LauncherHeader.h"
#include "ui/ProjectGrid.h"
#include "ui/NewProjectDialog.h"
#include "core/LauncherConfig.h"

namespace launcher {

/**
 * @brief Main launcher application (orchestrator)
 */
class LauncherApp {
public:
    LauncherApp();
    ~LauncherApp();
    
    bool initialize();
    void run();
    void shutdown();
    
private:
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    // Event handlers
    void onNewProject();
    void onOpenEditor(const RetroProject& project);
    void onLaunchGame(const RetroProject& project);
    void onDeleteProject(const RetroProject& project);
    void onCreateProject(const std::string& name, const std::string& path, const std::string& templateName);
    
    // SDL
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    bool m_running = false;
    
    // Services
    std::unique_ptr<ProjectService> m_projectService;
    
    // UI Components
    std::unique_ptr<LauncherHeader> m_header;
    std::unique_ptr<ProjectGrid> m_projectGrid;
    std::unique_ptr<NewProjectDialog> m_newProjectDialog;
    
    // Config
    LauncherConfig& m_config;
};

} // namespace launcher
