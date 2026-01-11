/**
 * @file EditorContext.cpp
 * @brief Implementation of shared editor context
 */
#include "editor/core/EditorContext.h"
#include "editor/core/EditorState.h"
#include "editor/managers/EditorWorldManager.h"
#include "editor/data/EditorDataManager.h"
#include "game/Game.h"
#include "engine/data/DataLoader.h"
#include "engine/utils/Logger.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/PlayerConfigActor.h"
#include "engine/actors/NPC3DActor.h"
#include <nlohmann/json.hpp>
#include <SDL_image.h>
#include <fstream>
#include <filesystem>
#include <iostream>

using json = nlohmann::json;

EditorContext::~EditorContext() {
    clearTextureCache();
}

void EditorContext::setSelection(::SelectionType type, const std::string& id, int index) {
    m_selection.type = type;
    m_selection.primaryId = id;
    m_selection.secondaryIndex = index;
}

void EditorContext::clearSelection() {
    m_selection.clear();
    
    // Clear direct selection fields too
    selectedType = SelectionType::None;
    selectedRoomId.clear();
    selectedDialogId.clear();
    selectedQuestId.clear();
    selectedItemId.clear();
    selectedNPCId.clear();
    selectedLevelId.clear();
    selectedSceneId.clear();
    selectedHotspotIndex = -1;
    selectedActorId.clear();
    selectedCollisionBoxIndex = -1;
}

void EditorContext::copyToClipboard(const std::string& type, const std::any& data) {
    m_clipboard[type] = data;
}

bool EditorContext::hasClipboardData(const std::string& type) const {
    return m_clipboard.find(type) != m_clipboard.end();
}

std::any EditorContext::getClipboardData(const std::string& type) const {
    auto it = m_clipboard.find(type);
    return it != m_clipboard.end() ? it->second : std::any{};
}

SDL_Texture* EditorContext::getTexture(SDL_Renderer* renderer, const std::string& path) {
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) {
        return it->second;
    }
    
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (texture) {
        m_textureCache[path] = texture;
    } else {
        LOG_WARNING("Failed to load texture: " + path);
    }
    
    return texture;
}

void EditorContext::clearTextureCache() {
    for (auto& pair : m_textureCache) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    m_textureCache.clear();
}

void EditorContext::loadFromDataLoader() {
    // Try to load current project info from .current_project file
    std::ifstream projectFile(".current_project");
    if (projectFile.is_open()) {
        std::getline(projectFile, currentProjectPath);
        std::getline(projectFile, currentProjectName);
        projectFile.close();
        
        if (!currentProjectPath.empty()) {
            std::cout << "[EditorContext] Loaded project info from .current_project" << std::endl;
            std::cout << "[EditorContext] Project: " << currentProjectName << std::endl;
            std::cout << "[EditorContext] Path: " << currentProjectPath << std::endl;
        }
    }
    
    // Copy data from DataLoader (make mutable copies for editing)
    const auto& dl = DataLoader::instance();
    
    rooms = dl.getRooms();
    dialogs = dl.getDialogs();
    quests = dl.getQuests();
    items = dl.getItems();
    npcs = dl.getNPCs();
    
    LOG_INFO("EditorContext loaded " + std::to_string(rooms.size()) + " rooms, " +
             std::to_string(dialogs.size()) + " dialogs");
}

