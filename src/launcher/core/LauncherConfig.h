/**
 * @file LauncherConfig.h
 * @brief Launcher configuration and settings
 */
#pragma once

#include <string>

namespace launcher {

/**
 * @brief Launcher configuration
 */
class LauncherConfig {
public:
    static LauncherConfig& instance();
    
    // Window settings
    int getWindowWidth() const { return m_windowWidth; }
    int getWindowHeight() const { return m_windowHeight; }
    const std::string& getWindowTitle() const { return m_windowTitle; }
    
    // Paths
    std::string getProjectsFilePath() const;
    std::string getDefaultProjectsPath() const;
    
private:
    LauncherConfig();
    
    int m_windowWidth = 1200;
    int m_windowHeight = 700;
    std::string m_windowTitle = "Retro Engine Launcher";
};

} // namespace launcher
