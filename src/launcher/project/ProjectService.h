/**
 * @file ProjectService.h
 * @brief Service for managing project list and persistence
 */
#pragma once

#include "ProjectManager.h"
#include <vector>
#include <string>

namespace launcher {

/**
 * @brief Service for managing project list
 */
class ProjectService {
public:
    ProjectService();
    
    /**
     * @brief Load projects from disk
     */
    void loadProjects();
    
    /**
     * @brief Save projects to disk
     */
    void saveProjects();
    
    /**
     * @brief Add project to list
     */
    void addProject(const RetroProject& project);
    
    /**
     * @brief Remove project from list
     */
    void removeProject(const std::string& projectPath);
    
    /**
     * @brief Get all projects
     */
    const std::vector<RetroProject>& getProjects() const { return m_projects; }
    
    /**
     * @brief Get projects file path
     */
    const std::string& getProjectsFilePath() const { return m_projectsFile; }
    
private:
    std::vector<RetroProject> m_projects;
    std::string m_projectsFile;
};

} // namespace launcher
