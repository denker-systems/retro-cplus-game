/**
 * @file SaveSystem.cpp
 * @brief Implementation av SaveSystem
 */
#include "SaveSystem.h"
#include "InventorySystem.h"
#include "QuestSystem.h"
#include "RoomManager.h"
#include "AISystem.h"
#include <fstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

SaveSystem& SaveSystem::instance() {
    static SaveSystem inst;
    return inst;
}

std::string SaveSystem::getSavePath(const std::string& slotName) const {
    return "saves/" + slotName + ".json";
}

bool SaveSystem::save(const std::string& slotName) {
    // Skapa saves-mappen om den inte finns
    fs::create_directories("saves");
    
    SaveData data = gatherSaveData();
    data.saveName = slotName;
    
    // Sätt timestamp
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    data.timestamp = ss.str();
    
    // Serialisera och spara
    std::string path = getSavePath(slotName);
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open save file: " << path << std::endl;
        return false;
    }
    
    json j = data;
    file << j.dump(2);  // Pretty print med 2 spaces
    file.close();
    
    std::cout << "Game saved to: " << path << std::endl;
    return true;
}

bool SaveSystem::load(const std::string& slotName) {
    std::string path = getSavePath(slotName);
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Save file not found: " << path << std::endl;
        return false;
    }
    
    try {
        json j;
        file >> j;
        SaveData data = j.get<SaveData>();
        applySaveData(data);
        
        std::cout << "Game loaded from: " << path << std::endl;
        return true;
    } catch (const json::exception& e) {
        std::cerr << "Failed to parse save file: " << e.what() << std::endl;
        return false;
    }
}

bool SaveSystem::saveExists(const std::string& slotName) const {
    return fs::exists(getSavePath(slotName));
}

bool SaveSystem::deleteSave(const std::string& slotName) {
    std::string path = getSavePath(slotName);
    if (fs::exists(path)) {
        return fs::remove(path);
    }
    return false;
}

std::vector<std::string> SaveSystem::getSaveSlots() const {
    std::vector<std::string> slots;
    
    if (!fs::exists("saves")) {
        return slots;
    }
    
    for (const auto& entry : fs::directory_iterator("saves")) {
        if (entry.path().extension() == ".json") {
            slots.push_back(entry.path().stem().string());
        }
    }
    
    return slots;
}

bool SaveSystem::getSaveInfo(const std::string& slotName, SaveData& outData) const {
    std::string path = getSavePath(slotName);
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        outData = j.get<SaveData>();
        return true;
    } catch (const json::exception&) {
        return false;
    }
}

void SaveSystem::setFlag(const std::string& flag, bool value) {
    m_flags[flag] = value;
}

bool SaveSystem::getFlag(const std::string& flag) const {
    auto it = m_flags.find(flag);
    return (it != m_flags.end()) ? it->second : false;
}

void SaveSystem::setCounter(const std::string& counter, int value) {
    m_counters[counter] = value;
}

int SaveSystem::getCounter(const std::string& counter) const {
    auto it = m_counters.find(counter);
    return (it != m_counters.end()) ? it->second : 0;
}

void SaveSystem::incrementCounter(const std::string& counter, int amount) {
    m_counters[counter] = getCounter(counter) + amount;
}

void SaveSystem::updatePlayTime(float deltaTime) {
    m_playTimeAccumulator += deltaTime;
    while (m_playTimeAccumulator >= 1.0f) {
        m_playTimeAccumulator -= 1.0f;
        m_playTimeSeconds++;
    }
}

SaveData SaveSystem::gatherSaveData() const {
    SaveData data;
    
    // Speltid
    data.playTimeSeconds = m_playTimeSeconds;
    
    // Aktuellt rum och spelarposition
    data.currentRoom = RoomManager::instance().getCurrentRoomId();
    // Spelarposition hämtas via callback (PlayState äger spelaren)
    // För nu, använd spawn-position
    RoomManager::instance().getSpawnPosition(data.playerX, data.playerY);
    
    // Inventory
    const auto& items = InventorySystem::instance().getItems();
    for (const auto& item : items) {
        if (!item.id.empty()) {
            data.inventoryItems.push_back(item.id);
        }
    }
    
    // Quests
    const auto& activeQuests = QuestSystem::instance().getActiveQuests();
    const auto& completedQuests = QuestSystem::instance().getCompletedQuests();
    
    for (const auto& quest : activeQuests) {
        SaveData::QuestState qs;
        qs.questId = quest.id;
        qs.started = true;
        qs.completed = false;
        for (const auto& obj : quest.objectives) {
            if (obj.status == QuestStatus::Completed) {
                qs.completedObjectives.push_back(obj.id);
            }
        }
        data.quests.push_back(qs);
    }
    
    for (const auto& quest : completedQuests) {
        SaveData::QuestState qs;
        qs.questId = quest.id;
        qs.started = true;
        qs.completed = true;
        data.quests.push_back(qs);
    }
    
    // Game time
    data.gameHour = AISystem::instance().getGameHour();
    data.gameMinute = AISystem::instance().getGameMinute();
    
    // Flags och counters
    data.flags = m_flags;
    data.counters = m_counters;
    
    return data;
}

void SaveSystem::applySaveData(const SaveData& data) {
    // Återställ speltid
    m_playTimeSeconds = data.playTimeSeconds;
    
    // Byt rum
    RoomManager::instance().setSpawnPosition(data.playerX, data.playerY);
    RoomManager::instance().changeRoom(data.currentRoom);
    
    // Återställ inventory
    InventorySystem::instance().clear();
    for (const auto& itemId : data.inventoryItems) {
        InventorySystem::instance().addItem(itemId);
    }
    
    // Återställ quests
    for (const auto& qs : data.quests) {
        if (qs.started) {
            QuestSystem::instance().startQuest(qs.questId);
            
            // Markera completed objectives
            for (const auto& objId : qs.completedObjectives) {
                QuestSystem::instance().completeObjectiveById(qs.questId, objId);
            }
            
            if (qs.completed) {
                QuestSystem::instance().completeQuest(qs.questId);
            }
        }
    }
    
    // Återställ game time
    AISystem::instance().setGameTime(data.gameHour, data.gameMinute);
    
    // Återställ flags och counters
    m_flags = data.flags;
    m_counters = data.counters;
}
