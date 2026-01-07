/**
 * @file VideoSettings.h
 * @brief Videoinställningar - upplösning, fullscreen, etc.
 */
#pragma once

#include <SDL.h>
#include <string>

/**
 * @brief Upplösningslägen
 */
enum class Resolution {
    Auto,       // Automatiskt baserat på skärm
    HD,         // 1280x800 (16:10)
    FullHD,     // 1920x1200 (16:10)
    QHD,        // 2560x1600 (16:10)
    UHD         // 3840x2400 (16:10)
};

/**
 * @brief Fönsterläge
 */
enum class WindowMode {
    Windowed,
    Borderless,
    Fullscreen
};

/**
 * @brief Videoinställningar singleton
 */
class VideoSettings {
public:
    static VideoSettings& instance();
    
    /** @brief Detektera optimal upplösning baserat på skärm */
    void detectOptimalSettings();
    
    /** @brief Applicera inställningar på fönster */
    void apply(SDL_Window* window, SDL_Renderer* renderer);
    
    // Setters
    void setResolution(Resolution res);
    void setWindowMode(WindowMode mode);
    void setVSync(bool enabled) { m_vsync = enabled; }
    
    // Getters
    Resolution getResolution() const { return m_resolution; }
    WindowMode getWindowMode() const { return m_windowMode; }
    bool getVSync() const { return m_vsync; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    /** @brief Hämta upplösningsnamn */
    static std::string getResolutionName(Resolution res);
    
    /** @brief Nästa upplösning i listan */
    Resolution nextResolution() const;
    Resolution prevResolution() const;

private:
    VideoSettings() = default;
    
    void calculateDimensions();
    
    Resolution m_resolution = Resolution::Auto;
    WindowMode m_windowMode = WindowMode::Fullscreen;
    bool m_vsync = true;
    
    int m_width = 1920;
    int m_height = 1200;
    int m_displayIndex = 0;
    
    // Detekterad skärminfo
    int m_nativeWidth = 0;
    int m_nativeHeight = 0;
};
