/**
 * @file RoomPanel.cpp
 * @brief Visual room editor panel implementation
 */
#include "RoomPanel.h"
#include "../EditorCore.h"
#include "../EditorContext.h"
#include "../widgets/Button.h"
#include "../widgets/ListView.h"
#include "../commands/HotspotCommands.h"
#include "engine/graphics/FontManager.h"
#include "engine/audio/AudioManager.h"
#include "../../utils/Logger.h"
#include <SDL_image.h>

RoomPanel::RoomPanel() {
    LOG_INFO("RoomPanel created");
}

RoomPanel::~RoomPanel() {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
    }
}

void RoomPanel::onActivate() {
    LOG_INFO("RoomPanel activated");
    
    // Select first room if none selected
    auto& ctx = EditorCore::instance().getContext();
    if (!ctx.rooms.empty() && !ctx.getSelection().hasSelection()) {
        selectRoom(ctx.rooms[0].id);
    }
}

void RoomPanel::onDeactivate() {
    LOG_INFO("RoomPanel deactivated");
}

void RoomPanel::update(float deltaTime) {
    (void)deltaTime;
}

void RoomPanel::render(SDL_Renderer* renderer) {
    renderRoomList(renderer);
    renderToolbar(renderer);
    renderRoomPreview(renderer);
    renderHotspots(renderer);
    renderPropertyPanel(renderer);
}

bool RoomPanel::handleEvent(const SDL_Event& event) {
    auto& ctx = EditorCore::instance().getContext();
    
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            int x = event.button.x;
            int y = event.button.y;
            
            // Check if click is in preview area
            if (x >= PREVIEW_X && x < PREVIEW_X + PREVIEW_WIDTH &&
                y >= PREVIEW_Y && y < PREVIEW_Y + PREVIEW_HEIGHT) {
                handlePreviewClick(x, y, event.button.button);
                return true;
            }
            
            // Check room list clicks
            if (x >= 10 && x < LIST_WIDTH && y >= 90) {
                int index = (y - 90) / 18;
                if (index >= 0 && index < static_cast<int>(ctx.rooms.size())) {
                    selectRoom(ctx.rooms[index].id);
                    AudioManager::instance().playSound("select");
                    return true;
                }
            }
            break;
        }
        
        case SDL_MOUSEMOTION: {
            if (m_isDragging && m_currentRoom && m_selectedHotspotIndex >= 0) {
                handlePreviewDrag(event.motion.x, event.motion.y);
                return true;
            }
            break;
        }
        
        case SDL_MOUSEBUTTONUP: {
            if (m_isDragging) {
                handlePreviewRelease(event.button.x, event.button.y);
                return true;
            }
            break;
        }
        
        case SDL_KEYDOWN: {
            if (event.key.keysym.scancode == SDL_SCANCODE_DELETE && m_selectedHotspotIndex >= 0) {
                if (m_currentRoom) {
                    auto cmd = std::make_unique<DeleteHotspotCommand>(
                        m_currentRoom->id, m_selectedHotspotIndex);
                    EditorCore::instance().executeCommand(std::move(cmd));
                    m_selectedHotspotIndex = -1;
                }
                return true;
            }
            
            // Tool shortcuts
            if (event.key.keysym.scancode == SDL_SCANCODE_V) {
                m_currentTool = RoomEditorTool::Select;
                return true;
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_R) {
                m_currentTool = RoomEditorTool::Resize;
                return true;
            }
            if (event.key.keysym.scancode == SDL_SCANCODE_C) {
                m_currentTool = RoomEditorTool::Create;
                return true;
            }
            break;
        }
    }
    
    return false;
}

std::string RoomPanel::getShortcutHint() const {
    return "V: Select | R: Resize | C: Create | DEL: Delete";
}

void RoomPanel::renderRoomList(SDL_Renderer* renderer) {
    auto& ctx = EditorCore::instance().getContext();
    
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Rooms]", 10, 70, cyan);
    
    int y = 90;
    for (const auto& room : ctx.rooms) {
        bool selected = (m_currentRoom && room.id == m_currentRoom->id);
        SDL_Color color = selected ? yellow : white;
        
        std::string text = (selected ? "> " : "  ") + room.id;
        FontManager::instance().renderText(renderer, "default", text, 10, y, color);
        y += 18;
    }
}

