/**
 * @file IconLoader.h
 * @brief Utility for loading and setting application icons
 */
#pragma once

#include <SDL.h>
#include <string>

namespace engine {

/**
 * @class IconLoader
 * @brief Loads and sets application window icons
 */
class IconLoader {
public:
    /**
     * @brief Set window icon from PNG file
     * @param window SDL window to set icon for
     * @param iconPath Path to PNG icon file
     * @return true if icon was set successfully
     */
    static bool SetWindowIcon(SDL_Window* window, const std::string& iconPath);
    
    /**
     * @brief Set window icon with automatic size selection
     * @param window SDL window to set icon for
     * @return true if icon was set successfully
     * 
     * Tries to load icon in this order:
     * 1. assets/icons/retro_engine_256.png
     * 2. assets/icons/retro_engine_128.png
     * 3. assets/icons/retro_engine_64.png
     * 4. assets/icons/retro_engine_48.png
     */
    static bool SetWindowIconAuto(SDL_Window* window);
};

} // namespace engine
