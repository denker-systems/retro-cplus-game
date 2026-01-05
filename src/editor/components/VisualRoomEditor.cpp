/**
 * @file VisualRoomEditor.cpp
 * @brief Implementation av visuell rum-editor
 */
#include "VisualRoomEditor.h"
#include "engine/graphics/FontManager.h"
#include <algorithm>

VisualRoomEditor::VisualRoomEditor() = default;
VisualRoomEditor::~VisualRoomEditor() = default;

void VisualRoomEditor::render(SDL_Renderer* renderer, const RoomData& roomData, SDL_Texture* previewTexture) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    
    // Rita bakgrund
    if (previewTexture) {
        SDL_Rect dstRect = {10, 90, 620, 260};
        SDL_RenderCopy(renderer, previewTexture, nullptr, &dstRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255);
        SDL_Rect bg = {10, 90, 620, 260};
        SDL_RenderFillRect(renderer, &bg);
    }
    
    float scaleX = 620.0f / 640.0f;
    float scaleY = 260.0f / 400.0f;
    
    renderHotspots(renderer, roomData.hotspots, scaleX, scaleY);
    renderWalkArea(renderer, roomData.walkArea, scaleX, scaleY);
    renderPlayerSpawn(renderer, roomData.playerSpawnX, roomData.playerSpawnY, scaleX, scaleY);
    
    // Visa info
    if (m_selectedHotspot >= 0 && m_selectedHotspot < static_cast<int>(roomData.hotspots.size())) {
        const auto& hs = roomData.hotspots[m_selectedHotspot];
        std::string info = "Selected: " + hs.name + " [" + hs.type + "] " + 
                          std::to_string(hs.w) + "x" + std::to_string(hs.h) + " at (" + 
                          std::to_string(hs.x) + "," + std::to_string(hs.y) + ")";
        FontManager::instance().renderText(renderer, "default", info, 10, 355, yellow);
    } else {
        char scaleStr[64];
        snprintf(scaleStr, sizeof(scaleStr), "Depth: %.0f%% (top) to %.0f%% (bottom)", 
                 roomData.walkArea.scaleTop * 100, roomData.walkArea.scaleBottom * 100);
        FontManager::instance().renderText(renderer, "default", scaleStr, 10, 355, cyan);
    }
}

void VisualRoomEditor::renderHotspots(SDL_Renderer* renderer, const std::vector<HotspotData>& hotspots, float scaleX, float scaleY) {
    SDL_Color white = {255, 255, 255, 255};
    
    for (size_t i = 0; i < hotspots.size(); i++) {
        const auto& hs = hotspots[i];
        bool selected = (static_cast<int>(i) == m_selectedHotspot);
        
        SDL_Rect rect = {
            10 + static_cast<int>(hs.x * scaleX),
            90 + static_cast<int>(hs.y * scaleY),
            static_cast<int>(hs.w * scaleX),
            static_cast<int>(hs.h * scaleY)
        };
        
        // Färg baserat på typ
        if (hs.type == "npc") {
            SDL_SetRenderDrawColor(renderer, 100, 255, 100, selected ? 200 : 100);
        } else if (hs.type == "item") {
            SDL_SetRenderDrawColor(renderer, 255, 255, 100, selected ? 200 : 100);
        } else if (hs.type == "exit") {
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, selected ? 200 : 100);
        } else {
            SDL_SetRenderDrawColor(renderer, 150, 150, 150, selected ? 200 : 100);
        }
        
        SDL_RenderFillRect(renderer, &rect);
        
        // Ram
        if (selected) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
            SDL_Rect outerRect = {rect.x - 1, rect.y - 1, rect.w + 2, rect.h + 2};
            SDL_RenderDrawRect(renderer, &outerRect);
            
            // Resize handles
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            int hs_size = 6;
            SDL_Rect handles[4] = {
                {rect.x - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},
                {rect.x + rect.w - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},
                {rect.x - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size},
                {rect.x + rect.w - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size}
            };
            for (int h = 0; h < 4; h++) {
                SDL_RenderFillRect(renderer, &handles[h]);
            }
            
            FontManager::instance().renderText(renderer, "default", hs.name, rect.x + 2, rect.y + 2, white);
        } else {
            SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}

