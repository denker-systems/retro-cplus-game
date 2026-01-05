/**
 * @file SaveSystem.h
 * @brief System för att spara och ladda spelstatus
 */
#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

/**
 * @brief Sparad speldata
 */
struct SaveData {
    // Metadata
    std::string saveName;
    std::string timestamp;
    int playTimeSeconds = 0;
    
    // Player
    std::string currentRoom;
    float playerX = 0.0f;
    float playerY = 0.0f;
    
    // Inventory
    std::vector<std::string> inventoryItems;
    
    // Quests
    struct QuestState {
        std::string questId;
        bool started = false;
        bool completed = false;
        std::vector<std::string> completedObjectives;
    };
    std::vector<QuestState> quests;
    
    // Game time (för AI-scheman)
    int gameHour = 12;
    int gameMinute = 0;
    
    // Flags (för custom game state)
    std::unordered_map<std::string, bool> flags;
    std::unordered_map<std::string, int> counters;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SaveData::QuestState,
    questId, started, completed, completedObjectives)

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SaveData,
    saveName, timestamp, playTimeSeconds,
    currentRoom, playerX, playerY,
    inventoryItems, quests,
    gameHour, gameMinute,
    flags, counters)

/**
 * @brief Singleton för save/load hantering
 */
class SaveSystem {
public:
    static SaveSystem& instance();
    
    /** @brief Spara aktuell spelstatus till fil */
    bool save(const std::string& slotName);
    
    /** @brief Ladda spelstatus från fil */
    bool load(const std::string& slotName);
    
    /** @brief Kolla om en sparfil finns */
    bool saveExists(const std::string& slotName) const;
    
    /** @brief Radera en sparfil */
    bool deleteSave(const std::string& slotName);
    
    /** @brief Hämta lista över alla sparfiler */
    std::vector<std::string> getSaveSlots() const;
    
    /** @brief Hämta metadata för en sparfil */
    bool getSaveInfo(const std::string& slotName, SaveData& outData) const;
    
    /** @brief Sätt/hämta game flags */
    void setFlag(const std::string& flag, bool value);
    bool getFlag(const std::string& flag) const;
    
    /** @brief Sätt/hämta counters */
    void setCounter(const std::string& counter, int value);
    int getCounter(const std::string& counter) const;
    void incrementCounter(const std::string& counter, int amount = 1);
    
    /** @brief Uppdatera speltid (kalla varje frame) */
    void updatePlayTime(float deltaTime);
    int getPlayTimeSeconds() const { return m_playTimeSeconds; }

private:
    SaveSystem() = default;
    
    /** @brief Samla ihop aktuell spelstatus */
    SaveData gatherSaveData() const;
    
    /** @brief Applicera laddad data till spelsystemen */
    void applySaveData(const SaveData& data);
    
    /** @brief Hämta sökväg till sparfiler */
    std::string getSavePath(const std::string& slotName) const;
    
    std::unordered_map<std::string, bool> m_flags;
    std::unordered_map<std::string, int> m_counters;
    
    int m_playTimeSeconds = 0;
    float m_playTimeAccumulator = 0.0f;
};
