/**
 * @file EnvironmentActor.cpp
 * @brief Implementation of environment actors
 */
#include "EnvironmentActor.h"

namespace engine {

// ============================================================================
// EnvironmentActor
// ============================================================================

EnvironmentActor::EnvironmentActor(const std::string& name)
    : ActorObjectExtended(name)
{
}

void EnvironmentActor::update(float deltaTime) {
    ActorObjectExtended::update(deltaTime);
}

void EnvironmentActor::render(SDL_Renderer* renderer) {
    ActorObjectExtended::render(renderer);
}

// ============================================================================
// TileMapActor
// ============================================================================

TileMapActor::TileMapActor(const std::string& name)
    : EnvironmentActor(name)
{
}

void TileMapActor::setGridSize(int width, int height) {
    m_gridWidth = width;
    m_gridHeight = height;
}

// ============================================================================
// ParallaxActor
// ============================================================================

ParallaxActor::ParallaxActor(const std::string& name)
    : EnvironmentActor(name)
{
}

void ParallaxActor::setScrollFactor(float factorX, float factorY) {
    m_scrollFactor.x = factorX;
    m_scrollFactor.y = factorY;
}

void ParallaxActor::setScrollFactor(const Vec2& factor) {
    m_scrollFactor = factor;
}

void ParallaxActor::update(float deltaTime) {
    EnvironmentActor::update(deltaTime);
    // TODO: Update position based on camera and scroll factor
}

// ============================================================================
// PropActor
// ============================================================================

PropActor::PropActor(const std::string& name)
    : EnvironmentActor(name)
{
}

} // namespace engine
