/**
 * @file ProjectManager.cpp
 * @brief Project management implementation
 */
#include "ProjectManager.h"
#include <fstream>
#include <filesystem>
#include <iostream>
#include <ctime>

namespace fs = std::filesystem;

// ============================================================================
// RetroProject Serialization
// ============================================================================

json RetroProject::toJson() const {
    json j;
    
    j["project"]["name"] = name;
    j["project"]["version"] = version;
    j["project"]["engineVersion"] = engineVersion;
    j["project"]["description"] = description;
    j["project"]["author"] = author;
    j["project"]["created"] = created;
    j["project"]["lastModified"] = lastModified;
    
    j["settings"]["windowTitle"] = settings.windowTitle;
    j["settings"]["windowWidth"] = settings.windowWidth;
    j["settings"]["windowHeight"] = settings.windowHeight;
    j["settings"]["fullscreen"] = settings.fullscreen;
    j["settings"]["vsync"] = settings.vsync;
    j["settings"]["targetFPS"] = settings.targetFPS;
    
    j["paths"]["assets"] = paths.assets;
    j["paths"]["data"] = paths.data;
    j["paths"]["scenes"] = paths.scenes;
    j["paths"]["startScene"] = paths.startScene;
    
    j["build"]["outputName"] = build.outputName;
    j["build"]["icon"] = build.icon;
    j["build"]["version"] = build.version;
    
    return j;
}

RetroProject RetroProject::fromJson(const json& j) {
    RetroProject proj;
    
    proj.name = j["project"]["name"];
    proj.version = j["project"].value("version", "1.0.0");
    proj.engineVersion = j["project"].value("engineVersion", "1.0.0");
    proj.description = j["project"].value("description", "");
    proj.author = j["project"].value("author", "");
    proj.created = j["project"].value("created", "");
    proj.lastModified = j["project"].value("lastModified", "");
    
    proj.settings.windowTitle = j["settings"].value("windowTitle", proj.name);
    proj.settings.windowWidth = j["settings"].value("windowWidth", 1280);
    proj.settings.windowHeight = j["settings"].value("windowHeight", 720);
    proj.settings.fullscreen = j["settings"].value("fullscreen", false);
    proj.settings.vsync = j["settings"].value("vsync", true);
    proj.settings.targetFPS = j["settings"].value("targetFPS", 60);
    
    proj.paths.assets = j["paths"].value("assets", "assets");
    proj.paths.data = j["paths"].value("data", "assets/data");
    proj.paths.scenes = j["paths"].value("scenes", "assets/data/scenes.json");
    proj.paths.startScene = j["paths"].value("startScene", "");
    
    proj.build.outputName = j["build"].value("outputName", proj.name);
    proj.build.icon = j["build"].value("icon", "assets/icons/game_icon.ico");
    proj.build.version = j["build"].value("version", "1.0.0");
    
    return proj;
}

bool RetroProject::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }
        
        json j = toJson();
        file << j.dump(2);
        
        std::cout << "Saved project to: " << filepath << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving project: " << e.what() << std::endl;
        return false;
    }
}

RetroProject RetroProject::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + filepath);
        }
        
        json j = json::parse(file);
        RetroProject proj = RetroProject::fromJson(j);
        proj.path = fs::path(filepath).parent_path().string();
        
        std::cout << "Loaded project: " << proj.name << " from " << filepath << std::endl;
        return proj;
    } catch (const std::exception& e) {
        std::cerr << "Error loading project: " << e.what() << std::endl;
        throw;
    }
}

// ============================================================================
// ProjectManager
// ============================================================================

RetroProject ProjectManager::createProject(
    const std::string& name,
    const std::string& parentPath,
    const std::string& templateName
) {
    std::cout << "Creating project: " << name << " at " << parentPath << std::endl;
    
    // Create project directory
    fs::path projectPath = fs::path(parentPath) / name;
    
    if (fs::exists(projectPath)) {
        throw std::runtime_error("Project directory already exists: " + projectPath.string());
    }
    
    // Create folder structure
    createFolderStructure(projectPath.string());
    
    // Create default project
    RetroProject project = createDefaultProject(name, projectPath.string());
    
    // Copy template files
    copyTemplate(templateName, projectPath.string());
    
    // Save .retro file
    std::string projectFile = (projectPath / (name + ".retro")).string();
    project.saveToFile(projectFile);
    
    std::cout << "Project created successfully at: " << projectPath << std::endl;
    return project;
}

RetroProject ProjectManager::openProject(const std::string& projectFile) {
    return RetroProject::loadFromFile(projectFile);
}

bool ProjectManager::isValidProject(const std::string& path) {
    return !findProjectFile(path).empty();
}

std::string ProjectManager::findProjectFile(const std::string& path) {
    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".retro") {
                return entry.path().string();
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error searching for project file: " << e.what() << std::endl;
    }
    return "";
}

std::vector<std::string> ProjectManager::getTemplates() {
    return {
        "Empty Project",
        "2D Platformer",
        "Point & Click Adventure",
        "Top-Down RPG"
    };
}