void RoomPanel::renderToolbar(SDL_Renderer* renderer) {
    SDL_Color green = {100, 255, 100, 255};
    SDL_Color white = {255, 255, 255, 255};
    
    // Tool indicator
    std::string toolName;
    switch (m_currentTool) {
        case RoomEditorTool::Select: toolName = "[Select]"; break;
        case RoomEditorTool::Resize: toolName = "[Resize]"; break;
        case RoomEditorTool::Create: toolName = "[Create]"; break;
        case RoomEditorTool::Pan: toolName = "[Pan]"; break;
    }
    
    FontManager::instance().renderText(renderer, "default", "Tool: " + toolName, PREVIEW_X, 70, green);
    
    // Undo/Redo status
    auto& core = EditorCore::instance();
    std::string undoText = core.canUndo() ? "Ctrl+Z: Undo" : "";
    std::string redoText = core.canRedo() ? "Ctrl+Y: Redo" : "";
    
    if (!undoText.empty()) {
        FontManager::instance().renderText(renderer, "default", undoText, PREVIEW_X + 150, 70, white);
    }
    if (!redoText.empty()) {
        FontManager::instance().renderText(renderer, "default", redoText, PREVIEW_X + 270, 70, white);
    }
}

void RoomPanel::renderRoomPreview(SDL_Renderer* renderer) {
    // Preview background
    SDL_Rect previewRect = {PREVIEW_X, PREVIEW_Y, PREVIEW_WIDTH, PREVIEW_HEIGHT};
    
    if (m_roomTexture) {
        SDL_RenderCopy(renderer, m_roomTexture, nullptr, &previewRect);
    } else {
        SDL_SetRenderDrawColor(renderer, 30, 30, 50, 255);
        SDL_RenderFillRect(renderer, &previewRect);
    }
    
    // Border
    SDL_SetRenderDrawColor(renderer, 100, 100, 150, 255);
    SDL_RenderDrawRect(renderer, &previewRect);
    
    // Grid (if enabled)
    auto& ctx = EditorCore::instance().getContext();
    if (ctx.showGrid) {
        SDL_SetRenderDrawColor(renderer, 60, 60, 80, 100);
        for (int x = 0; x < PREVIEW_WIDTH; x += 20) {
            SDL_RenderDrawLine(renderer, PREVIEW_X + x, PREVIEW_Y, 
                              PREVIEW_X + x, PREVIEW_Y + PREVIEW_HEIGHT);
        }
        for (int y = 0; y < PREVIEW_HEIGHT; y += 20) {
            SDL_RenderDrawLine(renderer, PREVIEW_X, PREVIEW_Y + y,
                              PREVIEW_X + PREVIEW_WIDTH, PREVIEW_Y + y);
        }
    }
}

void RoomPanel::renderHotspots(SDL_Renderer* renderer) {
    if (!m_currentRoom) return;
    
    auto& ctx = EditorCore::instance().getContext();
    float scaleX = static_cast<float>(PREVIEW_WIDTH) / 640.0f;
    float scaleY = static_cast<float>(PREVIEW_HEIGHT) / 400.0f;
    
    for (size_t i = 0; i < m_currentRoom->hotspots.size(); i++) {
        const auto& hs = m_currentRoom->hotspots[i];
        bool selected = (static_cast<int>(i) == m_selectedHotspotIndex);
        
        SDL_Rect rect = {
            PREVIEW_X + static_cast<int>(hs.x * scaleX),
            PREVIEW_Y + static_cast<int>(hs.y * scaleY),
            static_cast<int>(hs.w * scaleX),
            static_cast<int>(hs.h * scaleY)
        };
        
        // Color by type
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
        
        // Border
        SDL_SetRenderDrawColor(renderer, selected ? 255 : 200, selected ? 255 : 200, selected ? 255 : 200, 255);
        SDL_RenderDrawRect(renderer, &rect);
        
        // Resize handles for selected hotspot
        if (selected && m_currentTool == RoomEditorTool::Resize) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            int handleSize = 6;
            
            // Corners
            SDL_Rect handles[] = {
                {rect.x - handleSize/2, rect.y - handleSize/2, handleSize, handleSize},
                {rect.x + rect.w - handleSize/2, rect.y - handleSize/2, handleSize, handleSize},
                {rect.x - handleSize/2, rect.y + rect.h - handleSize/2, handleSize, handleSize},
                {rect.x + rect.w - handleSize/2, rect.y + rect.h - handleSize/2, handleSize, handleSize}
            };
            
            for (const auto& handle : handles) {
                SDL_RenderFillRect(renderer, &handle);
            }
        }
        
        // Name label
        if (ctx.showHotspotNames || selected) {
            SDL_Color labelColor = {255, 255, 255, 255};
            FontManager::instance().renderText(renderer, "default", hs.name, 
                                              rect.x + 2, rect.y + 2, labelColor);
        }
    }
}