void VisualRoomEditor::renderWalkArea(SDL_Renderer* renderer, const WalkAreaData& walkArea, float scaleX, float scaleY) {
    SDL_Rect walkRect = {
        10 + static_cast<int>(walkArea.minX * scaleX),
        90 + static_cast<int>(walkArea.minY * scaleY),
        static_cast<int>((walkArea.maxX - walkArea.minX) * scaleX),
        static_cast<int>((walkArea.maxY - walkArea.minY) * scaleY)
    };
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 200, m_editingWalkArea ? 200 : 100);
    SDL_RenderDrawRect(renderer, &walkRect);
    SDL_Rect innerWalk = {walkRect.x + 1, walkRect.y + 1, walkRect.w - 2, walkRect.h - 2};
    SDL_RenderDrawRect(renderer, &innerWalk);
    
    // Handles
    if (m_editingWalkArea || m_selectedHotspot < 0) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 200, 255);
        int wh_size = 8;
        SDL_Rect walkHandles[4] = {
            {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y - wh_size/2, wh_size, wh_size},
            {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y + walkRect.h - wh_size/2, wh_size, wh_size},
            {walkRect.x - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size},
            {walkRect.x + walkRect.w - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size}
        };
        for (int h = 0; h < 4; h++) {
            SDL_RenderFillRect(renderer, &walkHandles[h]);
        }
    }
    
    // Depth scale figurer
    int figTopH = static_cast<int>(20 * walkArea.scaleTop);
    int figBotH = static_cast<int>(20 * walkArea.scaleBottom);
    SDL_SetRenderDrawColor(renderer, 255, 200, 100, 200);
    SDL_Rect figTop = {walkRect.x + 5, walkRect.y + 5, 8, figTopH};
    SDL_Rect figBot = {walkRect.x + 5, walkRect.y + walkRect.h - figBotH - 5, 8, figBotH};
    SDL_RenderFillRect(renderer, &figTop);
    SDL_RenderFillRect(renderer, &figBot);
}

void VisualRoomEditor::renderPlayerSpawn(SDL_Renderer* renderer, float spawnX, float spawnY, float scaleX, float scaleY) {
    int screenX = 10 + static_cast<int>(spawnX * scaleX);
    int screenY = 90 + static_cast<int>(spawnY * scaleY);
    SDL_SetRenderDrawColor(renderer, 255, 100, 255, m_editingPlayerSpawn ? 255 : 150);
    SDL_Rect spawnRect = {screenX - 8, screenY - 12, 16, 24};
    SDL_RenderFillRect(renderer, &spawnRect);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &spawnRect);
}

