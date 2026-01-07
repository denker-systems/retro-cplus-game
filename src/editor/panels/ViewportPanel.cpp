/**
 * @file ViewportPanel.cpp
 * @brief Implementation of viewport panel
 */
#include "ViewportPanel.h"
#include "editor/EditorContext.h"
#include "engine/world/World.h"
#include "engine/world/Level.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/SpriteComponent.h"
#include <SDL_image.h>
#include <iostream>
#include <cmath>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

// ============================================================================
// Constructor / Destructor
// ============================================================================

ViewportPanel::ViewportPanel(EditorContext& context)
    : m_context(context)
{
}

ViewportPanel::~ViewportPanel() {
    clearTextureCache();
}

// ============================================================================
// IEditorPanel Interface
// ============================================================================

void ViewportPanel::update(float deltaTime) {
    // Update tool context
    m_toolContext.scene = m_scene;
    m_toolContext.selectedActor = m_selectedActor;
    m_toolContext.renderer = m_renderer;
    m_toolContext.zoom = m_zoom;
    m_toolContext.panX = m_panX;
    m_toolContext.panY = m_panY;
    m_toolContext.snapToGrid = m_snapToGrid;
    m_toolContext.gridSize = m_gridSize;
}

void ViewportPanel::render() {
#ifdef HAS_IMGUI
    if (!ImGui::Begin(m_title.c_str())) {
        ImGui::End();
        return;
    }
    
    renderBreadcrumbs();
    renderToolbar();
    
    ImGui::Separator();
    
    renderViewModeContent();
    
    ImGui::End();
#endif
}

// ============================================================================
// Hierarchy Navigation
// ============================================================================

void ViewportPanel::setWorld(engine::World* world) {
    m_world = world;
    m_level = nullptr;
    m_scene = nullptr;
    m_viewMode = ViewMode::World;
    m_selectedActor = nullptr;
}

void ViewportPanel::setLevel(engine::Level* level) {
    m_level = level;
    m_scene = nullptr;
    m_viewMode = ViewMode::Level;
    m_selectedActor = nullptr;
}

void ViewportPanel::setScene(engine::Scene* scene) {
    m_scene = scene;
    m_viewMode = ViewMode::Scene;
    m_selectedActor = nullptr;
}

// ============================================================================
// Rendering - Main
// ============================================================================

void ViewportPanel::renderToolbar() {
#ifdef HAS_IMGUI
    // Tool buttons
    m_toolManager.renderToolbar();
    
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    
    // Zoom controls
    ImGui::Text("Zoom:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(60);
    int zoomPct = static_cast<int>(m_zoom * 100);
    if (ImGui::InputInt("##zoom", &zoomPct, 0, 0)) {
        m_zoom = std::clamp(zoomPct / 100.0f, 0.25f, 4.0f);
    }
    ImGui::SameLine();
    ImGui::Text("%%");
    
    ImGui::SameLine();
    ImGui::Text("|");
    ImGui::SameLine();
    
    // View options
    ImGui::Checkbox("Grid", &m_showGrid);
    ImGui::SameLine();
    ImGui::Checkbox("Hotspots", &m_showHotspots);
    ImGui::SameLine();
    ImGui::Checkbox("Walk Area", &m_showWalkArea);
#endif
}

void ViewportPanel::renderBreadcrumbs() {
#ifdef HAS_IMGUI
    if (m_world) {
        if (ImGui::Button("? World")) {
            setWorld(m_world);
        }
    }
    
    if (m_level) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        std::string levelLabel = "? " + m_level->getName();
        if (ImGui::Button(levelLabel.c_str())) {
            setLevel(m_level);
        }
    }
    
    if (m_scene) {
        ImGui::SameLine();
        ImGui::Text(">");
        ImGui::SameLine();
        std::string sceneLabel = "? " + m_scene->getName();
        ImGui::Button(sceneLabel.c_str());
    }
#endif
}

void ViewportPanel::renderViewModeContent() {
#ifdef HAS_IMGUI
    switch (m_viewMode) {
        case ViewMode::World:
            renderWorldView();
            break;
        case ViewMode::Level:
            renderLevelView();
            break;
        case ViewMode::Scene:
            renderSceneView();
            break;
    }
#endif
}

