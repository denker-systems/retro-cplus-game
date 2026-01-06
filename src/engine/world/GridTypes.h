/**
 * @file GridTypes.h
 * @brief Spatial grid types for World/Level/Room hierarchy
 * 
 * Enables Super Mario World-style level structure with:
 * - World map (grid of Levels)
 * - Level grid (grid of Rooms with varying sizes)
 * - Room layers (Z-sorted rendering)
 */
#pragma once

#include <string>
#include <vector>
#include <SDL.h>
#include <nlohmann/json.hpp>

namespace engine {

/**
 * @brief Position and size in parent's spatial grid
 * 
 * Used by World (for Levels), Level (for Rooms), and Room (for world position)
 */
struct GridPosition {
    int gridX = 0;              // X position in parent's grid
    int gridY = 0;              // Y position in parent's grid
    int pixelWidth = 640;       // Width in pixels (for rooms) or cells (for levels)
    int pixelHeight = 400;      // Height in pixels (for rooms) or cells (for levels)
    
    // Helper to check if a point is within this cell
    bool contains(int px, int py) const {
        return px >= 0 && px < pixelWidth && py >= 0 && py < pixelHeight;
    }
    
    // Get world-space position (requires parent grid cell size)
    SDL_Point getWorldPosition(int cellWidth = 640, int cellHeight = 400) const {
        return { gridX * cellWidth, gridY * cellHeight };
    }
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GridPosition,
    gridX, gridY, pixelWidth, pixelHeight)

/**
 * @brief Connection between grid cells (exits, doors, portals)
 * 
 * Maps to existing Hotspot exits but adds more flexibility
 */
struct GridConnection {
    std::string id;                     // Unique connection ID
    std::string targetId;               // Target cell ID (room/level)
    std::string exitPoint;              // "north", "south", "east", "west", or custom "door_xxx"
    std::string entryPoint;             // Spawn point ID in target
    std::string transitionType = "fade"; // "fade", "slide", "instant"
    
    // Optional: unlock conditions
    std::string requiredFlag;           // WorldState flag required
    std::string requiredItem;           // Inventory item required
    
    // Visual hint in editor
    int triggerX = 0, triggerY = 0, triggerW = 0, triggerH = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GridConnection,
    id, targetId, exitPoint, entryPoint, transitionType,
    requiredFlag, requiredItem, triggerX, triggerY, triggerW, triggerH)

/**
 * @brief Camera configuration for a room
 */
struct CameraConfig {
    float zoom = 1.0f;                  // Camera zoom level
    bool followPlayer = true;           // Auto-follow player
    float smoothing = 0.1f;             // Lerp factor for smooth following
    
    // Camera bounds (0,0,0,0 = no bounds, use room size)
    int boundsX = 0, boundsY = 0, boundsW = 0, boundsH = 0;
    
    // Offset from player when following
    int offsetX = 0;
    int offsetY = 0;
    
    // Shake parameters
    float shakeIntensity = 0.0f;
    float shakeDuration = 0.0f;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CameraConfig,
    zoom, followPlayer, smoothing, boundsX, boundsY, boundsW, boundsH,
    offsetX, offsetY, shakeIntensity, shakeDuration)

/**
 * @brief Parallax layer configuration
 * 
 * Extends existing RoomLayer with more parallax options
 */
struct ParallaxConfig {
    float scrollSpeedX = 1.0f;          // Horizontal scroll multiplier (0 = static, 1 = normal)
    float scrollSpeedY = 1.0f;          // Vertical scroll multiplier
    bool repeatX = false;               // Tile horizontally
    bool repeatY = false;               // Tile vertically
    int offsetX = 0;                    // Base offset
    int offsetY = 0;
};

/**
 * @brief Direction enum for grid navigation
 */
enum class GridDirection {
    North,      // Up (y-1)
    South,      // Down (y+1)
    East,       // Right (x+1)
    West,       // Left (x-1)
    None
};

/**
 * @brief Get opposite direction
 */
inline GridDirection getOppositeDirection(GridDirection dir) {
    switch (dir) {
        case GridDirection::North: return GridDirection::South;
        case GridDirection::South: return GridDirection::North;
        case GridDirection::East:  return GridDirection::West;
        case GridDirection::West:  return GridDirection::East;
        default: return GridDirection::None;
    }
}

/**
 * @brief Get direction name for serialization
 */
inline const char* getDirectionName(GridDirection dir) {
    switch (dir) {
        case GridDirection::North: return "north";
        case GridDirection::South: return "south";
        case GridDirection::East:  return "east";
        case GridDirection::West:  return "west";
        default: return "none";
    }
}

/**
 * @brief Parse direction from string
 */
inline GridDirection parseDirection(const std::string& str) {
    if (str == "north") return GridDirection::North;
    if (str == "south") return GridDirection::South;
    if (str == "east")  return GridDirection::East;
    if (str == "west")  return GridDirection::West;
    return GridDirection::None;
}

} // namespace engine
