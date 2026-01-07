/**
 * @file InteractiveArea.cpp
 * @brief Implementation of InteractiveArea
 */
#include "InteractiveArea.h"

namespace engine {

InteractiveArea::InteractiveArea()
    : InteractiveActor("InteractiveArea") {
}

InteractiveArea::InteractiveArea(const std::string& name)
    : InteractiveActor(name) {
}

InteractiveArea::InteractiveArea(const std::string& name, float x, float y, float w, float h)
    : InteractiveActor(name)
    , m_width(w)
    , m_height(h)
{
    setPosition(Vec2(x, y));
}

bool InteractiveArea::contains(float x, float y) const {
    Vec2 pos = getGlobalPosition();
    return x >= pos.x && x <= pos.x + m_width &&
           y >= pos.y && y <= pos.y + m_height;
}

void InteractiveArea::render(SDL_Renderer* renderer) {
    if (!isVisible() || !renderer) return;
    
    Vec2 pos = getGlobalPosition();
    
    // Draw semi-transparent fill
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, m_enabled ? 100 : 50);
    SDL_Rect fillRect = {
        static_cast<int>(pos.x),
        static_cast<int>(pos.y),
        static_cast<int>(m_width),
        static_cast<int>(m_height)
    };
    SDL_RenderFillRect(renderer, &fillRect);
    
    // Draw border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &fillRect);
}

} // namespace engine