// ============================================================================
// View Rendering
// ============================================================================

void ViewportPanel::renderWorldView() {
#ifdef HAS_IMGUI
    if (!m_world) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No World loaded");
        return;
    }
    
    ImGui::Text("World: %s", m_world->getName().c_str());
    ImGui::Text("Levels: %zu", m_world->getLevels().size());
    
    // List levels
    for (auto& level : m_world->getLevels()) {
        if (ImGui::Selectable(level->getName().c_str())) {
            setLevel(level.get());
        }
    }
#endif
}

void ViewportPanel::renderLevelView() {
#ifdef HAS_IMGUI
    if (!m_level) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Level selected");
        return;
    }
    
    ImGui::Text("Level: %s", m_level->getName().c_str());
    ImGui::Text("Scenes: %zu", m_level->getScenes().size());
    
    // List scenes
    for (auto& scene : m_level->getScenes()) {
        if (ImGui::Selectable(scene->getName().c_str())) {
            setScene(scene.get());
        }
    }
#endif
}

void ViewportPanel::renderSceneView() {
#ifdef HAS_IMGUI
    if (!m_scene) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Scene selected");
        return;
    }
    
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    
    // Room size in viewport
    ImVec2 roomSize(640.0f * m_zoom, 400.0f * m_zoom);
    
    // Center room in viewport
    ImVec2 renderPos = cursorPos;
    if (roomSize.x < contentSize.x) {
        renderPos.x += (contentSize.x - roomSize.x) / 2.0f;
    }
    if (roomSize.y < contentSize.y) {
        renderPos.y += (contentSize.y - roomSize.y) / 2.0f;
    }
    
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Background
    drawList->AddRectFilled(renderPos, 
        ImVec2(renderPos.x + roomSize.x, renderPos.y + roomSize.y),
        IM_COL32(40, 40, 50, 255));
    
    // Grid
    if (m_showGrid) {
        renderSceneGrid(drawList, renderPos, roomSize);
    }
    
    // Border
    drawList->AddRect(renderPos,
        ImVec2(renderPos.x + roomSize.x, renderPos.y + roomSize.y),
        IM_COL32(100, 100, 100, 255), 0, 0, 1.0f);
    
    // Update tool context viewport info
    m_toolContext.viewportX = renderPos.x + m_panX;
    m_toolContext.viewportY = renderPos.y + m_panY;
    m_toolContext.viewportW = roomSize.x;
    m_toolContext.viewportH = roomSize.y;
    
    // Actors
    ImVec2 actorOffset(renderPos.x + m_panX, renderPos.y + m_panY);
    renderSceneActors(drawList, actorOffset);
    
    // Tool overlay
    m_toolManager.renderOverlay(drawList, m_toolContext);
    
    // Interactive area
    ImGui::SetCursorScreenPos(cursorPos);
    ImGui::InvisibleButton("scene_canvas", contentSize);
    
    // Handle drag-drop
    handleDragDrop(renderPos);
    
    // Handle input
    handleInput(renderPos, roomSize);
    
    // Info overlay
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 10, cursorPos.y + 10));
    ImGui::BeginGroup();
    ImGui::Text("Scene: %s", m_scene->getName().c_str());
    ImGui::Text("Actors: %zu", m_scene->getActors().size());
    if (m_selectedActor) {
        ImGui::Text("Selected: %s", m_selectedActor->getName().c_str());
    }
    ImGui::Text("Zoom: %.0f%% | Tool: %s", m_zoom * 100, 
                m_toolManager.getActiveTool() ? m_toolManager.getActiveTool()->getName() : "None");
    ImGui::EndGroup();
#endif
}

// ============================================================================
// Scene Rendering Helpers
// ============================================================================

