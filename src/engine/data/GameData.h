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
struct SceneData;
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
    
    // Map waypoint for navigation
    std::string targetScene;    // Scene where objective target is located
    float targetX = 0.0f;       // 3D world position X
    float targetY = 0.0f;       // 3D world position Y (height)
    float targetZ = 0.0f;       // 3D world position Z
    bool showOnCompass = true;  // Show direction indicator in HUD
    std::string waypointIcon;   // Custom icon (empty = default based on type)
    float waypointRadius = 2.0f; // Distance to consider "arrived"
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ObjectiveData,
    id, description, type, targetId, requiredCount, optional,
    targetScene, targetX, targetY, targetZ, showOnCompass, waypointIcon, waypointRadius)

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
// PHYSICS DATA (shared by all actors)
// ============================================================================

struct ColliderData {
    std::string shape = "box";  // "box", "circle", "capsule"
    float width = 32.0f;
    float height = 32.0f;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    bool isTrigger = false;     // True = trigger events, no solid collision
    float density = 1.0f;
    float friction = 0.3f;
    float restitution = 0.0f;   // Bounciness
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ColliderData,
    shape, width, height, offsetX, offsetY, isTrigger, density, friction, restitution)

struct PhysicsData {
    bool enabled = false;       // Physics active for this actor?
    std::string bodyType = "static";  // "static", "dynamic", "kinematic"
    bool fixedRotation = true;  // Prevent rotation
    float gravityScale = 1.0f;  // 0 = no gravity
    ColliderData collider;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(PhysicsData,
    enabled, bodyType, fixedRotation, gravityScale, collider)

// ============================================================================
// COLLISION BOX DATA (static level geometry - platforms, walls, ground)
// ============================================================================

struct CollisionBoxData {
    std::string id;
    std::string type = "ground";  // "ground", "wall", "platform", "ceiling", "hazard"
    float x = 0, y = 0;           // Position
    float width = 64, height = 32; // Size
    bool oneWay = false;          // Platform you can jump through from below
    std::string tag;              // Optional tag for scripting
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(CollisionBoxData,
    id, type, x, y, width, height, oneWay, tag)

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
    
    // Interaction volume (3D bounding box for interaction detection)
    float interactVolumeX = 3.0f;  // Width (X-axis)
    float interactVolumeY = 2.0f;  // Height (Y-axis)
    float interactVolumeZ = 3.0f;  // Depth (Z-axis)
    
    // Physics
    PhysicsData physics;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(NPCData,
    id, name, description, sprite, dialogId, room, x, y, canTalk, canMove, moveSpeed, interactVolumeX, interactVolumeY, interactVolumeZ, physics)

// ============================================================================
// SCENE DATA (formerly RoomData)
// ============================================================================

struct HotspotData {
    std::string id;
    std::string name;
    std::string type;           // "npc", "item", "exit", "examine", "gateway"
    int x, y, w, h;
    std::string targetScene;    // För exits (target scene id)
    std::string targetLevel;    // För gateways (target level id)
    std::string targetWorld;    // För gateways (target world id)
    std::string dialogId;       // För NPCs
    std::string examineText;    // "Titta på" beskrivning
    std::vector<std::string> funnyFails;  // Roliga svar på dumma försök
    
    // Interaction volume (3D bounding box for interaction detection)
    float interactVolumeX = 2.0f;  // Width (X-axis)
    float interactVolumeY = 1.5f;  // Height (Y-axis)
    float interactVolumeZ = 2.0f;  // Depth (Z-axis)
    
    // Physics (triggers, collision)
    PhysicsData physics;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(HotspotData,
    id, name, type, x, y, w, h, targetScene, targetLevel, targetWorld, dialogId, examineText, funnyFails, interactVolumeX, interactVolumeY, interactVolumeZ, physics)

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

// Actor data for scene-level actors (3D)
struct SceneActorData {
    std::string id;
    std::string name;
    std::string type;           // "PlayerStart", "PlayerConfig", "NPC3D", "StaticMesh", etc.
    float x = 0.0f, y = 0.0f, z = 0.0f;  // 3D position
    float rotationY = 0.0f;     // Yaw rotation
    std::string sprite;         // Sprite name (for NPCs)
    std::string dialogId;       // Dialog ID (for NPCs)
    std::map<std::string, std::string> properties;  // Additional properties
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SceneActorData,
    id, name, type, x, y, z, rotationY, sprite, dialogId, properties)

struct SceneData {
    std::string id;
    std::string name;
    std::string background;     // Bakgrundsbild (legacy, använd layers istället)
    std::vector<LayerData> layers;  // Multi-layer rendering
    WalkAreaData walkArea;
    std::vector<HotspotData> hotspots;
    std::vector<CollisionBoxData> collisionBoxes;  // Static level geometry
    std::vector<SceneActorData> actors;  // NEW: All 3D actors in scene
    float playerSpawnX = 320.0f;  // Player spawn X
    float playerSpawnY = 300.0f;  // Player spawn Y
    
    // Grid data for spatial hierarchy
    std::optional<engine::GridPosition> gridPosition;
    std::optional<engine::CameraConfig> camera;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(SceneData,
    id, name, background, layers, walkArea, hotspots, collisionBoxes, actors, playerSpawnX, playerSpawnY,
    gridPosition, camera)

// Legacy alias for backward compatibility
using RoomData = SceneData;
