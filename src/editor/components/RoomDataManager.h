/**
 * @file RoomDataManager.h
 * @brief Hanterar laddning och sparning av room data
 */
#pragma once

#include "engine/data/GameData.h"
#include <SDL.h>
#include <string>

/**
 * @brief Manager för room data operations
 */
class RoomDataManager {
public:
    /** @brief Ladda room data för redigering */
    static bool loadRoomForEditing(const std::string& roomId, RoomData& outData);
    
    /** @brief Spara room ändringar till JSON */
    static bool saveRoomChanges(const RoomData& roomData, std::string& outMessage);
    
    /** @brief Ladda room preview texture */
    static SDL_Texture* loadRoomPreview(SDL_Renderer* renderer, const RoomData& roomData);
};
