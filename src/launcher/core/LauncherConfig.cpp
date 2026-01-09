/**
 * @file LauncherConfig.cpp
 * @brief Launcher configuration implementation
 */
#include "LauncherConfig.h"
#include "launcher/project/ProjectManager.h"
#include <cstdlib>

namespace launcher {

LauncherConfig& LauncherConfig::instance() {
    static LauncherConfig config;
    return config;
}

LauncherConfig::LauncherConfig() {
}

std::string LauncherConfig::getProjectsFilePath() const {
    const char* home = std::getenv("USERPROFILE");
    if (home) {
        return std::string(home) + "\\Documents\\RetroEngine\\projects.json";
    }
    return "projects.json";
}

std::string LauncherConfig::getDefaultProjectsPath() const {
    return ProjectManager::getDefaultProjectsPath();
}

} // namespace launcher
