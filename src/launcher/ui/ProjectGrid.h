/**
 * @file ProjectGrid.h
 * @brief Project grid UI component
 */
#pragma once

#include <SDL.h>
#include "launcher/project/ProjectManager.h"
#include <vector>
#include <functional>

namespace launcher {

/**
 * @brief Grid view for displaying projects
 */
class ProjectGrid {
public:
    using ProjectActionCallback = std::function<void(const RetroProject&)>;
    
    ProjectGrid();
    ~ProjectGrid();
    
    /**
     * @brief Initialize with renderer for thumbnails
     */
    void initialize(SDL_Renderer* renderer);
    
    /**
     * @brief Render project grid
     * @param projects List of projects to display
     * @param screenWidth Screen width for layout
     * @param screenHeight Screen height for layout
     * @param headerHeight Height of header
     */
    void render(const std::vector<RetroProject>& projects, float screenWidth, float screenHeight, float headerHeight);
    
    /**
     * @brief Set callback for opening project in editor
     */
    void setOnOpenEditor(ProjectActionCallback callback) { m_onOpenEditor = callback; }
    
    /**
     * @brief Set callback for launching game
     */
    void setOnLaunchGame(ProjectActionCallback callback) { m_onLaunchGame = callback; }
    
    /**
     * @brief Set callback for deleting project
     */
    void setOnDeleteProject(ProjectActionCallback callback) { m_onDeleteProject = callback; }
    
private:
    void renderProjectCard(const RetroProject& project, int index);
    
    ProjectActionCallback m_onOpenEditor;
    ProjectActionCallback m_onLaunchGame;
    ProjectActionCallback m_onDeleteProject;
    SDL_Texture* m_defaultThumbnail = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};

} // namespace launcher
