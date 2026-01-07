/**
 * @file Room.h
 * @brief Rum med bakgrund, walk area och interaktiva hotspots
 */
#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>

#include "engine/Hotspot.h"

namespace engine { namespace actors {
    class NPC;
}}

/**
 * @brief Ett spelrum med bakgrund och hotspots
 */
class Room {
public:
    Room(const std::string& id, const std::string& name);
    ~Room();

    void render(SDL_Renderer* renderer);
    
    /** @brief Ladda bakgrundstextur (legacy) */
    bool loadBackground(SDL_Renderer* renderer, const std::string& path);
    
    /** @brief Ladda layer från LayerData */
    bool loadLayer(SDL_Renderer* renderer, const std::string& imagePath, int zIndex, 
                   float parallaxX = 1.0f, float parallaxY = 1.0f, float opacity = 1.0f);
    
    /** @brief Rendera layers baserat på zIndex och spelarposition */
    void renderLayers(SDL_Renderer* renderer, int playerY, bool renderBehind);
    
    /** @brief Lägg till hotspot */
    void addHotspot(const std::string& id, const std::string& name, 
                    int x, int y, int w, int h, HotspotType type,
                    const std::string& dialogId = "",
                    const std::string& examineText = "",
                    const std::vector<std::string>& funnyFails = {});
    
    /** @brief Lägg till exit-hotspot med målrum */
    void addExit(const std::string& id, const std::string& name,
                 int x, int y, int w, int h, const std::string& targetRoom);
    
    /** @brief Sätt walk area med depth scale */
    void setWalkArea(int minX, int maxX, int minY, int maxY, float scaleTop = 0.5f, float scaleBottom = 1.0f);
    
    /** @brief Sätt player spawn position */
    void setPlayerSpawn(float x, float y) { m_playerSpawnX = x; m_playerSpawnY = y; }
    
    /** @brief Hämta player spawn position */
    void getPlayerSpawn(float& x, float& y) const { x = m_playerSpawnX; y = m_playerSpawnY; }
    
    /** @brief Kolla om position är inom walk area */
    bool isInWalkArea(float x, float y) const;
    
    /** @brief Clamp position till walk area */
    void clampToWalkArea(float& x, float& y, int width, int height) const;
    
    /** @brief Hitta hotspot vid position */
    Hotspot* getHotspotAt(int x, int y);
    
    const std::string& getId() const { return m_id; }
    const std::string& getName() const { return m_name; }
    const WalkArea& getWalkArea() const { return m_walkArea; }
    const std::vector<Hotspot>& getHotspots() const { return m_hotspots; }
    
    /** @brief NPC-hantering */
    void addNPC(std::unique_ptr<engine::actors::NPC> npc);
    engine::actors::NPC* getNPC(const std::string& id);
    const std::vector<std::unique_ptr<engine::actors::NPC>>& getNPCs() const { return m_npcs; }
    void updateNPCs(float deltaTime);
    void renderNPCs(SDL_Renderer* renderer);

private:
    std::string m_id;
    std::string m_name;
    SDL_Texture* m_background = nullptr;  // Legacy
    std::vector<RoomLayer> m_layers;      // Multi-layer rendering
    std::vector<Hotspot> m_hotspots;
    std::vector<std::unique_ptr<engine::actors::NPC>> m_npcs;
    WalkArea m_walkArea = {0, 640, 260, 350};
    float m_playerSpawnX = 320.0f;
    float m_playerSpawnY = 300.0f;
    
    void renderDebugInfo(SDL_Renderer* renderer);
};
