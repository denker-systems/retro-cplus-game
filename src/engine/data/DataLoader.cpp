/**
 * @file DataLoader.cpp
 * @brief Implementation av DataLoader
 */
#include "DataLoader.h"
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
    success &= loadRooms(dataPath + "rooms.json");
    success &= loadNPCs(dataPath + "npcs.json");
    
    std::cout << "DataLoader: Loaded " 
              << m_items.size() << " items, "
              << m_quests.size() << " quests, "
              << m_dialogs.size() << " dialogs, "
              << m_rooms.size() << " rooms, "
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

bool DataLoader::loadRooms(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open: " << path << std::endl;
        return false;
    }
    
    try {
        json data = json::parse(file);
        m_rooms = data["rooms"].get<std::vector<RoomData>>();
        std::cout << "Loaded rooms from: " << path << std::endl;
        return true;
    } catch (const json::exception& e) {
        std::cerr << "JSON error in " << path << ": " << e.what() << std::endl;
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
