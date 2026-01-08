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

// ============================================================================
// STATIC ACCESSORS FOR AI TOOLS
// ============================================================================

const SceneData* DataLoader::getSceneById(const std::string& id) {
    for (const auto& scene : instance().m_scenes) {
        if (scene.id == id) return &scene;
    }
    return nullptr;
}

SceneData* DataLoader::getSceneByIdMutable(const std::string& id) {
    for (auto& scene : instance().m_scenes) {
        if (scene.id == id) return &scene;
    }
    return nullptr;
}

const DialogData* DataLoader::getDialogById(const std::string& id) {
    for (const auto& dialog : instance().m_dialogs) {
        if (dialog.id == id) return &dialog;
    }
    return nullptr;
}

DialogData* DataLoader::getDialogByIdMutable(const std::string& id) {
    for (auto& dialog : instance().m_dialogs) {
        if (dialog.id == id) return &dialog;
    }
    return nullptr;
}

const ItemData* DataLoader::getItemById(const std::string& id) {
    for (const auto& item : instance().m_items) {
        if (item.id == id) return &item;
    }
    return nullptr;
}

ItemData* DataLoader::getItemByIdMutable(const std::string& id) {
    for (auto& item : instance().m_items) {
        if (item.id == id) return &item;
    }
    return nullptr;
}

bool DataLoader::addDialog(const DialogData& dialog) {
    instance().m_dialogs.push_back(dialog);
    return true;
}

bool DataLoader::addItem(const ItemData& item) {
    instance().m_items.push_back(item);
    return true;
}

bool DataLoader::saveScenes() {
    try {
        json data;
        data["scenes"] = instance().m_scenes;
        
        std::ofstream file("assets/data/scenes.json");
        if (!file.is_open()) {
            LOG_ERROR("Could not open scenes.json for writing");
            return false;
        }
        file << data.dump(2);
        LOG_INFO("Saved scenes to assets/data/scenes.json");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save scenes: " + std::string(e.what()));
        return false;
    }
}

bool DataLoader::saveDialogs() {
    try {
        json data;
        data["dialogs"] = instance().m_dialogs;
        
        std::ofstream file("assets/data/dialogs.json");
        if (!file.is_open()) {
            LOG_ERROR("Could not open dialogs.json for writing");
            return false;
        }
        file << data.dump(2);
        LOG_INFO("Saved dialogs to assets/data/dialogs.json");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save dialogs: " + std::string(e.what()));
        return false;
    }
}

bool DataLoader::saveItems() {
    try {
        json data;
        data["items"] = instance().m_items;
        
        std::ofstream file("assets/data/items.json");
        if (!file.is_open()) {
            LOG_ERROR("Could not open items.json for writing");
            return false;
        }
        file << data.dump(2);
        LOG_INFO("Saved items to assets/data/items.json");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save items: " + std::string(e.what()));
        return false;
    }
}

bool DataLoader::saveNPCs() {
    try {
        json data;
        data["npcs"] = instance().m_npcs;
        
        std::ofstream file("assets/data/npcs.json");
        if (!file.is_open()) {
            LOG_ERROR("Could not open npcs.json for writing");
            return false;
        }
        file << data.dump(2);
        LOG_INFO("Saved npcs to assets/data/npcs.json");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save npcs: " + std::string(e.what()));
        return false;
    }
}

bool DataLoader::saveQuests() {
    try {
        json data;
        data["quests"] = instance().m_quests;
        
        std::ofstream file("assets/data/quests.json");
        if (!file.is_open()) {
            LOG_ERROR("Could not open quests.json for writing");
            return false;
        }
        file << data.dump(2);
        LOG_INFO("Saved quests to assets/data/quests.json");
        return true;
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to save quests: " + std::string(e.what()));
        return false;
    }
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
