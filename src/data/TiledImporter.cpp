/**
 * @file TiledImporter.cpp
 * @brief Implementation av Tiled JSON-import
 */
#include "TiledImporter.h"
#include "../utils/Logger.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

using json = nlohmann::json;
namespace fs = std::filesystem;

RoomData TiledImporter::importRoom(const std::string& tiledJsonPath) {
    RoomData room;
    
    std::ifstream file(tiledJsonPath);
    if (!file.is_open()) {
        LOG_ERROR("Could not open Tiled file: " + tiledJsonPath);
        return room;
    }
    
    try {
        json data = json::parse(file);
        
        // Extrahera rum-id från filnamn
        fs::path path(tiledJsonPath);
        room.id = path.stem().string();
        room.name = room.id;  // Kan överskrivas av property
        
        // Karta-storlek
        int mapWidth = data.value("width", 640);
        int mapHeight = data.value("height", 400);
        int tileWidth = data.value("tilewidth", 1);
        int tileHeight = data.value("tileheight", 1);
        
        // Default walk area (hela kartan)
        room.walkArea.minX = 0;
        room.walkArea.maxX = mapWidth * tileWidth;
        room.walkArea.minY = mapHeight * tileHeight / 2;
        room.walkArea.maxY = mapHeight * tileHeight;
        
        // Gå igenom layers
        if (data.contains("layers")) {
            for (const auto& layer : data["layers"]) {
                std::string layerType = layer.value("type", "");
                std::string layerName = layer.value("name", "");
                
                // Image layer = bakgrund
                if (layerType == "imagelayer") {
                    if (layer.contains("image")) {
                        room.background = layer["image"].get<std::string>();
                        // Ta bort eventuell sökväg, behåll bara filnamn
                        fs::path bgPath(room.background);
                        room.background = "assets/backgrounds/" + bgPath.filename().string();
                    }
                }
                
                // Object layer = hotspots, exits, walk areas
                if (layerType == "objectgroup") {
                    if (layer.contains("objects")) {
                        for (const auto& obj : layer["objects"]) {
                            // Kolla object type via "type" eller custom property
                            std::string objType = obj.value("type", "");
                            std::string objClass = obj.value("class", "");
                            
                            // Prioritera "class" över "type" (Tiled 1.9+)
                            if (!objClass.empty()) {
                                objType = objClass;
                            }
                            
                            // Walk area
                            if (layerName == "walkarea" || objType == "walkarea") {
                                auto wa = parseWalkArea(obj);
                                room.walkArea = wa;
                            }
                            // Hotspot (default)
                            else {
                                auto hs = parseHotspot(obj);
                                room.hotspots.push_back(hs);
                            }
                        }
                    }
                }
            }
        }
        
        // Properties på root-nivå
        if (data.contains("properties")) {
            for (const auto& prop : data["properties"]) {
                std::string propName = prop.value("name", "");
                if (propName == "name" || propName == "roomName") {
                    room.name = prop.value("value", room.name);
                }
                else if (propName == "music") {
                    // Framtida: room.music = prop["value"];
                }
            }
        }
        
        LOG_INFO("Imported Tiled room: " + room.id + " with " + 
                 std::to_string(room.hotspots.size()) + " hotspots");
        
    } catch (const json::exception& e) {
        LOG_ERROR("JSON error parsing Tiled file: " + std::string(e.what()));
    }
    
    return room;
}

HotspotData TiledImporter::parseHotspot(const nlohmann::json& obj) {
    HotspotData hs;
    
    // Grundläggande geometri
    hs.x = obj.value("x", 0);
    hs.y = obj.value("y", 0);
    hs.w = obj.value("width", 32);
    hs.h = obj.value("height", 32);
    
    // Namn
    hs.name = obj.value("name", "unnamed");
    hs.id = hs.name;  // Default: samma som name
    
    // Type (exit, npc, item, examine)
    std::string typeStr = obj.value("type", "examine");
    std::string classStr = obj.value("class", "");
    if (!classStr.empty()) typeStr = classStr;
    
    if (typeStr == "exit") hs.type = "exit";
    else if (typeStr == "npc") hs.type = "npc";
    else if (typeStr == "item") hs.type = "item";
    else hs.type = "examine";
    
    // Custom properties
    if (obj.contains("properties")) {
        for (const auto& prop : obj["properties"]) {
            std::string propName = prop.value("name", "");
            
            if (propName == "id") {
                hs.id = prop.value("value", hs.id);
            }
            else if (propName == "targetRoom") {
                hs.targetRoom = prop.value("value", "");
            }
            else if (propName == "dialogId") {
                hs.dialogId = prop.value("value", "");
            }
            else if (propName == "examineText") {
                hs.examineText = prop.value("value", "");
            }
            else if (propName == "funnyFail" || propName == "funnyFails") {
                // Kan vara string eller array
                auto& val = prop["value"];
                if (val.is_string()) {
                    hs.funnyFails.push_back(val.get<std::string>());
                } else if (val.is_array()) {
                    for (const auto& item : val) {
                        if (item.is_string()) {
                            hs.funnyFails.push_back(item.get<std::string>());
                        }
                    }
                }
            }
        }
    }
    
    return hs;
}

