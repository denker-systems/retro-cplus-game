/**
 * @file ProjectService.cpp
 * @brief Project service implementation
 */
#include "ProjectService.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace launcher {

ProjectService::ProjectService() {
    const char* home = std::getenv("USERPROFILE");
    if (home) {
        m_projectsFile = std::string(home) + "\\Documents\\RetroEngine\\projects.json";
    } else {
        m_projectsFile = "projects.json";
    }
}

void ProjectService::loadProjects() {
    try {
        if (!fs::exists(m_projectsFile)) {
            std::cout << "No projects file found, starting fresh" << std::endl;
            return;
        }
        
        std::ifstream file(m_projectsFile);
        json data = json::parse(file);
        
        m_projects.clear();
        for (const auto& projPath : data["projects"]) {
            std::string projectFile = ProjectManager::findProjectFile(projPath);
            if (!projectFile.empty()) {
                try {
                    RetroProject proj = RetroProject::loadFromFile(projectFile);
                    m_projects.push_back(proj);
                } catch (const std::exception& e) {
                    std::cerr << "Failed to load project at " << projPath << ": " << e.what() << std::endl;
                }
            }
        }
        
        std::cout << "Loaded " << m_projects.size() << " projects" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error loading projects: " << e.what() << std::endl;
    }
}

void ProjectService::saveProjects() {
    try {
        fs::path projectsPath(m_projectsFile);
        fs::create_directories(projectsPath.parent_path());
        
        json data;
        data["projects"] = json::array();
        
        for (const auto& proj : m_projects) {
            data["projects"].push_back(proj.path);
        }
        
        std::ofstream file(m_projectsFile);
        file << data.dump(2);
        
        std::cout << "Saved " << m_projects.size() << " projects" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error saving projects: " << e.what() << std::endl;
    }
}

void ProjectService::addProject(const RetroProject& project) {
    m_projects.push_back(project);
    saveProjects();
}

void ProjectService::removeProject(const std::string& projectPath) {
    m_projects.erase(
        std::remove_if(m_projects.begin(), m_projects.end(),
            [&projectPath](const RetroProject& p) { return p.path == projectPath; }),
        m_projects.end()
    );
    saveProjects();
}

} // namespace launcher
