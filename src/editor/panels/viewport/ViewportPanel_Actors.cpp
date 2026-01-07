/**
 * @file ViewportPanel_Actors.cpp
 * @brief ViewportPanel actor rendering
 */
#include "ViewportPanel.h"
#include "engine/world/Scene.h"
#include "engine/components/SpriteComponent.h"
#include "engine/core/ActorObjectExtended.h"
#include <algorithm>
#include <cmath>
#include <string>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

void ViewportPanel::renderSceneActors(ImDrawList* drawList, ImVec2 offset) {
#ifdef HAS_IMGUI
    if (!m_scene || !drawList) return;
    
    const auto& actors = m_scene->getActors();
    
    for (const auto& actor : actors) {
        if (!actor || !actor->isActive()) continue;
        
        // Get actor transform
        engine::Vec2 pos = actor->getPosition();
        float worldX = offset.x + pos.x * m_zoom;
        float worldY = offset.y + pos.y * m_zoom;
        
        // Try to get SpriteComponent
        auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
        if (spriteComp) {
            SDL_Texture* tex = spriteComp->getTexture();
            if (tex) {
                int w = spriteComp->getWidth();
                int h = spriteComp->getHeight();
                
                // Special handling for background images
                if (actor->getName() == "Background") {
                    // Calculate aspect ratio
                    float aspectRatio = (float)w / (float)h;
                    
                    // Get dynamic room size from Scene
                    const auto& sceneGrid = m_scene->getGridPosition();
                    float roomWidth = static_cast<float>(sceneGrid.pixelWidth) * m_zoom;
                    float roomHeight = static_cast<float>(sceneGrid.pixelHeight) * m_zoom;
                    
                    float scaleX = roomWidth / w;
                    float scaleY = roomHeight / h;
                    float scale = std::min(scaleX, scaleY);
                    
                    float scaledW = w * scale;
                    float scaledH = h * scale;
                    
                    // Center the background
                    float offsetX = (roomWidth - scaledW) / 2.0f;
                    float offsetY = (roomHeight - scaledH) / 2.0f;
                    
                    // Render centered background
                    drawList->AddImage(
                        (ImTextureID)(intptr_t)tex,
                        ImVec2(worldX + offsetX, worldY + offsetY),
                        ImVec2(worldX + offsetX + scaledW, worldY + offsetY + scaledH)
                    );
                } else {
                    // Normal rendering for other sprites
                    drawList->AddImage(
                        (ImTextureID)(intptr_t)tex,
                        ImVec2(worldX, worldY),
                        ImVec2(worldX + w * m_zoom, worldY + h * m_zoom)
                    );
                }
            }
        }
        
        // Debug visualization for actors without sprites
        if (!spriteComp) {
            std::string name = actor->getName();
            
            // Different colors for different actor types
            ImU32 color = IM_COL32(200, 200, 200, 255);
            
            if (name == "Player") {
                color = IM_COL32(255, 100, 100, 255);
                // Draw player as a filled rectangle with border
                drawList->AddRectFilled(
                    ImVec2(worldX - 16 * m_zoom, worldY - 24 * m_zoom),
                    ImVec2(worldX + 16 * m_zoom, worldY + 24 * m_zoom),
                    color
                );
                drawList->AddRect(
                    ImVec2(worldX - 16 * m_zoom, worldY - 24 * m_zoom),
                    ImVec2(worldX + 16 * m_zoom, worldY + 24 * m_zoom),
                    IM_COL32(255, 255, 255, 255), 0, 0, 2.0f
                );
                drawList->AddText(ImVec2(worldX - 20, worldY + 30 * m_zoom), 
                                IM_COL32(255, 255, 255, 255), 
                                "Player");
            }
            else if (name == "PlayerSpawn") {
                color = IM_COL32(255, 0, 255, 255);
                drawList->AddCircleFilled(ImVec2(worldX, worldY), 8.0f * m_zoom, color);
                drawList->AddText(ImVec2(worldX + 10, worldY - 8), color, "Spawn");
            }
            else if (name == "WalkArea") {
                color = IM_COL32(100, 255, 100, 200);
                // Draw box around dynamic room size
                const auto& sceneGrid = m_scene->getGridPosition();
                drawList->AddRect(
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + sceneGrid.pixelWidth * m_zoom, worldY + sceneGrid.pixelHeight * m_zoom),
                    color, 0, 0, 2.0f
                );
            }
            else {
                // Interactive actors (hotspots)
                color = IM_COL32(100, 255, 255, 255);
                drawList->AddRect(
                    ImVec2(worldX, worldY),
                    ImVec2(worldX + 64 * m_zoom, worldY + 64 * m_zoom),
                    color, 0, 0, 2.0f
                );
                drawList->AddText(ImVec2(worldX + 5, worldY + 5), 
                                IM_COL32(255, 255, 255, 255), 
                                name.c_str());
            }
        }
        
        // Draw selection highlight
        if (actor.get() == m_selectedActor) {
            engine::Vec2 pos = actor->getPosition();
            float ax = offset.x + pos.x * m_zoom;
            float ay = offset.y + pos.y * m_zoom;
            float aw = 64.0f * m_zoom;
            float ah = 64.0f * m_zoom;
            
            auto* sprite = actor->getComponent<engine::SpriteComponent>();
            if (sprite) {
                aw = sprite->getWidth() * m_zoom;
                ah = sprite->getHeight() * m_zoom;
            }
            
            // Selection border
            ImU32 selColor = m_draggedActor == actor.get() ? 
                             IM_COL32(255, 200, 100, 255) : 
                             IM_COL32(100, 200, 255, 255);
            drawList->AddRect(
                ImVec2(ax - 2, ay - 2),
                ImVec2(ax + aw + 2, ay + ah + 2),
                selColor, 0, 0, 3.0f);
            
            // Corner handles
            float handleSize = 6.0f;
            ImU32 handleColor = IM_COL32(255, 255, 255, 255);
            drawList->AddRectFilled(ImVec2(ax - handleSize/2, ay - handleSize/2), 
                                   ImVec2(ax + handleSize/2, ay + handleSize/2), handleColor);
            drawList->AddRectFilled(ImVec2(ax + aw - handleSize/2, ay - handleSize/2), 
                                   ImVec2(ax + aw + handleSize/2, ay + handleSize/2), handleColor);
            drawList->AddRectFilled(ImVec2(ax - handleSize/2, ay + ah - handleSize/2), 
                                   ImVec2(ax + handleSize/2, ay + ah + handleSize/2), handleColor);
            drawList->AddRectFilled(ImVec2(ax + aw - handleSize/2, ay + ah - handleSize/2), 
                                   ImVec2(ax + aw + handleSize/2, ay + ah + handleSize/2), handleColor);
        }
    }
#endif
}