void RoomPanel::renderPropertyPanel(SDL_Renderer* renderer) {
    int panelX = PREVIEW_X + PREVIEW_WIDTH + 10;
    int panelY = PREVIEW_Y;
    
    SDL_Color cyan = {100, 255, 255, 255};
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 100, 255};
    
    FontManager::instance().renderText(renderer, "default", "[Properties]", panelX, panelY, cyan);
    
    if (!m_currentRoom) {
        FontManager::instance().renderText(renderer, "default", "No room selected", panelX, panelY + 20, white);
        return;
    }
    
    int y = panelY + 20;
    
    // Room info
    FontManager::instance().renderText(renderer, "default", "Room: " + m_currentRoom->name, panelX, y, yellow);
    y += 15;
    FontManager::instance().renderText(renderer, "default", "ID: " + m_currentRoom->id, panelX, y, white);
    y += 15;
    FontManager::instance().renderText(renderer, "default", 
        "Hotspots: " + std::to_string(m_currentRoom->hotspots.size()), panelX, y, white);
    y += 25;
    
    // Selected hotspot info
    if (m_selectedHotspotIndex >= 0 && m_selectedHotspotIndex < static_cast<int>(m_currentRoom->hotspots.size())) {
        const auto& hs = m_currentRoom->hotspots[m_selectedHotspotIndex];
        
        FontManager::instance().renderText(renderer, "default", "[Hotspot]", panelX, y, cyan);
        y += 15;
        FontManager::instance().renderText(renderer, "default", "Name: " + hs.name, panelX, y, white);
        y += 15;
        FontManager::instance().renderText(renderer, "default", "Type: " + hs.type, panelX, y, white);
        y += 15;
        FontManager::instance().renderText(renderer, "default", 
            "Pos: " + std::to_string(hs.x) + "," + std::to_string(hs.y), panelX, y, white);
        y += 15;
        FontManager::instance().renderText(renderer, "default",
            "Size: " + std::to_string(hs.w) + "x" + std::to_string(hs.h), panelX, y, white);
    }
}

void RoomPanel::handlePreviewClick(int x, int y, int button) {
    if (!m_currentRoom) return;
    
    int roomX, roomY;
    screenToRoom(x, y, roomX, roomY);
    
    if (button == SDL_BUTTON_LEFT) {
        if (m_currentTool == RoomEditorTool::Create) {
            // Create new hotspot
            HotspotData newHotspot;
            newHotspot.id = "new_hotspot_" + std::to_string(m_currentRoom->hotspots.size());
            newHotspot.name = "New Hotspot";
            newHotspot.type = "examine";
            newHotspot.x = roomX;
            newHotspot.y = roomY;
            newHotspot.w = 50;
            newHotspot.h = 50;
            
            auto cmd = std::make_unique<AddHotspotCommand>(m_currentRoom->id, newHotspot);
            EditorCore::instance().executeCommand(std::move(cmd));
            
            m_selectedHotspotIndex = static_cast<int>(m_currentRoom->hotspots.size()) - 1;
            AudioManager::instance().playSound("select");
        } else {
            // Select or start dragging
            int hitIndex = getHotspotAtPosition(x, y);
            
            if (hitIndex >= 0) {
                selectHotspot(hitIndex);
                
                const auto& hs = m_currentRoom->hotspots[hitIndex];
                m_isDragging = true;
                m_dragStartX = x;
                m_dragStartY = y;
                m_hotspotStartX = hs.x;
                m_hotspotStartY = hs.y;
                m_hotspotStartW = hs.w;
                m_hotspotStartH = hs.h;
            } else {
                selectHotspot(-1);
            }
        }
    } else if (button == SDL_BUTTON_RIGHT) {
        // Context menu or quick create
        m_currentTool = RoomEditorTool::Create;
    }
}

void RoomPanel::handlePreviewDrag(int x, int y) {
    if (!m_isDragging || !m_currentRoom || m_selectedHotspotIndex < 0) return;
    
    float scaleX = 640.0f / PREVIEW_WIDTH;
    float scaleY = 400.0f / PREVIEW_HEIGHT;
    
    int deltaX = static_cast<int>((x - m_dragStartX) * scaleX);
    int deltaY = static_cast<int>((y - m_dragStartY) * scaleY);
    
    auto& hs = m_currentRoom->hotspots[m_selectedHotspotIndex];
    hs.x = m_hotspotStartX + deltaX;
    hs.y = m_hotspotStartY + deltaY;
    
    // Snap to grid
    auto& ctx = EditorCore::instance().getContext();
    if (ctx.snapToGrid) {
        hs.x = (hs.x / ctx.gridSize) * ctx.gridSize;
        hs.y = (hs.y / ctx.gridSize) * ctx.gridSize;
    }
}