void VisualRoomEditor::handleMouseClick(int x, int y, RoomData& roomData) {
    if (x < 10 || x >= 630 || y < 90 || y >= 350) return;
    
    float scaleX = 620.0f / 640.0f;
    float scaleY = 260.0f / 400.0f;
    int hs_size = 6;
    int wh_size = 8;
    
    // Kolla resize handles
    if (m_selectedHotspot >= 0 && m_selectedHotspot < static_cast<int>(roomData.hotspots.size())) {
        const auto& hs = roomData.hotspots[m_selectedHotspot];
        SDL_Rect rect = {
            10 + static_cast<int>(hs.x * scaleX),
            90 + static_cast<int>(hs.y * scaleY),
            static_cast<int>(hs.w * scaleX),
            static_cast<int>(hs.h * scaleY)
        };
        
        SDL_Rect handles[4] = {
            {rect.x - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},
            {rect.x + rect.w - hs_size/2, rect.y - hs_size/2, hs_size, hs_size},
            {rect.x - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size},
            {rect.x + rect.w - hs_size/2, rect.y + rect.h - hs_size/2, hs_size, hs_size}
        };
        
        for (int h = 0; h < 4; h++) {
            if (x >= handles[h].x && x < handles[h].x + handles[h].w &&
                y >= handles[h].y && y < handles[h].y + handles[h].h) {
                m_resizingHotspot = true;
                m_resizeHandle = h;
                m_dragStartX = x;
                m_dragStartY = y;
                m_hotspotOrigX = hs.x;
                m_hotspotOrigY = hs.y;
                m_hotspotOrigW = hs.w;
                m_hotspotOrigH = hs.h;
                return;
            }
        }
    }
    
    // Kolla walk area handles
    const auto& wa = roomData.walkArea;
    SDL_Rect walkRect = {
        10 + static_cast<int>(wa.minX * scaleX),
        90 + static_cast<int>(wa.minY * scaleY),
        static_cast<int>((wa.maxX - wa.minX) * scaleX),
        static_cast<int>((wa.maxY - wa.minY) * scaleY)
    };
    
    SDL_Rect walkHandles[4] = {
        {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y - wh_size/2, wh_size, wh_size},
        {walkRect.x + walkRect.w/2 - wh_size/2, walkRect.y + walkRect.h - wh_size/2, wh_size, wh_size},
        {walkRect.x - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size},
        {walkRect.x + walkRect.w - wh_size/2, walkRect.y + walkRect.h/2 - wh_size/2, wh_size, wh_size}
    };
    
    for (int h = 0; h < 4; h++) {
        if (x >= walkHandles[h].x && x < walkHandles[h].x + walkHandles[h].w &&
            y >= walkHandles[h].y && y < walkHandles[h].y + walkHandles[h].h) {
            m_editingWalkArea = true;
            m_walkAreaHandle = h;
            m_dragStartX = x;
            m_dragStartY = y;
            m_hotspotOrigX = wa.minX;
            m_hotspotOrigY = wa.minY;
            m_hotspotOrigW = wa.maxX;
            m_hotspotOrigH = wa.maxY;
            m_selectedHotspot = -1;
            return;
        }
    }
    
    // Kolla player spawn
    int spawnScreenX = 10 + static_cast<int>(roomData.playerSpawnX * scaleX);
    int spawnScreenY = 90 + static_cast<int>(roomData.playerSpawnY * scaleY);
    SDL_Rect spawnClickRect = {spawnScreenX - 8, spawnScreenY - 12, 16, 24};
    
    if (x >= spawnClickRect.x && x < spawnClickRect.x + spawnClickRect.w &&
        y >= spawnClickRect.y && y < spawnClickRect.y + spawnClickRect.h) {
        m_editingPlayerSpawn = true;
        m_selectedHotspot = -1;
        return;
    }
    
    // Kolla hotspots
    for (int i = static_cast<int>(roomData.hotspots.size()) - 1; i >= 0; i--) {
        const auto& hs = roomData.hotspots[i];
        SDL_Rect rect = {
            10 + static_cast<int>(hs.x * scaleX),
            90 + static_cast<int>(hs.y * scaleY),
            static_cast<int>(hs.w * scaleX),
            static_cast<int>(hs.h * scaleY)
        };
        
        if (x >= rect.x && x < rect.x + rect.w &&
            y >= rect.y && y < rect.y + rect.h) {
            m_selectedHotspot = i;
            m_draggingHotspot = true;
            m_dragStartX = x;
            m_dragStartY = y;
            m_hotspotOrigX = hs.x;
            m_hotspotOrigY = hs.y;
            return;
        }
    }
    
    m_selectedHotspot = -1;
}

