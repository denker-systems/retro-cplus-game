/**
 * @file DataLoader.cpp
 * @brief Implementation av DataLoader
 */
#include "DataLoader.h"
#include "engine/utils/Logger.h"
#include <fstream>
#include <iostream>

DataLoader& DataLoader::instance() {
    static DataLoader instance;
    return instance;
}

bool DataLoader::loadAll(const std::string& dataPath) {
    bool success = true;
    
    success &= loadItems(dataPath + "items.json");
    success &= loadQuests(dataPath + "quests.json");
    success &= loadDialogs(dataPath + "dialogs.json");
    success &= loadScenes(dataPath + "scenes.json");
    success &= loadNPCs(dataPath + "npcs.json");
    
    std::cout << "DataLoader: Loaded " 
              << m_items.size() << " items, "
              << m_quests.size() << " quests, "
              << m_dialogs.size() << " dialogs, "
              << m_scenes.size() << " scenes, "
              << m_npcs.size() << " npcs" << std::endl;
    
    return success;
}

bool DataLoader::loadItems(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open: " << path << std::endl;
        return false;
    }
    
    try {
        json data = json::parse(file);
        m_items = data["items"].get<std::vector<ItemData>>();
        std::cout << "Loaded items from: " << path << std::endl;
        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON error in " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool DataLoader::loadQuests(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open: " << path << std::endl;
        return false;
    }
    
    try {
        json data = json::parse(file);
        m_quests = data["quests"].get<std::vector<QuestData>>();
        std::cout << "Loaded quests from: " << path << std::endl;
        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON error in " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool DataLoader::loadDialogs(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open: " << path << std::endl;
        return false;
    }
    
    try {
        json data = json::parse(file);
        m_dialogs = data["dialogs"].get<std::vector<DialogData>>();
        std::cout << "Loaded dialogs from: " << path << std::endl;
        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON error in " << path << ": " << e.what() << std::endl;
        return false;
    }
}

bool DataLoader::loadScenes(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open: " << path << std::endl;
        return false;
    }
    
    try {
        json data = json::parse(file);
        if (data.find("scenes") == data.end()) {
            LOG_WARNING("scenes.json not found or empty");
            return false;
        }
        m_scenes = data["scenes"].get<std::vector<SceneData>>();
        std::cout << "Loaded scenes from: " << path << std::endl;
        
        // DEBUG: Print first scene's hotspot positions
        if (!m_scenes.empty()) {
            const auto& firstScene = m_scenes[0];
            std::cout << "DEBUG: First scene '" << firstScene.name << "' hotspots:" << std::endl;
            for (const auto& hs : firstScene.hotspots) {
                std::cout << "  - " << hs.name << " at (" << hs.x << ", " << hs.y << ")" << std::endl;
            }
        }
        
        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON error in " << path << ": " << e.what() << std::endl;
        return false;
    }
}

// Legacy alias for backward compatibility
bool DataLoader::loadRooms(const std::string& path) {
    return loadScenes(path);
}

bool DataLoader::loadNPCs(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open: " << path << std::endl;
        return false;
    }
    
    try {
        json data = json::parse(file);
        m_npcs = data["npcs"].get<std::vector<NPCData>>();
        std::cout << "Loaded npcs from: " << path << std::endl;
        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON error in " << path << ": " << e.what() << std::endl;
        return false;
    }
}
