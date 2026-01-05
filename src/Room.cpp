/**
 * @file Room.cpp
 * @brief Implementation av rum och hotspots
 */
#include "Room.h"
#include "entities/NPC.h"
#include <SDL_image.h>
#include <iostream>

Room::Room(const std::string& id, const std::string& name) 
    : m_id(id), m_name(name) {
    // Default walk area
    setWalkArea(0, 640, 260, 350);
}

Room::~Room() {
    if (m_background) {
        SDL_DestroyTexture(m_background);
        m_background = nullptr;
    }
}

bool Room::loadBackground(SDL_Renderer* renderer, const std::string& path) {
    if (m_background) {
        SDL_DestroyTexture(m_background);
    }
    
    m_background = IMG_LoadTexture(renderer, path.c_str());
    if (!m_background) {
        std::cerr << "Failed to load background: " << path << " - " << IMG_GetError() << std::endl;
        return false;
    }
    
    std::cout << "Loaded room background: " << path << std::endl;
    return true;
}

void Room::render(SDL_Renderer* renderer) {
    // Rita bakgrund
    if (m_background) {
        SDL_RenderCopy(renderer, m_background, nullptr, nullptr);
    } else {
        // Fallback - färgad bakgrund
        SDL_SetRenderDrawColor(renderer, 20, 20, 60, 255);
        SDL_Rect bg = {0, 0, 640, 260};
        SDL_RenderFillRect(renderer, &bg);
        
        // Walk area
        SDL_Rect walkRect = {m_walkArea.minX, m_walkArea.minY, 
                            m_walkArea.maxX - m_walkArea.minX,
                            m_walkArea.maxY - m_walkArea.minY};
        SDL_SetRenderDrawColor(renderer, 40, 35, 30, 255);
        SDL_RenderFillRect(renderer, &walkRect);
    }

    // Rita hotspots (debug)
    renderDebugInfo(renderer);

    // UI-bar
    SDL_Rect uiBar = {0, 375, 640, 25};
    SDL_SetRenderDrawColor(renderer, 0, 0, 40, 255);
    SDL_RenderFillRect(renderer, &uiBar);
    
    SDL_SetRenderDrawColor(renderer, 100, 100, 200, 255);
    SDL_RenderDrawLine(renderer, 0, 375, 640, 375);
}

void Room::renderDebugInfo(SDL_Renderer* renderer) {
    // Rita hotspots med färgkodning
    for (const auto& hs : m_hotspots) {
        if (!hs.active) continue;
        
        // Färg baserat på typ
        switch (hs.type) {
            case HotspotType::NPC:
                SDL_SetRenderDrawColor(renderer, 100, 200, 100, 200);
                break;
            case HotspotType::Item:
                SDL_SetRenderDrawColor(renderer, 200, 200, 100, 200);
                break;
            case HotspotType::Exit:
                SDL_SetRenderDrawColor(renderer, 100, 100, 200, 200);
                break;
            default:
                SDL_SetRenderDrawColor(renderer, 150, 150, 150, 200);
        }
        
        SDL_RenderFillRect(renderer, &hs.rect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawRect(renderer, &hs.rect);
    }
}

void Room::addHotspot(const std::string& id, const std::string& name,
                       int x, int y, int w, int h, HotspotType type) {
    Hotspot hs;
    hs.id = id;
    hs.name = name;
    hs.rect = {x, y, w, h};
    hs.type = type;
    hs.active = true;
    m_hotspots.push_back(hs);
}

void Room::addExit(const std::string& id, const std::string& name,
                   int x, int y, int w, int h, const std::string& targetRoom) {
    Hotspot hs;
    hs.id = id;
    hs.name = name;
    hs.rect = {x, y, w, h};
    hs.type = HotspotType::Exit;
    hs.targetRoom = targetRoom;
    hs.active = true;
    m_hotspots.push_back(hs);
}

void Room::setWalkArea(int minX, int maxX, int minY, int maxY) {
    m_walkArea = {minX, maxX, minY, maxY};
}

bool Room::isInWalkArea(float x, float y) const {
    return x >= m_walkArea.minX && x <= m_walkArea.maxX &&
           y >= m_walkArea.minY && y <= m_walkArea.maxY;
}

void Room::clampToWalkArea(float& x, float& y, int width, int height) const {
    if (x < m_walkArea.minX) x = m_walkArea.minX;
    if (x > m_walkArea.maxX - width) x = m_walkArea.maxX - width;
    if (y < m_walkArea.minY) y = m_walkArea.minY;
    if (y > m_walkArea.maxY - height) y = m_walkArea.maxY - height;
}

Hotspot* Room::getHotspotAt(int x, int y) {
    for (auto& hs : m_hotspots) {
        if (!hs.active) continue;
        
        SDL_Point point = {x, y};
        if (SDL_PointInRect(&point, &hs.rect)) {
            return &hs;
        }
    }
    return nullptr;
}

// ============================================================================
// NPC MANAGEMENT
// ============================================================================

void Room::addNPC(std::unique_ptr<NPC> npc) {
    if (npc) {
        m_npcs.push_back(std::move(npc));
    }
}

NPC* Room::getNPC(const std::string& id) {
    for (auto& npc : m_npcs) {
        if (npc && npc->getName() == id) {
            return npc.get();
        }
    }
    return nullptr;
}

void Room::updateNPCs(float deltaTime) {
    for (auto& npc : m_npcs) {
        if (npc) {
            npc->update(deltaTime);
        }
    }
}

void Room::renderNPCs(SDL_Renderer* renderer) {
    for (auto& npc : m_npcs) {
        if (npc) {
            npc->render(renderer);
        }
    }
}
