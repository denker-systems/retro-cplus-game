/**
 * @file TiledIntegration.cpp
 * @brief Implementation av TiledIntegration
 */
#include "TiledIntegration.h"
#include "engine/data/TiledImporter.h"
#include "engine/data/DataLoader.h"
#include <filesystem>

bool TiledIntegration::importFromTiled(std::string& outMessage) {
    namespace fs = std::filesystem;
    
    std::string tiledFolder = "assets/tiled/";
    if (!fs::exists(tiledFolder)) {
        outMessage = "Tiled folder not found: " + tiledFolder;
        return false;
    }
    
    int imported = 0;
    for (const auto& entry : fs::directory_iterator(tiledFolder)) {
        if (entry.path().extension() == ".json") {
            RoomData roomData = TiledImporter::importRoom(entry.path().string());
            if (!roomData.id.empty()) {
                imported++;
            }
        }
    }
    
    if (imported > 0) {
        DataLoader::instance().loadAll("assets/data/");
        outMessage = "Imported " + std::to_string(imported) + " rooms from Tiled";
        return true;
    } else {
        outMessage = "No Tiled files found in " + tiledFolder;
        return false;
    }
}

bool TiledIntegration::exportRoomToTiled(const std::string& roomId, std::string& outMessage) {
    auto& rooms = DataLoader::instance().getRooms();
    for (const auto& room : rooms) {
        if (room.id == roomId) {
            std::string tiledPath = "assets/tiled/" + roomId + ".json";
            if (TiledImporter::exportRoom(room, tiledPath)) {
                outMessage = "Exported " + roomId + " to Tiled";
                return true;
            } else {
                outMessage = "Failed to export " + roomId;
                return false;
            }
        }
    }
    
    outMessage = "Room not found: " + roomId;
    return false;
}

bool TiledIntegration::exportAllToTiled(std::string& outMessage) {
    namespace fs = std::filesystem;
    std::string tiledFolder = "assets/tiled/";
    
    if (!fs::exists(tiledFolder)) {
        fs::create_directories(tiledFolder);
    }
    
    auto& rooms = DataLoader::instance().getRooms();
    int exported = 0;
    
    for (const auto& room : rooms) {
        std::string tiledPath = tiledFolder + room.id + ".json";
        if (TiledImporter::exportRoom(room, tiledPath)) {
            exported++;
        }
    }
    
    outMessage = "Exported " + std::to_string(exported) + " rooms to Tiled";
    return exported > 0;
}
