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
    
    /** @brief Hämta all scene-data (formerly room-data) */
    const std::vector<SceneData>& getScenes() const { return m_scenes; }
    std::vector<SceneData>& getScenes() { return m_scenes; }
    
    /** @brief Legacy aliases for backward compatibility */
    const std::vector<RoomData>& getRooms() const { return m_scenes; }
    std::vector<RoomData>& getRooms() { return m_scenes; }
    
    /** @brief Hämta all NPC-data */
    const std::vector<NPCData>& getNPCs() const { return m_npcs; }

private:
    DataLoader() = default;
    
    bool loadItems(const std::string& path);
    bool loadQuests(const std::string& path);
    bool loadDialogs(const std::string& path);
    bool loadScenes(const std::string& path);  // Formerly loadRooms
    bool loadRooms(const std::string& path);  // Legacy alias
    bool loadNPCs(const std::string& path);
    
    std::vector<ItemData> m_items;
    std::vector<QuestData> m_quests;
    std::vector<DialogData> m_dialogs;
    std::vector<SceneData> m_scenes;  // Renamed from m_rooms
    std::vector<NPCData> m_npcs;
};
