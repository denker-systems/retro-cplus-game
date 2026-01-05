/**
 * @file Room.cpp
 * @brief Implementation av rum och hotspots
 */
#include "Room.h"

Room::Room(const std::string& name) : m_name(name) {
    // TODO: Ladda hotspots från JSON istället
    // Temporära test-hotspots
    addHotspot(80, 280, 40, 50, "Bartender", 1);   // NPC
    addHotspot(400, 320, 30, 30, "Chest", 2);      // Container
    addHotspot(550, 300, 20, 20, "Key", 3);        // Item
}

void Room::render(SDL_Renderer* renderer) {
    // TODO: Ersätt med bakgrundstextur
    
    // Walk area (där spelaren kan gå)
    SDL_Rect walkArea = {0, 260, 640, 115};
    SDL_SetRenderDrawColor(renderer, 40, 35, 30, 255);
    SDL_RenderFillRect(renderer, &walkArea);

    // Rita interaktiva hotspots
    for (const auto& hotspot : m_hotspots) {
        // Hotspot bakgrund
        SDL_SetRenderDrawColor(renderer, 0, 100, 0, 255);
        SDL_RenderFillRect(renderer, &hotspot.rect);
        
        // Highlight-ram (visar att det är interaktivt)
        SDL_SetRenderDrawColor(renderer, 0, 200, 200, 255);
        SDL_RenderDrawRect(renderer, &hotspot.rect);
    }

    // UI-bar längst ner på skärmen
    SDL_Rect uiBar = {0, 375, 640, 25};
    SDL_SetRenderDrawColor(renderer, 0, 0, 40, 255);
    SDL_RenderFillRect(renderer, &uiBar);

    // Separator-linje mellan spel och UI
    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_RenderDrawLine(renderer, 0, 375, 640, 375);
}

void Room::addHotspot(int x, int y, int w, int h, const std::string& name, int type) {
    Hotspot hs;
    hs.rect = {x, y, w, h};
    hs.name = name;
    hs.type = type;
    m_hotspots.push_back(hs);
}