std::string ProjectManager::getDefaultProjectsPath() {
    const char* home = std::getenv("USERPROFILE");
    if (home) {
        fs::path defaultPath = fs::path(home) / "Documents" / "RetroEngine" / "Projects";
        
        // Create directory if it doesn't exist
        try {
            fs::create_directories(defaultPath);
            std::cout << "Default projects path: " << defaultPath << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Failed to create default projects directory: " << e.what() << std::endl;
        }
        
        return defaultPath.string();
    }
    
    // Fallback to current directory
    return "Projects";
}

void ProjectManager::createFolderStructure(const std::string& projectPath) {
    fs::path root(projectPath);
    
    // Create main folders
    fs::create_directories(root);
    fs::create_directories(root / "assets");
    fs::create_directories(root / "assets/data");
    fs::create_directories(root / "assets/sprites");
    fs::create_directories(root / "assets/sprites/characters");
    fs::create_directories(root / "assets/sprites/items");
    fs::create_directories(root / "assets/sprites/environment");
    fs::create_directories(root / "assets/backgrounds");
    fs::create_directories(root / "assets/audio");
    fs::create_directories(root / "assets/audio/music");
    fs::create_directories(root / "assets/audio/sfx");
    fs::create_directories(root / "assets/fonts");
    fs::create_directories(root / "assets/tilesets");
    fs::create_directories(root / "assets/ui");
    fs::create_directories(root / "assets/icons");
    fs::create_directories(root / "config");
    
    std::cout << "Created folder structure at: " << projectPath << std::endl;
}

void ProjectManager::copyTemplate(const std::string& templateName, const std::string& projectPath) {
    fs::path root(projectPath);
    
    // Create default data files based on template
    if (templateName == "Empty Project") {
        // Create minimal JSON files
        json scenes;
        scenes["scenes"] = json::array();
        std::ofstream(root / "assets/data/scenes.json") << scenes.dump(2);
        
        json world;
        world["world"]["name"] = "World";
        world["levels"] = json::array();
        std::ofstream(root / "assets/data/world.json") << world.dump(2);
        
        json dialogs;
        dialogs["dialogs"] = json::array();
        std::ofstream(root / "assets/data/dialogs.json") << dialogs.dump(2);
        
        json quests;
        quests["quests"] = json::array();
        std::ofstream(root / "assets/data/quests.json") << quests.dump(2);
        
        json items;
        items["items"] = json::array();
        std::ofstream(root / "assets/data/items.json") << items.dump(2);
        
        json npcs;
        npcs["npcs"] = json::array();
        std::ofstream(root / "assets/data/npcs.json") << npcs.dump(2);
        
    } else if (templateName == "Point & Click Adventure") {
        // Create sample room
        json scenes;
        scenes["scenes"] = json::array({
            {
                {"id", "start_room"},
                {"name", "Starting Room"},
                {"background", ""},
                {"walkArea", {{"minX", 50}, {"maxX", 590}, {"minY", 260}, {"maxY", 380}}},
                {"playerSpawnX", 320.0},
                {"playerSpawnY", 300.0},
                {"hotspots", json::array()},
                {"gridPosition", {{"x", 0}, {"y", 0}}}
            }
        });
        std::ofstream(root / "assets/data/scenes.json") << scenes.dump(2);
        
        // Create world with one level
        json world;
        world["world"]["name"] = "Adventure World";
        world["levels"] = json::array({
            {
                {"id", "chapter1"},
                {"name", "Chapter 1"},
                {"scenes", json::array({"start_room"})}
            }
        });
        std::ofstream(root / "assets/data/world.json") << world.dump(2);
        
        // Empty other files
        std::ofstream(root / "assets/data/dialogs.json") << "{\"dialogs\":[]}\n";
        std::ofstream(root / "assets/data/quests.json") << "{\"quests\":[]}\n";
        std::ofstream(root / "assets/data/items.json") << "{\"items\":[]}\n";
        std::ofstream(root / "assets/data/npcs.json") << "{\"npcs\":[]}\n";
    }
    
    // Create README
    std::ofstream readme(root / "README.md");
    readme << "# " << fs::path(projectPath).filename().string() << "\n\n";
    readme << "A Retro Engine project.\n\n";
    readme << "## Getting Started\n\n";
    readme << "Open this project in Retro Engine Editor to start developing.\n";
    
    std::cout << "Copied template: " << templateName << std::endl;
}

RetroProject ProjectManager::createDefaultProject(const std::string& name, const std::string& path) {
    RetroProject project;
    
    project.name = name;
    project.path = path;
    project.version = "1.0.0";
    project.engineVersion = "1.0.0";
    project.description = "A Retro Engine project";
    project.author = "";
    
    // Get current date
    auto now = std::time(nullptr);
    char dateStr[100];
    std::strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", std::localtime(&now));
    project.created = dateStr;
    project.lastModified = dateStr;
    
    // Settings
    project.settings.windowTitle = name;
    project.settings.windowWidth = 1280;
    project.settings.windowHeight = 720;
    project.settings.fullscreen = false;
    project.settings.vsync = true;
    project.settings.targetFPS = 60;
    
    // Paths
    project.paths.assets = "assets";
    project.paths.data = "assets/data";
    project.paths.scenes = "assets/data/scenes.json";
    project.paths.startScene = "";
    
    // Build
    project.build.outputName = name;
    project.build.icon = "assets/icons/game_icon.ico";
    project.build.version = "1.0.0";
    
    return project;
}
