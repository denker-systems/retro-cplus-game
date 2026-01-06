/**
 * @file Hotspot.h
 * @brief Hotspot och relaterade typer för interaktiva områden
 */
#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <cstdlib>

/**
 * @brief Typ av hotspot
 */
enum class HotspotType {
    None = 0,
    NPC,
    Item,
    Exit,
    Examine
};

/**
 * @brief Interaktivt område i en scene
 */
struct Hotspot {
    std::string id;         // Unik identifierare
    std::string name;       // Visningsnamn
    SDL_Rect rect;          // Klickbart område
    HotspotType type;       // Typ av hotspot
    std::string targetRoom; // För exits - vilken scene att gå till
    std::string dialogId;   // För NPCs - dialog att starta
    bool active = true;     // Om hotspot är synlig/interaktiv
    
    // Reaktivitet (LucasArts-inspirerat)
    std::string examineText;              // "Titta på" beskrivning
    std::vector<std::string> funnyFails;  // Roliga svar på dumma försök
    
    /** @brief Hämta slumpmässigt funny fail-svar */
    std::string getRandomFunnyFail() const {
        if (funnyFails.empty()) return "";
        return funnyFails[rand() % funnyFails.size()];
    }
};

/**
 * @brief Walk area - rektangulär för enkelhet
 */
struct WalkArea {
    int minX, maxX;
    int minY, maxY;
    float scaleTop = 0.5f;      // Skala vid minY (längre bort)
    float scaleBottom = 1.0f;   // Skala vid maxY (närmare)
};

/**
 * @brief Layer för multi-layer rendering
 */
struct RoomLayer {
    SDL_Texture* texture = nullptr;
    int zIndex = 0;             // Negativa = bakom spelare, positiva = framför
    float parallaxX = 1.0f;
    float parallaxY = 1.0f;
    float opacity = 1.0f;
};