void VisualRoomEditor::handleMouseMotion(int mouseX, int mouseY, RoomData& roomData) {
    float scaleX = 640.0f / 620.0f;
    float scaleY = 400.0f / 260.0f;
    
    if (m_resizingHotspot && m_selectedHotspot >= 0 && m_selectedHotspot < static_cast<int>(roomData.hotspots.size())) {
        int deltaX = static_cast<int>((mouseX - m_dragStartX) * scaleX);
        int deltaY = static_cast<int>((mouseY - m_dragStartY) * scaleY);
        
        auto& hs = roomData.hotspots[m_selectedHotspot];
        
        switch (m_resizeHandle) {
            case 0: // TL
                hs.x = m_hotspotOrigX + deltaX;
                hs.y = m_hotspotOrigY + deltaY;
                hs.w = m_hotspotOrigW - deltaX;
                hs.h = m_hotspotOrigH - deltaY;
                break;
            case 1: // TR
                hs.y = m_hotspotOrigY + deltaY;
                hs.w = m_hotspotOrigW + deltaX;
                hs.h = m_hotspotOrigH - deltaY;
                break;
            case 2: // BL
                hs.x = m_hotspotOrigX + deltaX;
                hs.w = m_hotspotOrigW - deltaX;
                hs.h = m_hotspotOrigH + deltaY;
                break;
            case 3: // BR
                hs.w = m_hotspotOrigW + deltaX;
                hs.h = m_hotspotOrigH + deltaY;
                break;
        }
        
        if (hs.w < 10) hs.w = 10;
        if (hs.h < 10) hs.h = 10;
    }
    else if (m_draggingHotspot && m_selectedHotspot >= 0 && m_selectedHotspot < static_cast<int>(roomData.hotspots.size())) {
        int deltaX = static_cast<int>((mouseX - m_dragStartX) * scaleX);
        int deltaY = static_cast<int>((mouseY - m_dragStartY) * scaleY);
        
        auto& hs = roomData.hotspots[m_selectedHotspot];
        hs.x = m_hotspotOrigX + deltaX;
        hs.y = m_hotspotOrigY + deltaY;
        
        if (hs.x < 0) hs.x = 0;
        if (hs.y < 0) hs.y = 0;
        if (hs.x + hs.w > 640) hs.x = 640 - hs.w;
        if (hs.y + hs.h > 400) hs.y = 400 - hs.h;
    }
    else if (m_editingWalkArea && m_walkAreaHandle >= 0) {
        int deltaX = static_cast<int>((mouseX - m_dragStartX) * scaleX);
        int deltaY = static_cast<int>((mouseY - m_dragStartY) * scaleY);
        
        auto& wa = roomData.walkArea;
        
        switch (m_walkAreaHandle) {
            case 0: wa.minY = m_hotspotOrigY + deltaY; break;
            case 1: wa.maxY = m_hotspotOrigH + deltaY; break;
            case 2: wa.minX = m_hotspotOrigX + deltaX; break;
            case 3: wa.maxX = m_hotspotOrigW + deltaX; break;
        }
        
        if (wa.minX < 0) wa.minX = 0;
        if (wa.minY < 0) wa.minY = 0;
        if (wa.maxX > 640) wa.maxX = 640;
        if (wa.maxY > 400) wa.maxY = 400;
        if (wa.minX > wa.maxX - 20) wa.minX = wa.maxX - 20;
        if (wa.minY > wa.maxY - 20) wa.minY = wa.maxY - 20;
    }
    else if (m_editingPlayerSpawn) {
        float gameX = (mouseX - 10) / (620.0f / 640.0f);
        float gameY = (mouseY - 90) / (260.0f / 400.0f);
        
        if (gameX < 0) gameX = 0;
        if (gameX > 640) gameX = 640;
        if (gameY < 0) gameY = 0;
        if (gameY > 400) gameY = 400;
        
        roomData.playerSpawnX = gameX;
        roomData.playerSpawnY = gameY;
    }
}

void VisualRoomEditor::handleMouseUp() {
    m_draggingHotspot = false;
    m_resizingHotspot = false;
    m_resizeHandle = -1;
    m_editingWalkArea = false;
    m_walkAreaHandle = -1;
    m_editingPlayerSpawn = false;
}

void VisualRoomEditor::adjustDepthScale(bool increase, bool top, RoomData& roomData) {
    float step = increase ? 0.05f : -0.05f;
    
    if (top) {
        roomData.walkArea.scaleTop = std::max(0.1f, std::min(2.0f, roomData.walkArea.scaleTop + step));
    } else {
        roomData.walkArea.scaleBottom = std::max(0.1f, std::min(2.0f, roomData.walkArea.scaleBottom + step));
    }
}

bool VisualRoomEditor::deleteSelectedHotspot(RoomData& roomData) {
    if (m_selectedHotspot >= 0 && m_selectedHotspot < static_cast<int>(roomData.hotspots.size())) {
        roomData.hotspots.erase(roomData.hotspots.begin() + m_selectedHotspot);
        m_selectedHotspot = -1;
        return true;
    }
    return false;
}