void EditorContext::saveToFiles() {
    LOG_INFO("=== SAVE: Starting full hierarchy sync ===");
    
    // STEP 1: Sync World/Level data (world.json)
    if (m_editorState) {
        auto* worldMgr = m_editorState->getWorldManager();
        if (worldMgr) {
            // Sync and save world.json (includes Level grid positions)
            worldMgr->saveWorldData();
        }
    }
    
    // STEP 2: Sync Scene grid positions to RoomData
    syncScenesToRoomData();
    
    // STEP 3: Sync back to DataLoader since we read from there (by ID, not index)
    LOG_INFO("=== SAVE: Syncing to DataLoader ===");
    LOG_INFO("EditorContext.rooms has " + std::to_string(rooms.size()) + " rooms");
    for (auto& editorRoom : rooms) {
        LOG_INFO("  EditorContext room: '" + editorRoom.id + "' with " + std::to_string(editorRoom.actors.size()) + " actors");
    }
    
    auto& dataLoader = DataLoader::instance();
    LOG_INFO("DataLoader.rooms has " + std::to_string(dataLoader.getRooms().size()) + " rooms");
    for (auto& dlRoom : dataLoader.getRooms()) {
        LOG_INFO("  DataLoader room: '" + dlRoom.id + "' with " + std::to_string(dlRoom.actors.size()) + " actors");
    }
    
    int syncCount = 0;
    for (auto& editorRoom : rooms) {
        // Find matching room in DataLoader by ID
        bool found = false;
        for (auto& dlRoom : dataLoader.getRooms()) {
            if (dlRoom.id == editorRoom.id) {
                dlRoom.gridPosition = editorRoom.gridPosition;
                dlRoom.actors = editorRoom.actors;
                LOG_INFO("MATCHED: Synced " + std::to_string(editorRoom.actors.size()) + " actors to DataLoader for room: " + editorRoom.id);
                syncCount++;
                found = true;
                break;
            }
        }
        if (!found) {
            LOG_WARNING("NO MATCH for EditorContext room: '" + editorRoom.id + "'");
        }
    }
    LOG_INFO("=== SAVE: Synced " + std::to_string(syncCount) + " rooms to DataLoader ===");
    
    // Save rooms - use DataLoader's rooms since ViewportPanel modifies those directly
    {
        json data;
        data["scenes"] = json::array();
        
        const auto& roomsToSave = DataLoader::instance().getRooms();
        
        if (roomsToSave.empty()) {
            LOG_WARNING("No rooms to save - aborting to prevent data loss");
            return;
        }
        
        for (const auto& room : roomsToSave) {
            json roomJson;
            roomJson["id"] = room.id;
            roomJson["name"] = room.name;
            roomJson["background"] = room.background;
            
            // Walk area
            roomJson["walkArea"]["minX"] = room.walkArea.minX;
            roomJson["walkArea"]["maxX"] = room.walkArea.maxX;
            roomJson["walkArea"]["minY"] = room.walkArea.minY;
            roomJson["walkArea"]["maxY"] = room.walkArea.maxY;
            roomJson["walkArea"]["scaleTop"] = room.walkArea.scaleTop;
            roomJson["walkArea"]["scaleBottom"] = room.walkArea.scaleBottom;
            
            // Player spawn
            roomJson["playerSpawnX"] = room.playerSpawnX;
            roomJson["playerSpawnY"] = room.playerSpawnY;
            
            // Layers
            roomJson["layers"] = json::array();
            for (const auto& layer : room.layers) {
                json layerJson;
                layerJson["image"] = layer.image;
                layerJson["zIndex"] = layer.zIndex;
                layerJson["parallaxX"] = layer.parallaxX;
                layerJson["parallaxY"] = layer.parallaxY;
                layerJson["opacity"] = layer.opacity;
                roomJson["layers"].push_back(layerJson);
            }
            
            // Hotspots
            roomJson["hotspots"] = json::array();
            for (const auto& hs : room.hotspots) {
                json hsJson;
                hsJson["id"] = hs.id;
                hsJson["name"] = hs.name;
                hsJson["type"] = hs.type;
                hsJson["x"] = hs.x;
                hsJson["y"] = hs.y;
                hsJson["w"] = hs.w;
                hsJson["h"] = hs.h;
                LOG_INFO("  Saving hotspot '" + hs.name + "' at (" + std::to_string(hs.x) + "," + std::to_string(hs.y) + ")");
                
                if (!hs.dialogId.empty()) hsJson["dialogId"] = hs.dialogId;
                if (!hs.examineText.empty()) hsJson["examineText"] = hs.examineText;
                if (!hs.targetScene.empty()) hsJson["targetRoom"] = hs.targetScene;
                
                if (!hs.funnyFails.empty()) {
                    hsJson["funnyFails"] = hs.funnyFails;
                }
                
                roomJson["hotspots"].push_back(hsJson);
            }
            
            // Grid position (NEW)
            if (room.gridPosition) {
                roomJson["gridPosition"] = *room.gridPosition;
            }
            
            // Camera config (NEW)
            if (room.camera) {
                roomJson["camera"] = *room.camera;
            }
            
            // Actors - get DIRECTLY from engine::Scene via WorldManager
            if (m_editorState) {
                auto* worldMgr = m_editorState->getWorldManager();
                if (worldMgr) {
                    auto* engineScene = worldMgr->findSceneById(room.id);
                    if (engineScene && !engineScene->getActors().empty()) {
                        roomJson["actors"] = json::array();
                        for (const auto& actor : engineScene->getActors()) {
                            std::string actorName = actor->getName();
                            if (actorName == "Background") continue;
                            
                            json actorJson;
                            actorJson["id"] = actorName;
                            actorJson["name"] = actorName;
                            
                            // Determine type and position
                            if (auto* ps = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
                                actorJson["type"] = "PlayerStart";
                                glm::vec3 pos = ps->getSpawnPosition();
                                actorJson["x"] = pos.x;
                                actorJson["y"] = pos.y;
                                actorJson["z"] = pos.z;
                            } else if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
                                actorJson["type"] = "NPC3D";
                                glm::vec3 pos = npc->getPosition3D();
                                actorJson["x"] = pos.x;
                                actorJson["y"] = pos.y;
                                actorJson["z"] = pos.z;
                                if (!npc->getDialogId().empty()) actorJson["dialogId"] = npc->getDialogId();
                            } else if (auto* pc = dynamic_cast<engine::PlayerConfigActor*>(actor.get())) {
                                actorJson["type"] = "PlayerConfig";
                                actorJson["x"] = 0.0f;
                                actorJson["y"] = 0.0f;
                                actorJson["z"] = 0.0f;
                            } else {
                                actorJson["type"] = "Generic";
                                actorJson["x"] = actor->getX() / 100.0f;
                                actorJson["y"] = 1.0f;
                                actorJson["z"] = actor->getY() / 100.0f;
                            }
                            
                            roomJson["actors"].push_back(actorJson);
                        }
                        LOG_INFO("  Saved " + std::to_string(roomJson["actors"].size()) + " actors for scene '" + room.id + "'");
                    }
                }
            }
            
            data["scenes"].push_back(roomJson);
        }
        
        // Create backup before saving
        std::string backupPath = "assets/data/scenes.json.bak";
        std::filesystem::copy_file("assets/data/scenes.json", backupPath, 
                                   std::filesystem::copy_options::overwrite_existing);
        
        std::ofstream file("assets/data/scenes.json");
        if (file.is_open()) {
            file << data.dump(2);
            file.close();
            LOG_INFO("Saved scenes.json (" + std::to_string(roomsToSave.size()) + " rooms)");
        } else {
            LOG_ERROR("Failed to open scenes.json for writing!");
            return;
        }
    }
    
    // Save dialogs
    if (!dialogs.empty()) {
        json data;
        data["dialogs"] = dialogs;  // nlohmann/json använder NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
        
        std::string backupPath = "assets/data/dialogs.json.bak";
        if (std::filesystem::exists("assets/data/dialogs.json")) {
            std::filesystem::copy_file("assets/data/dialogs.json", backupPath, 
                                       std::filesystem::copy_options::overwrite_existing);
        }
        
        std::ofstream file("assets/data/dialogs.json");
        if (file.is_open()) {
            file << data.dump(2);
            file.close();
            LOG_INFO("Saved dialogs.json (" + std::to_string(dialogs.size()) + " dialogs)");
        } else {
            LOG_ERROR("Failed to open dialogs.json for writing!");
        }
    }
    
    // Save quests
    if (!quests.empty()) {
        json data;
        data["quests"] = quests;
        
        std::string backupPath = "assets/data/quests.json.bak";
        if (std::filesystem::exists("assets/data/quests.json")) {
            std::filesystem::copy_file("assets/data/quests.json", backupPath, 
                                       std::filesystem::copy_options::overwrite_existing);
        }
        
        std::ofstream file("assets/data/quests.json");
        if (file.is_open()) {
            file << data.dump(2);
            file.close();
            LOG_INFO("Saved quests.json (" + std::to_string(quests.size()) + " quests)");
        } else {
            LOG_ERROR("Failed to open quests.json for writing!");
        }
    }
    
    // Save items
    if (!items.empty()) {
        json data;
        data["items"] = items;
        
        std::string backupPath = "assets/data/items.json.bak";
        if (std::filesystem::exists("assets/data/items.json")) {
            std::filesystem::copy_file("assets/data/items.json", backupPath, 
                                       std::filesystem::copy_options::overwrite_existing);
        }
        
        std::ofstream file("assets/data/items.json");
        if (file.is_open()) {
            file << data.dump(2);
            file.close();
            LOG_INFO("Saved items.json (" + std::to_string(items.size()) + " items)");
        } else {
            LOG_ERROR("Failed to open items.json for writing!");
        }
    }
    
    // Save NPCs
    if (!npcs.empty()) {
        json data;
        data["npcs"] = npcs;
        
        std::string backupPath = "assets/data/npcs.json.bak";
        if (std::filesystem::exists("assets/data/npcs.json")) {
            std::filesystem::copy_file("assets/data/npcs.json", backupPath, 
                                       std::filesystem::copy_options::overwrite_existing);
        }
        
        std::ofstream file("assets/data/npcs.json");
        if (file.is_open()) {
            file << data.dump(2);
            file.close();
            LOG_INFO("Saved npcs.json (" + std::to_string(npcs.size()) + " npcs)");
        } else {
            LOG_ERROR("Failed to open npcs.json for writing!");
        }
    }
    
    // Clear dirty flag after successful save
    isDirty = false;
    
    statusMessage = "Saved successfully!";
    statusTimer = 3.0f;
}

void EditorContext::syncScenesToRoomData() {
    LOG_INFO("EditorContext::syncScenesToRoomData() called");
    
    if (!m_editorState) {
        LOG_ERROR("EditorContext::syncScenesToRoomData: m_editorState is null!");
        return;
    }
    
    // Get WorldManager and sync
    auto* worldMgr = m_editorState->getWorldManager();
    if (!worldMgr) {
        LOG_ERROR("EditorContext::syncScenesToRoomData: worldMgr is null!");
        return;
    }
    
    LOG_INFO("EditorContext: Calling WorldManager::syncScenesToRoomData()");
    worldMgr->syncScenesToRoomData();
    LOG_INFO("EditorContext::syncScenesToRoomData() completed");
}

