/**
 * @file TileMapLayer.cpp
 * @brief Implementation of TileMapLayer
 */
#include "TileMapLayer.h"
#include "world/Camera2D.h"
#include <algorithm>

namespace engine {

TileMapLayer::TileMapLayer()
    : VisualActor("TileMapLayer")
    , m_width(0)
    , m_height(0)
    , m_tileSize(32)
{
}

TileMapLayer::TileMapLayer(const std::string& name)
    : VisualActor(name)
    , m_width(0)
    , m_height(0)
    , m_tileSize(32)
{
}

TileMapLayer::TileMapLayer(const std::string& name, int width, int height, int tileSize)
    : VisualActor(name), m_tileSize(tileSize) {
    resize(width, height);
}

// ═══════════════════════════════════════════════════════════════════════════
// GRID
// ═══════════════════════════════════════════════════════════════════════════

void TileMapLayer::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_tiles.resize(width * height, 0);
    m_solidTiles.resize(width * height, false);
}

int TileMapLayer::getTile(int x, int y) const {
    if (!isInBounds(x, y)) return 0;
    return m_tiles[y * m_width + x];
}

void TileMapLayer::setTile(int x, int y, int tileId) {
    if (!isInBounds(x, y)) return;
    m_tiles[y * m_width + x] = tileId;
}

void TileMapLayer::fill(int tileId) {
    std::fill(m_tiles.begin(), m_tiles.end(), tileId);
}

bool TileMapLayer::isInBounds(int x, int y) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height;
}

// ═══════════════════════════════════════════════════════════════════════════
// COLLISION
// ═══════════════════════════════════════════════════════════════════════════

bool TileMapLayer::isSolid(int x, int y) const {
    if (!isInBounds(x, y)) return false;
    return m_solidTiles[y * m_width + x];
}

void TileMapLayer::setSolid(int x, int y, bool solid) {
    if (!isInBounds(x, y)) return;
    m_solidTiles[y * m_width + x] = solid;
}

std::vector<SDL_Rect> TileMapLayer::getSolidTiles(const SDL_Rect& area) const {
    std::vector<SDL_Rect> result;
    
    // Convert area to grid coordinates
    int startX = area.x / m_tileSize;
    int startY = area.y / m_tileSize;
    int endX = (area.x + area.w) / m_tileSize + 1;
    int endY = (area.y + area.h) / m_tileSize + 1;
    
    // Clamp to grid bounds
    startX = std::max(0, startX);
    startY = std::max(0, startY);
    endX = std::min(m_width, endX);
    endY = std::min(m_height, endY);
    
    // Collect solid tiles
    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            if (isSolid(x, y)) {
                SDL_Rect tileRect;
                tileRect.x = x * m_tileSize;
                tileRect.y = y * m_tileSize;
                tileRect.w = m_tileSize;
                tileRect.h = m_tileSize;
                result.push_back(tileRect);
            }
        }
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
// WORLD CONVERSION
// ═══════════════════════════════════════════════════════════════════════════

void TileMapLayer::worldToGrid(float worldX, float worldY, int& gridX, int& gridY) const {
    Vec2 localPos = getGlobalPosition();
    gridX = static_cast<int>((worldX - localPos.x) / m_tileSize);
    gridY = static_cast<int>((worldY - localPos.y) / m_tileSize);
}

void TileMapLayer::gridToWorld(int gridX, int gridY, float& worldX, float& worldY) const {
    Vec2 localPos = getGlobalPosition();
    worldX = localPos.x + gridX * m_tileSize;
    worldY = localPos.y + gridY * m_tileSize;
}

// ═══════════════════════════════════════════════════════════════════════════
// RENDERING
// ═══════════════════════════════════════════════════════════════════════════

SDL_Rect TileMapLayer::getTileSourceRect(int tileId) const {
    if (tileId <= 0) return {0, 0, 0, 0};
    
    int tileIndex = tileId - 1;  // 0-based index
    int srcX = (tileIndex % m_tilesetColumns) * m_tileSize;
    int srcY = (tileIndex / m_tilesetColumns) * m_tileSize;
    
    return {srcX, srcY, m_tileSize, m_tileSize};
}

void TileMapLayer::render(SDL_Renderer* renderer) {
    if (!m_tileset || !renderer) {
        return;
    }
    
    Vec2 globalPos = getGlobalPosition();
    
    // TODO: Implement camera culling for efficiency
    // For now, render all tiles
    
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            int tileId = getTile(x, y);
            if (tileId <= 0) continue;  // Skip empty tiles
            
            SDL_Rect srcRect = getTileSourceRect(tileId);
            
            SDL_Rect dstRect;
            dstRect.x = static_cast<int>(globalPos.x) + x * m_tileSize;
            dstRect.y = static_cast<int>(globalPos.y) + y * m_tileSize;
            dstRect.w = m_tileSize;
            dstRect.h = m_tileSize;
            
            SDL_RenderCopy(renderer, m_tileset, &srcRect, &dstRect);
        }
    }
    
    // Render tiles
}

} // namespace engine
