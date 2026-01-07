/**
 * @file RoomDataManager.cpp
 * @brief Implementation av RoomDataManager
 */
#include "RoomDataManager.h"
#include "engine/data/DataLoader.h"
#include <SDL_image.h>
#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

bool RoomDataManager::loadRoomForEditing(const std::string& roomId, RoomData& outData) {
    auto& rooms = DataLoader::instance().getRooms();
    for (const auto& room : rooms) {
        if (room.id == roomId) {
            outData = room;
            return true;
        }
    }
    return false;
}

bool RoomDataManager::saveRoomChanges(const RoomData& roomData, std::string& outMessage) {
    namespace fs = std::filesystem;
    
    std::string roomsPath = "assets/data/scenes.json";
    std::ifstream inFile(roomsPath);
    if (!inFile.is_open()) {
        outMessage = "Error: Could not open scenes.json";
        return false;
    }
    
    json data;
    try {
        data = json::parse(inFile);
        inFile.close();
    } catch (const json::exception& e) {
        outMessage = "Error parsing JSON: " + std::string(e.what());
        return false;
    }
    
    bool found = false;
    if (data.contains("scenes") && data["scenes"].is_array()) {
        for (auto& room : data["scenes"]) {
            if (room["id"] == roomData.id) {
                // Uppdatera room data
                room["name"] = roomData.name;
                room["background"] = roomData.background;
                
                // Uppdatera layers
                room["layers"] = json::array();
                for (const auto& layer : roomData.layers) {
                    json layerJson;
                    layerJson["image"] = layer.image;
                    layerJson["zIndex"] = layer.zIndex;
                    layerJson["parallaxX"] = layer.parallaxX;
                    layerJson["parallaxY"] = layer.parallaxY;
                    layerJson["opacity"] = layer.opacity;
                    room["layers"].push_back(layerJson);
                }
                
                // Uppdatera walk area
                room["walkArea"]["minX"] = roomData.walkArea.minX;
                room["walkArea"]["maxX"] = roomData.walkArea.maxX;
                room["walkArea"]["minY"] = roomData.walkArea.minY;
                room["walkArea"]["maxY"] = roomData.walkArea.maxY;
                room["walkArea"]["scaleTop"] = roomData.walkArea.scaleTop;
                room["walkArea"]["scaleBottom"] = roomData.walkArea.scaleBottom;
                
                // Uppdatera player spawn
                room["playerSpawnX"] = roomData.playerSpawnX;
                room["playerSpawnY"] = roomData.playerSpawnY;
                
                // Uppdatera hotspots
                room["hotspots"] = json::array();
                for (const auto& hs : roomData.hotspots) {
                    json hsJson;
                    hsJson["id"] = hs.id;
                    hsJson["name"] = hs.name;
                    hsJson["type"] = hs.type;
                    hsJson["x"] = hs.x;
                    hsJson["y"] = hs.y;
                    hsJson["w"] = hs.w;
                    hsJson["h"] = hs.h;
                    
                    if (!hs.dialogId.empty()) hsJson["dialogId"] = hs.dialogId;
                    if (!hs.examineText.empty()) hsJson["examineText"] = hs.examineText;
                    if (!hs.targetScene.empty()) hsJson["targetRoom"] = hs.targetScene;
                    if (!hs.funnyFails.empty()) hsJson["funnyFails"] = hs.funnyFails;
                    
                    room["hotspots"].push_back(hsJson);
                }
                
                found = true;
                break;
            }
        }
    }
    
    if (!found) {
        outMessage = "Error: Room not found in JSON";
        return false;
    }
    
    // Skriv tillbaka
    std::ofstream outFile(roomsPath);
    if (!outFile.is_open()) {
        outMessage = "Error: Could not write to scenes.json";
        return false;
    }
    
    outFile << data.dump(2);
    outFile.close();
    
    outMessage = "Saved: " + roomData.name;
    return true;
}

SDL_Texture* RoomDataManager::loadRoomPreview(SDL_Renderer* renderer, const RoomData& roomData) {
    if (roomData.layers.empty()) {
        if (!roomData.background.empty()) {
            return IMG_LoadTexture(renderer, roomData.background.c_str());
        }
        return nullptr;
    }
    
    // Använd första layer som preview
    return IMG_LoadTexture(renderer, roomData.layers[0].image.c_str());
}
