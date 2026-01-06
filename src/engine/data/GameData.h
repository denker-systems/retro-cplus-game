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
#include <optional>
#include "engine/world/GridTypes.h"

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
    std::string tone;           // Ton: "friendly", "aggressive", "sarcastic", "neutral"
    std::string preview;        // Kort preview av intention (valfritt)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(DialogChoiceData,
    text, nextNodeId, condition, tone, preview)

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
    std::string examineText;    // "Titta på" beskrivning
    std::vector<std::string> funnyFails;  // Roliga svar på dumma försök
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(HotspotData,
    id, name, type, x, y, w, h, targetRoom, dialogId, examineText, funnyFails)

struct WalkAreaData {
    int minX, maxX, minY, maxY;
    float scaleTop = 0.5f;      // Skala vid minY (längre bort)
    float scaleBottom = 1.0f;   // Skala vid maxY (närmare)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(WalkAreaData,
    minX, maxX, minY, maxY, scaleTop, scaleBottom)

struct LayerData {
    std::string image;          // Bildfil
    int zIndex = 0;             // Djup (negativa = bakom spelare, positiva = framför)
    float parallaxX = 1.0f;     // Parallax-faktor X (1.0 = normal)
    float parallaxY = 1.0f;     // Parallax-faktor Y (1.0 = normal)
    float opacity = 1.0f;       // Genomskinlighet (0.0-1.0)
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(LayerData,
    image, zIndex, parallaxX, parallaxY, opacity)

struct RoomData {
    std::string id;
    std::string name;
    std::string background;     // Bakgrundsbild (legacy, använd layers istället)
    std::vector<LayerData> layers;  // Multi-layer rendering
    WalkAreaData walkArea;
    std::vector<HotspotData> hotspots;
    float playerSpawnX = 320.0f;  // Player spawn X
    float playerSpawnY = 300.0f;  // Player spawn Y
    
    // New: Optional grid data (for spatial hierarchy)
    std::optional<engine::GridPosition> gridPosition;
    std::optional<engine::CameraConfig> camera;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(RoomData,
    id, name, background, layers, walkArea, hotspots, playerSpawnX, playerSpawnY,
    gridPosition, camera)
