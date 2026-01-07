/**
 * @file TiledIntegration.h
 * @brief Integration med Tiled Map Editor
 */
#pragma once

#include <string>

/**
 * @brief Hanterar import/export till Tiled
 */
class TiledIntegration {
public:
    /** @brief Importera rum från Tiled JSON-filer */
    static bool importFromTiled(std::string& outMessage);
    
    /** @brief Exportera ett rum till Tiled format */
    static bool exportRoomToTiled(const std::string& roomId, std::string& outMessage);
    
    /** @brief Exportera alla rum till Tiled */
    static bool exportAllToTiled(std::string& outMessage);
};
