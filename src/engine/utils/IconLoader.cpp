/**
 * @file IconLoader.cpp
 * @brief Icon loading implementation
 */
#include "IconLoader.h"
#include <SDL_image.h>
#include <iostream>

namespace engine {

bool IconLoader::SetWindowIcon(SDL_Window* window, const std::string& iconPath) {
    if (!window) {
        std::cerr << "IconLoader: Invalid window pointer" << std::endl;
        return false;
    }
    
    // Load icon surface
    SDL_Surface* icon = IMG_Load(iconPath.c_str());
    if (!icon) {
        std::cerr << "IconLoader: Failed to load icon: " << iconPath << std::endl;
        std::cerr << "IMG_Error: " << IMG_GetError() << std::endl;
        return false;
    }
    
    // Set window icon
    SDL_SetWindowIcon(window, icon);
    
    // Free surface
    SDL_FreeSurface(icon);
    
    std::cout << "IconLoader: Successfully set window icon from " << iconPath << std::endl;
    return true;
}

bool IconLoader::SetWindowIconAuto(SDL_Window* window) {
    // Try different icon sizes in order of preference
    const char* iconPaths[] = {
        "assets/icons/retro_engine_256.png",
        "assets/icons/retro_engine_128.png",
        "assets/icons/retro_engine_64.png",
        "assets/icons/retro_engine_48.png",
        nullptr
    };
    
    for (int i = 0; iconPaths[i] != nullptr; i++) {
        if (SetWindowIcon(window, iconPaths[i])) {
            return true;
        }
    }
    
    std::cerr << "IconLoader: Failed to load any icon file" << std::endl;
    return false;
}

} // namespace engine
