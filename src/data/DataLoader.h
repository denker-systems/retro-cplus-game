/**
 * @file DataLoader.h
 * @brief Laddar speldata från JSON-filer
 */
#pragma once

#include "GameData.h"
#include <string>
#include <vector>

/**
 * @brief Singleton för att ladda speldata från JSON
 */
class DataLoader {
public:
    static DataLoader& instance();
    
    /** @brief Ladda alla datafiler från assets/data/ */
    bool loadAll(const std::string& dataPath = "assets/data/");
    
    /** @brief Hämta all item-data */
    const std::vector<ItemData>& getItems() const { return m_items; }
    
    /** @brief Hämta all quest-data */
    const std::vector<QuestData>& getQuests() const { return m_quests; }
    
    /** @brief Hämta all dialog-data */
    const std::vector<DialogData>& getDialogs() const { return m_dialogs; }
    
    /** @brief Hämta all rum-data */
    const std::vector<RoomData>& getRooms() const { return m_rooms; }
    
    /** @brief Hämta all NPC-data */
    const std::vector<NPCData>& getNPCs() const { return m_npcs; }

private:
    DataLoader() = default;
    
    bool loadItems(const std::string& path);
    bool loadQuests(const std::string& path);
    bool loadDialogs(const std::string& path);
    bool loadRooms(const std::string& path);
    bool loadNPCs(const std::string& path);
    
    std::vector<ItemData> m_items;
    std::vector<QuestData> m_quests;
    std::vector<DialogData> m_dialogs;
    std::vector<RoomData> m_rooms;
    std::vector<NPCData> m_npcs;
};