void RoomPanel::handlePreviewRelease(int x, int y) {
    (void)x; (void)y;
    
    if (m_isDragging && m_currentRoom && m_selectedHotspotIndex >= 0) {
        const auto& hs = m_currentRoom->hotspots[m_selectedHotspotIndex];
        
        // Create undo command if position changed
        if (hs.x != m_hotspotStartX || hs.y != m_hotspotStartY) {
            auto cmd = std::make_unique<MoveHotspotCommand>(
                m_currentRoom->id, m_selectedHotspotIndex,
                m_hotspotStartX, m_hotspotStartY, hs.x, hs.y);
            EditorCore::instance().executeCommand(std::move(cmd));
        }
    }
    
    m_isDragging = false;
    m_isResizing = false;
}

int RoomPanel::getHotspotAtPosition(int x, int y) {
    if (!m_currentRoom) return -1;
    
    float scaleX = static_cast<float>(PREVIEW_WIDTH) / 640.0f;
    float scaleY = static_cast<float>(PREVIEW_HEIGHT) / 400.0f;
    
    // Check in reverse order (top-most first)
    for (int i = static_cast<int>(m_currentRoom->hotspots.size()) - 1; i >= 0; i--) {
        const auto& hs = m_currentRoom->hotspots[i];
        
        SDL_Rect rect = {
            PREVIEW_X + static_cast<int>(hs.x * scaleX),
            PREVIEW_Y + static_cast<int>(hs.y * scaleY),
            static_cast<int>(hs.w * scaleX),
            static_cast<int>(hs.h * scaleY)
        };
        
        SDL_Point point = {x, y};
        if (SDL_PointInRect(&point, &rect)) {
            return i;
        }
    }
    
    return -1;
}

void RoomPanel::screenToRoom(int sx, int sy, int& rx, int& ry) {
    float scaleX = 640.0f / PREVIEW_WIDTH;
    float scaleY = 400.0f / PREVIEW_HEIGHT;
    
    rx = static_cast<int>((sx - PREVIEW_X) * scaleX);
    ry = static_cast<int>((sy - PREVIEW_Y) * scaleY);
}

void RoomPanel::roomToScreen(int rx, int ry, int& sx, int& sy) {
    float scaleX = static_cast<float>(PREVIEW_WIDTH) / 640.0f;
    float scaleY = static_cast<float>(PREVIEW_HEIGHT) / 400.0f;
    
    sx = PREVIEW_X + static_cast<int>(rx * scaleX);
    sy = PREVIEW_Y + static_cast<int>(ry * scaleY);
}

void RoomPanel::loadRoomTexture(SDL_Renderer* renderer) {
    if (m_roomTexture) {
        SDL_DestroyTexture(m_roomTexture);
        m_roomTexture = nullptr;
    }
    
    if (!m_currentRoom) return;
    
    std::string imagePath;
    if (!m_currentRoom->layers.empty()) {
        imagePath = m_currentRoom->layers[0].image;
    } else if (!m_currentRoom->background.empty()) {
        imagePath = m_currentRoom->background;
    }
    
    if (!imagePath.empty()) {
        m_roomTexture = IMG_LoadTexture(renderer, imagePath.c_str());
        if (!m_roomTexture) {
            LOG_WARNING("Failed to load room texture: " + imagePath);
        }
    }
}

void RoomPanel::selectRoom(const std::string& roomId) {
    auto& ctx = EditorCore::instance().getContext();
    
    m_currentRoom = nullptr;
    for (auto& room : ctx.rooms) {
        if (room.id == roomId) {
            m_currentRoom = &room;
            break;
        }
    }
    
    m_selectedHotspotIndex = -1;
    loadRoomTexture(EditorCore::instance().getRenderer());
    
    ctx.setSelection(SelectionType::Room, roomId);
}

void RoomPanel::selectHotspot(int index) {
    m_selectedHotspotIndex = index;
    
    if (m_currentRoom && index >= 0) {
        EditorCore::instance().getContext().setSelection(
            SelectionType::Hotspot, m_currentRoom->id, index);
    }
    
    AudioManager::instance().playSound("select");
}

