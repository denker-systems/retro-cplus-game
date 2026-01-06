/**
 * @file WalkArea.cpp
 * @brief Implementation of WalkArea
 */
#include "WalkArea.h"
#include <algorithm>

namespace engine {

WalkArea::WalkArea()
    : InteractiveActor("WalkArea") {
}

WalkArea::WalkArea(const std::string& name)
    : InteractiveActor(name) {
}

bool WalkArea::contains(float x, float y) const {
    return x >= m_bounds.minX && x <= m_bounds.maxX &&
           y >= m_bounds.minY && y <= m_bounds.maxY;
}

void WalkArea::clampPoint(float& x, float& y) const {
    x = std::max(static_cast<float>(m_bounds.minX), std::min(static_cast<float>(m_bounds.maxX), x));
    y = std::max(static_cast<float>(m_bounds.minY), std::min(static_cast<float>(m_bounds.maxY), y));
}

void WalkArea::render(SDL_Renderer* renderer) {
    if (!isVisible() || !renderer) return;
    
    // Draw walkable area as cyan rectangle
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 50);
    SDL_Rect fillRect = {
        m_bounds.minX,
        m_bounds.minY,
        m_bounds.maxX - m_bounds.minX,
        m_bounds.maxY - m_bounds.minY
    };
    SDL_RenderFillRect(renderer, &fillRect);
    
    // Draw border
    SDL_SetRenderDrawColor(renderer, 0, 255, 255, 200);
    SDL_RenderDrawRect(renderer, &fillRect);
}

} // namespace engine