WalkAreaData TiledImporter::parseWalkArea(const nlohmann::json& obj) {
    WalkAreaData wa;
    
    // Enkel rektangel-walk area
    int x = obj.value("x", 0);
    int y = obj.value("y", 0);
    int w = obj.value("width", 640);
    int h = obj.value("height", 200);
    
    wa.minX = x;
    wa.maxX = x + w;
    wa.minY = y;
    wa.maxY = y + h;
    
    return wa;
}

std::vector<RoomData> TiledImporter::importAllRooms(const std::string& tiledFolder) {
    std::vector<RoomData> rooms;
    
    try {
        for (const auto& entry : fs::directory_iterator(tiledFolder)) {
            if (entry.path().extension() == ".json") {
                auto room = importRoom(entry.path().string());
                if (!room.id.empty()) {
                    rooms.push_back(room);
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        LOG_ERROR("Filesystem error: " + std::string(e.what()));
    }
    
    LOG_INFO("Imported " + std::to_string(rooms.size()) + " rooms from Tiled");
    return rooms;
}

bool TiledImporter::exportRoom(const RoomData& room, const std::string& outputPath) {
    json data;
    
    // Tiled format
    data["width"] = 640;
    data["height"] = 400;
    data["tilewidth"] = 1;
    data["tileheight"] = 1;
    data["orientation"] = "orthogonal";
    data["renderorder"] = "right-down";
    data["type"] = "map";
    data["version"] = "1.10";
    data["tiledversion"] = "1.10.0";
    
    // Properties
    data["properties"] = json::array();
    json nameProp;
    nameProp["name"] = "roomName";
    nameProp["type"] = "string";
    nameProp["value"] = room.name;
    data["properties"].push_back(nameProp);
    
    // Layers
    data["layers"] = json::array();
    
    // Image layer (bakgrund)
    json imageLayer;
    imageLayer["type"] = "imagelayer";
    imageLayer["name"] = "background";
    imageLayer["image"] = room.background;
    imageLayer["x"] = 0;
    imageLayer["y"] = 0;
    imageLayer["opacity"] = 1;
    imageLayer["visible"] = true;
    data["layers"].push_back(imageLayer);
    
    // Object layer (hotspots)
    json objectLayer;
    objectLayer["type"] = "objectgroup";
    objectLayer["name"] = "hotspots";
    objectLayer["objects"] = json::array();
    
    int objId = 1;
    for (const auto& hs : room.hotspots) {
        json obj;
        obj["id"] = objId++;
        obj["name"] = hs.name;
        obj["type"] = hs.type;
        obj["x"] = hs.x;
        obj["y"] = hs.y;
        obj["width"] = hs.w;
        obj["height"] = hs.h;
        obj["visible"] = true;
        
        obj["properties"] = json::array();
        
        if (!hs.id.empty() && hs.id != hs.name) {
            json p; p["name"] = "id"; p["type"] = "string"; p["value"] = hs.id;
            obj["properties"].push_back(p);
        }
        if (!hs.targetRoom.empty()) {
            json p; p["name"] = "targetRoom"; p["type"] = "string"; p["value"] = hs.targetRoom;
            obj["properties"].push_back(p);
        }
        if (!hs.dialogId.empty()) {
            json p; p["name"] = "dialogId"; p["type"] = "string"; p["value"] = hs.dialogId;
            obj["properties"].push_back(p);
        }
        if (!hs.examineText.empty()) {
            json p; p["name"] = "examineText"; p["type"] = "string"; p["value"] = hs.examineText;
            obj["properties"].push_back(p);
        }
        
        objectLayer["objects"].push_back(obj);
    }
    
    data["layers"].push_back(objectLayer);
    
    // Walk area layer
    json walkLayer;
    walkLayer["type"] = "objectgroup";
    walkLayer["name"] = "walkarea";
    walkLayer["objects"] = json::array();
    
    json waObj;
    waObj["id"] = objId++;
    waObj["name"] = "walkarea";
    waObj["type"] = "walkarea";
    waObj["x"] = room.walkArea.minX;
    waObj["y"] = room.walkArea.minY;
    waObj["width"] = room.walkArea.maxX - room.walkArea.minX;
    waObj["height"] = room.walkArea.maxY - room.walkArea.minY;
    waObj["visible"] = true;
    walkLayer["objects"].push_back(waObj);
    
    data["layers"].push_back(walkLayer);
    
    // Skriv till fil
    std::ofstream file(outputPath);
    if (!file.is_open()) {
        LOG_ERROR("Could not write Tiled file: " + outputPath);
        return false;
    }
    
    file << data.dump(2);
    LOG_INFO("Exported room to Tiled: " + outputPath);
    return true;
}
