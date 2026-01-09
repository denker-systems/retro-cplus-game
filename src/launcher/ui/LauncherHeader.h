/**
 * @file LauncherHeader.h
 * @brief Launcher header UI component
 */
#pragma once

#include <SDL.h>
#include <string>

namespace launcher {

/**
 * @brief Header component with logo and action buttons
 */
class LauncherHeader {
public:
    LauncherHeader();
    ~LauncherHeader();
    
    /**
     * @brief Initialize header with logo
     */
    bool initialize(SDL_Renderer* renderer);
    
    /**
     * @brief Render header
     * @param screenWidth Screen width
     * @return Action triggered (0=none, 1=new project, 2=about)
     */
    int render(float screenWidth);
    
    /**
     * @brief Get header height
     */
    float getHeight() const { return 150.0f; }
    
private:
    SDL_Texture* m_logoTexture = nullptr;
    SDL_Renderer* m_renderer = nullptr;
};

} // namespace launcher
