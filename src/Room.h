/**
 * @file Room.h
 * @brief Rum med bakgrund och interaktiva hotspots
 */
#pragma once

#include <SDL.h>
#include <string>
#include <vector>

/**
 * @brief Interaktivt område i ett rum
 */
struct Hotspot {
    SDL_Rect rect;      // Klickbart område
    std::string name;   // Visningsnamn
    int type;           // 1=npc, 2=item, 3=exit, etc.
};

/**
 * @brief Ett spelrum med bakgrund och hotspots
 * 
 * Rum innehåller bakgrund, walk area och interaktiva
 * objekt som spelaren kan interagera med.
 */
class Room {
public:
    Room(const std::string& name);

    /** @brief Rita rummet och alla hotspots */
    void render(SDL_Renderer* renderer);
    
    /** @brief Lägg till interaktivt område */
    void addHotspot(int x, int y, int w, int h, const std::string& name, int type);

    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
    std::vector<Hotspot> m_hotspots;
    SDL_Color m_bgColor = {20, 20, 80, 255};  // Bakgrundsfärg
};
