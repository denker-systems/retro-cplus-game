/**
 * @file VideoSettings.cpp
 * @brief Implementation av videoinställningar
 */
#include "VideoSettings.h"
#include <iostream>

VideoSettings& VideoSettings::instance() {
    static VideoSettings instance;
    return instance;
}

void VideoSettings::detectOptimalSettings() {
    // Hämta skärminfo
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(m_displayIndex, &displayMode) == 0) {
        m_nativeWidth = displayMode.w;
        m_nativeHeight = displayMode.h;
        
        std::cout << "Detected display: " << m_nativeWidth << "x" << m_nativeHeight << std::endl;
        
        // Välj optimal upplösning baserat på skärmstorlek
        if (m_resolution == Resolution::Auto) {
            if (m_nativeWidth >= 3840) {
                m_resolution = Resolution::UHD;
            } else if (m_nativeWidth >= 2560) {
                m_resolution = Resolution::QHD;
            } else if (m_nativeWidth >= 1920) {
                m_resolution = Resolution::FullHD;
            } else {
                m_resolution = Resolution::HD;
            }
            std::cout << "Auto-selected resolution: " << getResolutionName(m_resolution) << std::endl;
        }
    }
    
    calculateDimensions();
}

void VideoSettings::setResolution(Resolution res) {
    m_resolution = res;
    calculateDimensions();
}

void VideoSettings::setWindowMode(WindowMode mode) {
    m_windowMode = mode;
}

void VideoSettings::calculateDimensions() {
    // 16:10 aspekt-ratio för retro-stil
    switch (m_resolution) {
        case Resolution::HD:
            m_width = 1280;
            m_height = 800;
            break;
        case Resolution::FullHD:
            m_width = 1920;
            m_height = 1200;
            break;
        case Resolution::QHD:
            m_width = 2560;
            m_height = 1600;
            break;
        case Resolution::UHD:
            m_width = 3840;
            m_height = 2400;
            break;
        case Resolution::Auto:
        default:
            // Använd native eller FullHD som fallback
            if (m_nativeWidth > 0) {
                m_width = m_nativeWidth;
                m_height = m_nativeHeight;
            } else {
                m_width = 1920;
                m_height = 1200;
            }
            break;
    }
}

void VideoSettings::apply(SDL_Window* window, SDL_Renderer* renderer) {
    Uint32 flags = 0;
    
    switch (m_windowMode) {
        case WindowMode::Fullscreen:
            flags = SDL_WINDOW_FULLSCREEN_DESKTOP;
            break;
        case WindowMode::Borderless:
            flags = SDL_WINDOW_BORDERLESS;
            SDL_SetWindowSize(window, m_width, m_height);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            break;
        case WindowMode::Windowed:
            flags = 0;
            SDL_SetWindowSize(window, m_width, m_height);
            SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
            break;
    }
    
    SDL_SetWindowFullscreen(window, flags);
    
    // Uppdatera VSync
    if (m_vsync) {
        SDL_RenderSetVSync(renderer, 1);
    } else {
        SDL_RenderSetVSync(renderer, 0);
    }
    
    std::cout << "Applied video settings: " << getResolutionName(m_resolution) 
              << " (" << m_width << "x" << m_height << ")" << std::endl;
}

std::string VideoSettings::getResolutionName(Resolution res) {
    switch (res) {
        case Resolution::Auto:   return "Auto";
        case Resolution::HD:     return "HD (1280x800)";
        case Resolution::FullHD: return "Full HD (1920x1200)";
        case Resolution::QHD:    return "2K (2560x1600)";
        case Resolution::UHD:    return "4K (3840x2400)";
        default:                 return "Unknown";
    }
}

Resolution VideoSettings::nextResolution() const {
    switch (m_resolution) {
        case Resolution::Auto:   return Resolution::HD;
        case Resolution::HD:     return Resolution::FullHD;
        case Resolution::FullHD: return Resolution::QHD;
        case Resolution::QHD:    return Resolution::UHD;
        case Resolution::UHD:    return Resolution::Auto;
        default:                 return Resolution::Auto;
    }
}

Resolution VideoSettings::prevResolution() const {
    switch (m_resolution) {
        case Resolution::Auto:   return Resolution::UHD;
        case Resolution::HD:     return Resolution::Auto;
        case Resolution::FullHD: return Resolution::HD;
        case Resolution::QHD:    return Resolution::FullHD;
        case Resolution::UHD:    return Resolution::QHD;
        default:                 return Resolution::Auto;
    }
}
