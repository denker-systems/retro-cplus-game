/**
 * @file Marker.cpp
 * @brief Implementation of Marker
 */
#include "Marker.h"

namespace engine {

Marker::Marker()
    : InteractiveActor("Marker")
    , m_markerType(MarkerType::Other)
{
}

Marker::Marker(const std::string& name)
    : InteractiveActor(name)
    , m_markerType(MarkerType::Other)
{
}

Marker::Marker(const std::string& name, MarkerType type)
    : InteractiveActor(name)
    , m_markerType(type) {
}

SDL_Color Marker::getColor() const {
    switch (m_markerType) {
        case MarkerType::PlayerSpawn:  return {255, 0, 255, 255};  // Magenta
        case MarkerType::NPCSpawn:     return {255, 165, 0, 255};  // Orange
        case MarkerType::Checkpoint:   return {0, 255, 0, 255};    // Green
        case MarkerType::Waypoint:     return {255, 255, 0, 255};  // Yellow
        case MarkerType::Camera:       return {0, 191, 255, 255};  // Deep sky blue
        default:                       return {128, 128, 128, 255}; // Gray
    }
}

const char* Marker::getLabel() const {
    switch (m_markerType) {
        case MarkerType::PlayerSpawn:  return "P";
        case MarkerType::NPCSpawn:     return "N";
        case MarkerType::Checkpoint:   return "C";
        case MarkerType::Waypoint:     return "W";
        case MarkerType::Camera:       return "CAM";
        default:                       return "?";
    }
}

void Marker::render(SDL_Renderer* renderer) {
    if (!isVisible() || !renderer) return;
    
    Vec2 pos = getGlobalPosition();
    SDL_Color color = getColor();
    
    // Draw circle
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 200);
    
    // Simple filled circle (approximate with rect for now)
    int radius = 8;
    SDL_Rect rect = {
        static_cast<int>(pos.x - radius),
        static_cast<int>(pos.y - radius),
        radius * 2,
        radius * 2
    };
    SDL_RenderFillRect(renderer, &rect);
    
    // Draw border
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &rect);
    
    // TODO: Render label text using FontManager if available
}

} // namespace engine
