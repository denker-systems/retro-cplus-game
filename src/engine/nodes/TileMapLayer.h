/**
 * @file TileMapLayer.h
 * @brief Grid-based tilemap rendering
 */
#pragma once

#include "core/Node2D.h"
#include <vector>
#include <SDL.h>

namespace engine {

/**
 * @brief Tilemap layer for grid-based levels
 * 
 * Features:
 * - Grid of tile IDs
 * - Tileset texture with tile size
 * - Efficient rendering (only visible tiles)
 * - Collision tile marking
 */
class TileMapLayer : public Node2D {
public:
    TileMapLayer();
    explicit TileMapLayer(const std::string& name);
    TileMapLayer(const std::string& name, int width, int height, int tileSize);
    virtual ~TileMapLayer() = default;
    
    void render(SDL_Renderer* renderer) override;
    
    // ═══════════════════════════════════════════════════════════════════
    // TILESET
    // ═══════════════════════════════════════════════════════════════════
    
    SDL_Texture* getTileset() const { return m_tileset; }
    void setTileset(SDL_Texture* tileset) { m_tileset = tileset; }
    
    int getTileSize() const { return m_tileSize; }
    void setTileSize(int size) { m_tileSize = size; }
    
    /** @brief Tiles per row in tileset texture */
    int getTilesetColumns() const { return m_tilesetColumns; }
    void setTilesetColumns(int cols) { m_tilesetColumns = cols; }
    
    // ═══════════════════════════════════════════════════════════════════
    // GRID
    // ═══════════════════════════════════════════════════════════════════
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    /** @brief Resize grid (clears existing data) */
    void resize(int width, int height);
    
    /** @brief Get tile ID at grid position */
    int getTile(int x, int y) const;
    
    /** @brief Set tile ID at grid position */
    void setTile(int x, int y, int tileId);
    
    /** @brief Fill entire grid with tile ID */
    void fill(int tileId);
    
    /** @brief Clear grid (set all to 0) */
    void clear() { fill(0); }
    
    // ═══════════════════════════════════════════════════════════════════
    // COLLISION
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Check if tile is solid */
    bool isSolid(int x, int y) const;
    
    /** @brief Mark tile as solid/non-solid */
    void setSolid(int x, int y, bool solid);
    
    /** @brief Get all solid tiles in area */
    std::vector<SDL_Rect> getSolidTiles(const SDL_Rect& area) const;
    
    // ═══════════════════════════════════════════════════════════════════
    // WORLD CONVERSION
    // ═══════════════════════════════════════════════════════════════════
    
    /** @brief Convert world position to grid coordinates */
    void worldToGrid(float worldX, float worldY, int& gridX, int& gridY) const;
    
    /** @brief Convert grid coordinates to world position */
    void gridToWorld(int gridX, int gridY, float& worldX, float& worldY) const;
    
protected:
    SDL_Rect getTileSourceRect(int tileId) const;
    bool isInBounds(int x, int y) const;
    
    SDL_Texture* m_tileset = nullptr;
    int m_tileSize = 16;
    int m_tilesetColumns = 16;
    
    int m_width = 0;
    int m_height = 0;
    std::vector<int> m_tiles;        // Tile IDs (0 = empty)
    std::vector<bool> m_solidTiles;  // Collision flags
};

} // namespace engine
