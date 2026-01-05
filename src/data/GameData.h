/**
 * @file GameData.h
 * @brief Centraliserade datastrukturer för spelinnehåll
 * 
 * Använder nlohmann/json för serialisering/deserialisering.
 * All speldata laddas från JSON-filer i assets/data/
 */
#pragma once

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

// Forward declarations
struct ItemData;
struct QuestData;
struct ObjectiveData;
struct DialogData;
struct DialogNodeData;
struct DialogChoiceData;
struct RoomData;
struct HotspotData;

// ============================================================================
// ITEM DATA
// ============================================================================

struct ItemData {
    std::string id;
    std::string name;
    std::string description;
    std::string icon;           // Sprite/textur-namn
    bool combinable = false;
    std::string combinesWith;
    std::string combineResult;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ItemData,
    id, name, description, icon, combinable, combinesWith, combineResult)

// ============================================================================
// QUEST DATA
// ============================================================================

struct ObjectiveData {
    std::string id;
    std::string description;
    std::string type;           // "talk", "collect", "deliver", "goto", "examine"
    std::string targetId;
    int requiredCount = 1;
    bool optional = false;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ObjectiveData,
    id, description, type, targetId, requiredCount, optional)

struct QuestData {
    std::string id;
    std::string title;
    std::string description;
    std::vector<ObjectiveData> objectives;
    std::string rewardItem;
    int rewardXP = 0;
    bool autoStart = false;     // Starta automatiskt vid spelstart
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(QuestData,
    id, title, description, objectives, rewardItem, rewardXP, autoStart)

// ============================================================================
// DIALOG DATA
// ============================================================================

struct DialogChoiceData {
    std::string text;
    int nextNodeId = -1;
    std::string condition;      // Villkor (t.ex. "has_item:rusty_key")
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DialogChoiceData,
    text, nextNodeId, condition)

struct DialogNodeData {
    int id = 0;
    std::string speaker;
    std::string text;
    std::vector<DialogChoiceData> choices;
    int nextNodeId = -1;        // Auto-advance om inga choices
    std::string action;         // Action att köra (t.ex. "give_item:gold_coin")
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DialogNodeData,
    id, speaker, text, choices, nextNodeId, action)

struct DialogData {
    std::string id;
    std::string npcName;
    int startNodeId = 0;
    std::vector<DialogNodeData> nodes;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DialogData,
    id, npcName, startNodeId, nodes)

// ============================================================================
// NPC/CHARACTER DATA
// ============================================================================

struct NPCData {
    std::string id;
    std::string name;
    std::string description;
    std::string sprite;         // Sprite-sheet namn
    std::string dialogId;       // Standard-dialog
    std::string room;           // Vilket rum NPC:n är i
    int x = 0, y = 0;           // Position i rummet
    bool canTalk = true;
    bool canMove = false;
    float moveSpeed = 50.0f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NPCData,
    id, name, description, sprite, dialogId, room, x, y, canTalk, canMove, moveSpeed)

// ============================================================================
// ROOM DATA
// ============================================================================

struct HotspotData {
    std::string id;
    std::string name;
    std::string type;           // "npc", "item", "exit", "examine"
    int x, y, w, h;
    std::string targetRoom;     // För exits
    std::string dialogId;       // För NPCs
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(HotspotData,
    id, name, type, x, y, w, h, targetRoom, dialogId)

struct WalkAreaData {
    int minX, maxX, minY, maxY;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WalkAreaData,
    minX, maxX, minY, maxY)

struct RoomData {
    std::string id;
    std::string name;
    std::string background;     // Bakgrundsbild
    WalkAreaData walkArea;
    std::vector<HotspotData> hotspots;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RoomData,
    id, name, background, walkArea, hotspots)
