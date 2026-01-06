/**
 * @file Scene.cpp
 * @brief Implementation av scene och hotspots
 */
#include "Scene.h"
#include "actors/NPC.h"
#include "engine/graphics/TextureManager.h"
#include "engine/utils/Logger.h"
#include <iostream>
#include <algorithm>
#include <SDL_image.h>

Scene::Scene(const std::string& id, const std::string& name) 
    : m_id(id), m_name(name) {
    // Default walk area
    setWalkArea(0, 640, 260, 350);
}

Scene::~Scene() {
    if (m_background) {
        SDL_DestroyTexture(m_background);
        m_background = nullptr;
    }
    
    // Rensa layers
    for (auto& layer : m_layers) {
        if (layer.texture) {
            SDL_DestroyTexture(layer.texture);
        }
    }
    m_layers.clear();
}

bool Scene::loadBackground(SDL_Renderer* renderer, const std::string& path) {
    if (m_background) {
        SDL_DestroyTexture(m_background);
    }
    
    m_background = IMG_LoadTexture(renderer, path.c_str());
    if (!m_background) {
        std::cerr << "Failed to load background: " << path << " - " << (IMG_GetError() ? IMG_GetError() : "Unknown error") << std::endl;
        return false;
    }
    
    std::cout << "Loaded scene background: " << path << std::endl;
    return true;
}

void Scene::render(SDL_Renderer* renderer) {
    // Rita layers om de finns
    if (!m_layers.empty()) {
        // All rendering är nu i renderLayers()
    } else if (m_background) {
        // Legacy background rendering
        SDL_Rect dest = {0, 0, 640, 400};
        SDL_RenderCopy(renderer, m_background, nullptr, &dest);
    }
    
    // Rita hotspots (debug)
    renderDebugInfo(renderer);
}

bool Scene::loadLayer(SDL_Renderer* renderer, const std::string& imagePath, int zIndex, 
                     float parallaxX, float parallaxY, float opacity) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, imagePath.c_str());
    if (!texture) {
        std::cerr << "Failed to load layer: " << imagePath << " - " << (IMG_GetError() ? IMG_GetError() : "Unknown error") << std::endl;
        return false;
    }
    
    RoomLayer layer;
    layer.texture = texture;
    layer.zIndex = zIndex;
    layer.parallaxX = parallaxX;
    layer.parallaxY = parallaxY;
    layer.opacity = opacity;
    
    m_layers.push_back(layer);
    
    // Sortera layers efter zIndex
    std::sort(m_layers.begin(), m_layers.end(), 
              [](const RoomLayer& a, const RoomLayer& b) {
                  return a.zIndex < b.zIndex;
              });
    
    return true;
}

void Scene::renderLayers(SDL_Renderer* renderer, int playerY, bool renderBehind) {
    for (const auto& layer : m_layers) {
        // Rendera bara bakom eller framför spelaren
        if ((renderBehind && layer.zIndex < 0) || (!renderBehind && layer.zIndex >= 0)) {
            if (layer.texture) {
                // Parallax scrolling baserat på player position
                int offsetX = 0, offsetY = 0;
                
                if (layer.parallaxX != 1.0f) {
                    // Simple parallax - justera X baserat på player position
                    float playerPercent = playerY / 400.0f;  // Normalized Y
                    offsetX = static_cast<int>((playerPercent - 0.5f) * 100 * (1.0f - layer.parallaxX));
                }
                
                SDL_Rect dest = {offsetX, offsetY, 640, 400};
                
                // Alpha blending
                if (layer.opacity < 1.0f) {
                    SDL_SetTextureAlphaMod(layer.texture, 
                                         static_cast<Uint8>(layer.opacity * 255));
                }
                
                SDL_RenderCopy(renderer, layer.texture, nullptr, &dest);
            }
        }
    }
}

void Scene::addHotspot(const std::string& id, const std::string& name, 
                       int x, int y, int w, int h, HotspotType type,
                       const std::string& dialogId,
                       const std::string& examineText,
                       const std::vector<std::string>& funnyFails) {
    Hotspot hotspot;
    hotspot.id = id;
    hotspot.name = name;
    hotspot.rect = {x, y, w, h};
    hotspot.type = type;
    hotspot.dialogId = dialogId;
    hotspot.examineText = examineText;
    hotspot.funnyFails = funnyFails;
    
    m_hotspots.push_back(hotspot);
}

void Scene::addExit(const std::string& id, const std::string& name,
                    int x, int y, int w, int h, const std::string& targetRoom) {
    addHotspot(id, name, x, y, w, h, HotspotType::Exit, "", "", {});
    m_hotspots.back().targetRoom = targetRoom;
}

void Scene::setWalkArea(int minX, int maxX, int minY, int maxY, float scaleTop, float scaleBottom) {
    m_walkArea.minX = minX;
    m_walkArea.maxX = maxX;
    m_walkArea.minY = minY;
    m_walkArea.maxY = maxY;
    m_walkArea.scaleTop = scaleTop;
    m_walkArea.scaleBottom = scaleBottom;
}

bool Scene::isInWalkArea(float x, float y) const {
    return x >= m_walkArea.minX && x <= m_walkArea.maxX &&
           y >= m_walkArea.minY && y <= m_walkArea.maxY;
}

void Scene::clampToWalkArea(float& x, float& y, int width, int height) const {
    x = std::max(static_cast<float>(m_walkArea.minX), 
                std::min(x, static_cast<float>(m_walkArea.maxX - width)));
    y = std::max(static_cast<float>(m_walkArea.minY), 
                std::min(y, static_cast<float>(m_walkArea.maxY - height)));
}

Hotspot* Scene::getHotspotAt(int x, int y) {
    for (auto& hotspot : m_hotspots) {
        if (hotspot.active &&
            x >= hotspot.rect.x && x < hotspot.rect.x + hotspot.rect.w &&
            y >= hotspot.rect.y && y < hotspot.rect.y + hotspot.rect.h) {
            return &hotspot;
        }
    }
    return nullptr;
}

void Scene::addNPC(std::unique_ptr<engine::actors::NPC> npc) {
    m_npcs.push_back(std::move(npc));
}

engine::actors::NPC* Scene::getNPC(const std::string& id) {
    for (auto& npc : m_npcs) {
        if (npc && npc->getName() == id) {
            return npc.get();
        }
    }
    return nullptr;
}

void Scene::updateNPCs(float deltaTime) {
    for (auto& npc : m_npcs) {
        if (npc) {
            npc->update(deltaTime);
        }
    }
}

void Scene::renderNPCs(SDL_Renderer* renderer) {
    for (auto& npc : m_npcs) {
        if (npc) {
            npc->render(renderer);
        }
    }
}

void Scene::renderDebugInfo(SDL_Renderer* renderer) {
    // Rita walk area (grön)
    SDL_Rect walkRect = {
        m_walkArea.minX, m_walkArea.minY,
        m_walkArea.maxX - m_walkArea.minX,
        m_walkArea.maxY - m_walkArea.minY
    };
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
    SDL_RenderDrawRect(renderer, &walkRect);
    
    // Rita hotspots (cyan)
    for (const auto& hotspot : m_hotspots) {
        if (hotspot.active) {
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 100);
            SDL_RenderDrawRect(renderer, &hotspot.rect);
        }
    }
}
