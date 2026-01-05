/**
 * @file EditorContext.cpp
 * @brief Implementation of shared editor context
 */
#include "EditorContext.h"
#include "../data/DataLoader.h"
#include "../utils/Logger.h"
#include <nlohmann/json.hpp>
#include <SDL_image.h>
#include <fstream>

using json = nlohmann::json;

EditorContext::~EditorContext() {
    clearTextureCache();
}

void EditorContext::setSelection(SelectionType type, const std::string& id, int index) {
    m_selection.type = type;
    m_selection.primaryId = id;
    m_selection.secondaryIndex = index;
}

void EditorContext::clearSelection() {
    m_selection.clear();
}

void EditorContext::copyToClipboard(const std::string& type, const std::any& data) {
    m_clipboard[type] = data;
}

bool EditorContext::hasClipboardData(const std::string& type) const {
    return m_clipboard.find(type) != m_clipboard.end();
}

std::any EditorContext::getClipboardData(const std::string& type) const {
    auto it = m_clipboard.find(type);
    return it != m_clipboard.end() ? it->second : std::any{};
}

SDL_Texture* EditorContext::getTexture(SDL_Renderer* renderer, const std::string& path) {
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) {
        return it->second;
    }
    
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (texture) {
        m_textureCache[path] = texture;
    } else {
        LOG_WARNING("Failed to load texture: " + path);
    }
    
    return texture;
}

void EditorContext::clearTextureCache() {
    for (auto& pair : m_textureCache) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    m_textureCache.clear();
}

void EditorContext::loadFromDataLoader() {
    // Copy data from DataLoader (make mutable copies for editing)
    const auto& dl = DataLoader::instance();
    
    rooms = dl.getRooms();
    dialogs = dl.getDialogs();
    quests = dl.getQuests();
    items = dl.getItems();
    npcs = dl.getNPCs();
    
    LOG_INFO("EditorContext loaded " + std::to_string(rooms.size()) + " rooms, " +
             std::to_string(dialogs.size()) + " dialogs");
}

void EditorContext::saveToFiles() {
    // Save rooms
    {
        json data;
        data["rooms"] = json::array();
        
        for (const auto& room : rooms) {
            json roomJson;
            roomJson["id"] = room.id;
            roomJson["name"] = room.name;
            roomJson["background"] = room.background;
            
            // Walk area
            roomJson["walkArea"]["minX"] = room.walkArea.minX;
            roomJson["walkArea"]["maxX"] = room.walkArea.maxX;
            roomJson["walkArea"]["minY"] = room.walkArea.minY;
            roomJson["walkArea"]["maxY"] = room.walkArea.maxY;
            
            // Layers
            roomJson["layers"] = json::array();
            for (const auto& layer : room.layers) {
                json layerJson;
                layerJson["image"] = layer.image;
                layerJson["zIndex"] = layer.zIndex;
                layerJson["parallaxX"] = layer.parallaxX;
                layerJson["parallaxY"] = layer.parallaxY;
                layerJson["opacity"] = layer.opacity;
                roomJson["layers"].push_back(layerJson);
            }
            
            // Hotspots
            roomJson["hotspots"] = json::array();
            for (const auto& hs : room.hotspots) {
                json hsJson;
                hsJson["id"] = hs.id;
                hsJson["name"] = hs.name;
                hsJson["type"] = hs.type;
                hsJson["x"] = hs.x;
                hsJson["y"] = hs.y;
                hsJson["w"] = hs.w;
                hsJson["h"] = hs.h;
                
                if (!hs.dialogId.empty()) hsJson["dialogId"] = hs.dialogId;
                if (!hs.examineText.empty()) hsJson["examineText"] = hs.examineText;
                if (!hs.targetRoom.empty()) hsJson["targetRoom"] = hs.targetRoom;
                
                if (!hs.funnyFails.empty()) {
                    hsJson["funnyFails"] = hs.funnyFails;
                }
                
                roomJson["hotspots"].push_back(hsJson);
            }
            
            data["rooms"].push_back(roomJson);
        }
        
        std::ofstream file("assets/data/rooms.json");
        if (file.is_open()) {
            file << data.dump(2);
            file.close();
            LOG_INFO("Saved rooms.json");
        }
    }
    
    // Reload DataLoader to sync
    DataLoader::instance().loadAll();
}