void ViewportPanel::renderSceneActors(ImDrawList* drawList, ImVec2 offset) {
#ifdef HAS_IMGUI
    if (!m_scene) return;
    
    for (const auto& actor : m_scene->getActors()) {
        if (!actor || !actor->isActive()) continue;
        
        engine::Vec2 pos = actor->getPosition();
        float worldX = offset.x + pos.x * m_zoom;
        float worldY = offset.y + pos.y * m_zoom;
        std::string name = actor->getName();
        
        // Get sprite info
        auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
        SDL_Texture* tex = nullptr;
        int texW = 64, texH = 64;
        
        if (spriteComp) {
            tex = spriteComp->getTexture();
            if (tex) {
                texW = spriteComp->getWidth();
                texH = spriteComp->getHeight();
            } else {
                // Try load on demand
                std::string texPath = spriteComp->getTexturePath();
                if (!texPath.empty()) {
                    tex = loadTexture(texPath);
                    if (tex) {
                        SDL_QueryTexture(tex, nullptr, nullptr, &texW, &texH);
                        spriteComp->setTexture(tex);
                        spriteComp->setSize(texW, texH);
                    }
                }
            }
        }
        
        // Render sprite or debug box
        if (tex) {
            if (name == "Background") {
                // Background fills room
                float roomW = 640.0f * m_zoom;
                float roomH = 400.0f * m_zoom;
                float scaleX = roomW / texW;
                float scaleY = roomH / texH;
                float scale = std::min(scaleX, scaleY);
                float scaledW = texW * scale;
                float scaledH = texH * scale;
                float bgOffsetX = (roomW - scaledW) / 2.0f;
                float bgOffsetY = (roomH - scaledH) / 2.0f;
                
                drawList->AddImage(
                    (ImTextureID)(intptr_t)tex,
                    ImVec2(worldX + bgOffsetX, worldY + bgOffsetY),
                    ImVec2(worldX + bgOffsetX + scaledW, worldY + bgOffsetY + scaledH));
            } else {
                drawList->AddImage(
                    (ImTextureID)(intptr_t)tex,
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + texW * m_zoom, worldY + texH * m_zoom));
            }
        } else {
            // Debug visualization
            ImU32 color = IM_COL32(100, 255, 255, 255);
            if (name == "PlayerSpawn") {
                color = IM_COL32(255, 0, 255, 255);
                drawList->AddCircleFilled(ImVec2(worldX, worldY), 8.0f * m_zoom, color);
            } else {
                drawList->AddRect(
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + texW * m_zoom, worldY + texH * m_zoom),
                    color, 0, 0, 2.0f);
                drawList->AddText(ImVec2(worldX + 5, worldY + 5), 
                    IM_COL32(255, 255, 255, 255), name.c_str());
            }
        }
    }
#endif
}

void ViewportPanel::renderSceneGrid(ImDrawList* drawList, ImVec2 offset, ImVec2 size) {
#ifdef HAS_IMGUI
    float gridSize = m_gridSize * m_zoom;
    ImU32 gridColor = IM_COL32(60, 60, 70, 100);
    
    // Vertical lines
    for (float x = 0; x <= size.x; x += gridSize) {
        drawList->AddLine(
            ImVec2(offset.x + x, offset.y),
            ImVec2(offset.x + x, offset.y + size.y),
            gridColor);
    }
    
    // Horizontal lines
    for (float y = 0; y <= size.y; y += gridSize) {
        drawList->AddLine(
            ImVec2(offset.x, offset.y + y),
            ImVec2(offset.x + size.x, offset.y + y),
            gridColor);
    }
#endif
}

// ============================================================================
// Input Handling
// ============================================================================

