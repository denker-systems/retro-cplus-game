/**
 * @file Room.h
 * @brief Rum med bakgrund, walk area och interaktiva hotspots
 */
#pragma once

#include <SDL.h>
#include <string>
#include <vector>
#include <memory>

class NPC;

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
 * @brief Interaktivt område i ett rum
 */
struct Hotspot {
    std::string id;         // Unik identifierare
    std::string name;       // Visningsnamn
    SDL_Rect rect;          // Klickbart område
    HotspotType type;       // Typ av hotspot
    std::string targetRoom; // För exits - vilken rum att gå till
    std::string dialogId;   // För NPCs - dialog att starta
    bool active = true;     // Om hotspot är synlig/interaktiv
};

/**
 * @brief Walk area - rektangulär för enkelhet
 */
struct WalkArea {
    int minX, maxX;
    int minY, maxY;
};

/**
 * @brief Ett spelrum med bakgrund och hotspots
 */
class Room {
public:
    Room(const std::string& id, const std::string& name);
    ~Room();

    void render(SDL_Renderer* renderer);
    
    /** @brief Ladda bakgrundstextur */
    bool loadBackground(SDL_Renderer* renderer, const std::string& path);
    
    /** @brief Lägg till hotspot */
    void addHotspot(const std::string& id, const std::string& name, 
                    int x, int y, int w, int h, HotspotType type,
                    const std::string& dialogId = "");
    
    /** @brief Lägg till exit-hotspot med målrum */
    void addExit(const std::string& id, const std::string& name,
                 int x, int y, int w, int h, const std::string& targetRoom);
    
    /** @brief Sätt walk area */
    void setWalkArea(int minX, int maxX, int minY, int maxY);
    
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
    void addNPC(std::unique_ptr<NPC> npc);
    NPC* getNPC(const std::string& id);
    const std::vector<std::unique_ptr<NPC>>& getNPCs() const { return m_npcs; }
    void updateNPCs(float deltaTime);
    void renderNPCs(SDL_Renderer* renderer);

private:
    std::string m_id;
    std::string m_name;
    SDL_Texture* m_background = nullptr;
    std::vector<Hotspot> m_hotspots;
    std::vector<std::unique_ptr<NPC>> m_npcs;
    WalkArea m_walkArea = {0, 640, 260, 350};
    
    void renderDebugInfo(SDL_Renderer* renderer);
};
