/**
 * @file TiledImporter.h
 * @brief Importerar rum-data från Tiled JSON-filer
 * 
 * Tiled (mapeditor.org) är en gratis kartredigerare.
 * Vi använder den för att rita:
 * - Hotspots (rektanglar med custom properties)
 * - Walk areas (polygoner)
 * - Exits (rektanglar med targetRoom property)
 * 
 * Workflow:
 * 1. Skapa ny karta i Tiled (640x400 pixels)
 * 2. Lägg till bakgrundsbild som Image Layer
 * 3. Skapa Object Layer "hotspots"
 * 4. Rita rektanglar och sätt properties (id, name, type, dialogId, targetRoom)
 * 5. Exportera som JSON
 * 6. Kör TiledImporter::importRoom()
 */
#pragma once

#include "GameData.h"
#include <string>
#include <vector>

/**
 * @brief Importerar Tiled-kartor till RoomData
 */
class TiledImporter {
public:
    /**
     * @brief Importera en Tiled JSON-fil till RoomData
     * @param tiledJsonPath Sökväg till .json-filen från Tiled
     * @return RoomData med hotspots från Tiled
     */
    static RoomData importRoom(const std::string& tiledJsonPath);
    
    /**
     * @brief Importera alla Tiled-filer från en mapp
     * @param tiledFolder Mapp med .json-filer
     * @return Vector med RoomData
     */
    static std::vector<RoomData> importAllRooms(const std::string& tiledFolder);
    
    /**
     * @brief Exportera RoomData till Tiled-kompatibel JSON
     * @param room Rum att exportera
     * @param outputPath Sökväg för output
     */
    static bool exportRoom(const RoomData& room, const std::string& outputPath);

private:
    static HotspotData parseHotspot(const nlohmann::json& obj);
    static WalkAreaData parseWalkArea(const nlohmann::json& obj);
};