void ViewportPanel::handleInput(ImVec2 renderPos, ImVec2 roomSize) {
#ifdef HAS_IMGUI
    if (!ImGui::IsItemHovered() && !m_toolManager.getActiveTool()->isActive()) {
        return;
    }
    
    ImVec2 mousePos = ImGui::GetMousePos();
    float roomX = (mousePos.x - renderPos.x - m_panX) / m_zoom;
    float roomY = (mousePos.y - renderPos.y - m_panY) / m_zoom;
    
    // Mouse down
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        if (!m_toolManager.onMouseDown(roomX, roomY, m_toolContext)) {
            // Tool didn't handle - try select actor
            m_selectedActor = nullptr;
            for (auto& actor : m_scene->getActors()) {
                engine::Vec2 pos = actor->getPosition();
                float aw = 64.0f, ah = 64.0f;
                auto* sprite = actor->getComponent<engine::SpriteComponent>();
                if (sprite) {
                    aw = sprite->getWidth();
                    ah = sprite->getHeight();
                }
                if (roomX >= pos.x && roomX <= pos.x + aw &&
                    roomY >= pos.y && roomY <= pos.y + ah) {
                    m_selectedActor = actor.get();
                    m_toolContext.selectedActor = m_selectedActor;
                    m_toolManager.onMouseDown(roomX, roomY, m_toolContext);
                    break;
                }
            }
        }
    }
    
    // Mouse drag
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        m_toolManager.onMouseDrag(roomX, roomY, delta.x / m_zoom, delta.y / m_zoom, m_toolContext);
    }
    
    // Mouse up
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        m_toolManager.onMouseUp(roomX, roomY, m_toolContext);
    }
    
    // Middle mouse pan
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
        ImVec2 delta = ImGui::GetIO().MouseDelta;
        m_panX += delta.x;
        m_panY += delta.y;
    }
    
    // Mouse wheel zoom
    float wheel = ImGui::GetIO().MouseWheel;
    if (wheel != 0.0f) {
        m_zoom = std::clamp(m_zoom + wheel * 0.1f, 0.25f, 4.0f);
    }
    
    // Keyboard shortcuts
    if (ImGui::IsKeyPressed(ImGuiKey_S)) {
        m_toolManager.setActiveTool(ToolType::Select);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_R)) {
        m_toolManager.setActiveTool(ToolType::Scale);
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        m_selectedActor = nullptr;
        m_toolContext.selectedActor = nullptr;
    }
#endif
}

void ViewportPanel::handleDragDrop(ImVec2 renderPos) {
#ifdef HAS_IMGUI
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
            const char* assetPath = (const char*)payload->Data;
            
            ImVec2 mousePos = ImGui::GetMousePos();
            float dropX = (mousePos.x - renderPos.x - m_panX) / m_zoom;
            float dropY = (mousePos.y - renderPos.y - m_panY) / m_zoom;
            
            if (m_snapToGrid) {
                dropX = round(dropX / m_gridSize) * m_gridSize;
                dropY = round(dropY / m_gridSize) * m_gridSize;
            }
            
            // Create actor name from file
            std::string path(assetPath);
            std::string actorName = path.substr(path.find_last_of("/\\") + 1);
            actorName = actorName.substr(0, actorName.find_last_of('.'));
            
            // Make unique
            int suffix = 1;
            std::string uniqueName = actorName;
            while (m_scene->findActor(uniqueName)) {
                uniqueName = actorName + "_" + std::to_string(suffix++);
            }
            
            // Create actor
            auto newActor = std::make_unique<engine::ActorObjectExtended>(uniqueName);
            newActor->setPosition(dropX, dropY);
            
            auto* spriteComp = newActor->addComponent<engine::SpriteComponent>();
            SDL_Texture* tex = loadTexture(assetPath);
            if (tex) {
                spriteComp->setTexture(tex);
                int w, h;
                SDL_QueryTexture(tex, nullptr, nullptr, &w, &h);
                spriteComp->setSize(w, h);
                spriteComp->setSourceRect({0, 0, w, h});
            }
            
            m_selectedActor = newActor.get();
            m_toolContext.selectedActor = m_selectedActor;
            m_scene->addActor(std::move(newActor));
            
            std::cout << "[Editor] Dropped: " << uniqueName << " at (" << dropX << ", " << dropY << ")" << std::endl;
        }
        ImGui::EndDragDropTarget();
    }
#endif
}

// ============================================================================
// Texture Management
// ============================================================================

SDL_Texture* ViewportPanel::loadTexture(const std::string& path) {
    if (!m_renderer || path.empty()) return nullptr;
    
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) {
        return it->second;
    }
    
    SDL_Texture* tex = IMG_LoadTexture(m_renderer, path.c_str());
    if (tex) {
        m_textureCache[path] = tex;
    }
    return tex;
}

void ViewportPanel::clearTextureCache() {
    for (auto& [path, tex] : m_textureCache) {
        if (tex) {
            SDL_DestroyTexture(tex);
        }
    }
    m_textureCache.clear();
}
