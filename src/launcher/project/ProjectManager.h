/**
 * @file ProjectManager.h
 * @brief Retro Engine project management
 */
#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief Retro Engine project metadata
 */
struct RetroProject {
    std::string name;
    std::string path;
    std::string version;
    std::string engineVersion;
    std::string description;
    std::string author;
    std::string created;
    std::string lastModified;
    
    struct Settings {
        std::string windowTitle;
        int windowWidth = 1280;
        int windowHeight = 720;
        bool fullscreen = false;
        bool vsync = true;
        int targetFPS = 60;
    } settings;
    
    struct Paths {
        std::string assets = "assets";
        std::string data = "assets/data";
        std::string scenes = "assets/data/scenes.json";
        std::string startScene = "";
    } paths;
    
    struct Build {
        std::string outputName;
        std::string icon = "assets/icons/game_icon.ico";
        std::string version = "1.0.0";
    } build;
    
    // Serialize to JSON
    json toJson() const;
    
    // Deserialize from JSON
    static RetroProject fromJson(const json& j);
    
    // Save to .retro file
    bool saveToFile(const std::string& filepath) const;
    
    // Load from .retro file
    static RetroProject loadFromFile(const std::string& filepath);
};

/**
 * @brief Manages Retro Engine projects
 */
class ProjectManager {
public:
    /**
     * @brief Create a new project from template
     * @param name Project name
     * @param path Parent directory path
     * @param templateName Template to use
     * @return Created project info
     */
    static RetroProject createProject(
        const std::string& name,
        const std::string& path,
        const std::string& templateName = "Empty Project"
    );
    
    /**
     * @brief Open existing project
     * @param projectFile Path to .retro file
     * @return Project info
     */
    static RetroProject openProject(const std::string& projectFile);
    
    /**
     * @brief Check if path contains a valid Retro Engine project
     * @param path Directory path
     * @return true if valid project found
     */
    static bool isValidProject(const std::string& path);
    
    /**
     * @brief Find .retro file in directory
     * @param path Directory path
     * @return Path to .retro file, or empty string if not found
     */
    static std::string findProjectFile(const std::string& path);
    
    /**
     * @brief Get available project templates
     */
    static std::vector<std::string> getTemplates();
    
    /**
     * @brief Get default projects directory
     * @return Path to %USERPROFILE%\Documents\RetroEngine\Projects
     */
    static std::string getDefaultProjectsPath();
    
private:
    /**
     * @brief Create project folder structure
     */
    static void createFolderStructure(const std::string& projectPath);
    
    /**
     * @brief Copy template files to project
     */
    static void copyTemplate(const std::string& templateName, const std::string& projectPath);
    
    /**
     * @brief Create default .retro file
     */
    static RetroProject createDefaultProject(const std::string& name, const std::string& path);
};
