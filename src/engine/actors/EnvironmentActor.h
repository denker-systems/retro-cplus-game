/**
 * @file EnvironmentActor.h
 * @brief Base class for environment actors (TileMaps, Parallax, Props)
 */
#pragma once

#include "engine/core/ActorObjectExtended.h"

namespace engine {

/**
 * @brief Base class for environment objects
 * 
 * Used for static/background elements like:
 * - Tile maps
 * - Parallax layers
 * - Static props
 * - Background decorations
 */
class EnvironmentActor : public ActorObjectExtended {
public:
    EnvironmentActor(const std::string& name);
    virtual ~EnvironmentActor() = default;
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
};

/**
 * @brief Tile map actor
 */
class TileMapActor : public EnvironmentActor {
public:
    TileMapActor(const std::string& name = "TileMap");
    virtual ~TileMapActor() = default;
    
    // Grid dimensions
    void setGridSize(int width, int height);
    int getGridWidth() const { return m_gridWidth; }
    int getGridHeight() const { return m_gridHeight; }
    
    void setTileSize(int size) { m_tileSize = size; }
    int getTileSize() const { return m_tileSize; }
    
protected:
    int m_gridWidth = 0;
    int m_gridHeight = 0;
    int m_tileSize = 32;
};

/**
 * @brief Parallax layer actor
 */
class ParallaxActor : public EnvironmentActor {
public:
    ParallaxActor(const std::string& name = "Parallax");
    virtual ~ParallaxActor() = default;
    
    void setScrollFactor(float factorX, float factorY);
    void setScrollFactor(const Vec2& factor);
    Vec2 getScrollFactor() const { return m_scrollFactor; }
    
    void update(float deltaTime) override;
    
protected:
    Vec2 m_scrollFactor{0.5f, 0.5f};
};

/**
 * @brief Static prop actor
 */
class PropActor : public EnvironmentActor {
public:
    PropActor(const std::string& name = "Prop");
    virtual ~PropActor() = default;
};

} // namespace engine
