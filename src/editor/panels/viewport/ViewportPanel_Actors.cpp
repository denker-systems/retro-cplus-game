/**
 * @file ViewportPanel_Actors.cpp
 * @brief ViewportPanel actor rendering
 */
#include "ViewportPanel.h"
#include "engine/world/Scene.h"
#include "engine/components/SpriteComponent.h"
#include "engine/components/Collider2DComponent.h"
#include "engine/components/RigidBody2DComponent.h"
#include "editor/properties/actors/LockableComponent.h"
#include "editor/core/ImGuiManager.h"
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
    
    // Sort actors by render order (z-index)
    std::vector<engine::ActorObjectExtended*> sortedActors;
    for (const auto& actor : actors) {
        if (actor && actor->isActive()) {
            sortedActors.push_back(actor.get());
        }
    }
    
    std::sort(sortedActors.begin(), sortedActors.end(), 
              [](const engine::ActorObjectExtended* a, const engine::ActorObjectExtended* b) {
                  return a->getRenderOrder() < b->getRenderOrder();
              });
    
    for (const auto& actor : sortedActors) {
        
        // Get actor transform
        engine::Vec2 pos = actor->getPosition();
        float worldX = offset.x + pos.x * m_zoom;
        float worldY = offset.y + pos.y * m_zoom;
        
        // Try to get SpriteComponent
        auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
        if (spriteComp) {
            const std::string& texturePath = spriteComp->getTexturePath();
            
            // Get appropriate texture ID based on backend
            ImTextureID texID = 0;
            int w = 0, h = 0;
            
            if (ImGuiManager::instance().isUsingOpenGL()) {
                // Use GLTextureManager for OpenGL textures
                unsigned int glTexID = spriteComp->getGLTextureID();
                if (glTexID == 0 && !texturePath.empty()) {
                    // Load GL texture if not already loaded
                    glTexID = const_cast<engine::SpriteComponent*>(spriteComp)->loadGLTexture(texturePath);
                }
                if (glTexID != 0) {
                    texID = (ImTextureID)(intptr_t)glTexID;
                    w = spriteComp->getWidth();
                    h = spriteComp->getHeight();
                }
            } else {
                SDL_Texture* tex = spriteComp->getTexture();
                if (tex) {
                    texID = (ImTextureID)(intptr_t)tex;
                    w = spriteComp->getWidth();
                    h = spriteComp->getHeight();
                }
            }
            
            if (texID && w > 0 && h > 0) {
                
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
                        texID,
                        ImVec2(worldX + offsetX, worldY + offsetY),
                        ImVec2(worldX + offsetX + scaledW, worldY + offsetY + scaledH)
                    );
                } else {
                    // Normal rendering for other sprites
                    drawList->AddImage(
                        texID,
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
        
        // Draw lock icon for ALL actors (shows lock state)
        {
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
            
            // Lock icon position (top-right corner)
            float lockSize = 16.0f;
            float lockX = ax + aw - lockSize - 2;
            float lockY = ay + 2;
            
            // Check if locked
            auto* lockable = actor->getComponent<engine::LockableComponent>();
            bool isLocked = lockable && lockable->isLocked();
            
            // Draw lock background
            drawList->AddRectFilled(
                ImVec2(lockX, lockY),
                ImVec2(lockX + lockSize, lockY + lockSize),
                isLocked ? IM_COL32(60, 40, 20, 220) : IM_COL32(40, 40, 40, 150)
            );
            
            if (isLocked) {
                // Draw LOCKED icon (closed padlock) - gold color
                ImU32 lockColor = IM_COL32(255, 200, 100, 255);
                // Lock body
                drawList->AddRectFilled(
                    ImVec2(lockX + 3, lockY + 8),
                    ImVec2(lockX + 13, lockY + 14),
                    lockColor
                );
                // Lock shackle (closed)
                drawList->AddRect(
                    ImVec2(lockX + 5, lockY + 3),
                    ImVec2(lockX + 11, lockY + 9),
                    lockColor, 2, 0, 2.0f
                );
            } else {
                // Draw UNLOCKED icon (open padlock) - gray color
                ImU32 unlockColor = IM_COL32(150, 150, 150, 200);
                // Lock body
                drawList->AddRect(
                    ImVec2(lockX + 3, lockY + 8),
                    ImVec2(lockX + 13, lockY + 14),
                    unlockColor, 0, 0, 1.5f
                );
                // Lock shackle (open - shifted up)
                drawList->AddRect(
                    ImVec2(lockX + 5, lockY + 1),
                    ImVec2(lockX + 11, lockY + 7),
                    unlockColor, 2, 0, 1.5f
                );
            }
        }
        
        // Draw selection highlight
        if (actor == m_selectedActor) {
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
            
            const ImU32 actorColor = (actor == m_selectedActor) 
                ? IM_COL32(255, 255, 0, 255)  // Yellow for selected
                : IM_COL32(0, 255, 0, 255);    // Green for normal
            
            // Selection border
            ImU32 selColor = m_draggedActor == actor ? 
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
        
        // Draw physics collider visualization
        auto* collider = actor->getComponent<engine::Collider2DComponent>();
        if (collider && m_showPhysicsDebug) {
            engine::Vec2 actorPos = actor->getPosition();
            float cx = offset.x + (actorPos.x + collider->getOffset().x) * m_zoom;
            float cy = offset.y + (actorPos.y + collider->getOffset().y) * m_zoom;
            glm::vec2 size = collider->getSize();
            float cw = size.x * m_zoom;
            float ch = size.y * m_zoom;
            
            // Different colors for different collider types
            ImU32 colliderColor;
            if (collider->isTrigger()) {
                colliderColor = IM_COL32(0, 255, 255, 150);  // Cyan for triggers
            } else {
                auto* rb = actor->getComponent<engine::RigidBody2DComponent>();
                if (rb) {
                    switch (rb->getBodyType()) {
                        case engine::RigidBody2DComponent::BodyType::Static:
                            colliderColor = IM_COL32(255, 100, 100, 150);  // Red for static
                            break;
                        case engine::RigidBody2DComponent::BodyType::Dynamic:
                            colliderColor = IM_COL32(100, 255, 100, 150);  // Green for dynamic
                            break;
                        case engine::RigidBody2DComponent::BodyType::Kinematic:
                            colliderColor = IM_COL32(255, 255, 100, 150);  // Yellow for kinematic
                            break;
                    }
                } else {
                    colliderColor = IM_COL32(200, 200, 200, 150);  // Gray default
                }
            }
            
            // Draw based on shape type
            auto shapeType = collider->getShapeType();
            if (shapeType == engine::Collider2DComponent::ShapeType::Circle) {
                float radius = size.x / 2.0f * m_zoom;
                drawList->AddCircle(ImVec2(cx + cw/2, cy + ch/2), radius, colliderColor, 16, 2.0f);
            } else if (shapeType == engine::Collider2DComponent::ShapeType::Capsule) {
                // Draw capsule as rounded rect
                float radius = size.x / 2.0f * m_zoom;
                drawList->AddRect(ImVec2(cx, cy), ImVec2(cx + cw, cy + ch), colliderColor, radius, 0, 2.0f);
            } else {
                // Box shape
                drawList->AddRect(ImVec2(cx, cy), ImVec2(cx + cw, cy + ch), colliderColor, 0, 0, 2.0f);
            }
            
            // Label for trigger type
            if (collider->isTrigger()) {
                drawList->AddText(ImVec2(cx, cy - 14), IM_COL32(0, 255, 255, 255), "TRIGGER");
            }
        }
    }
#endif
}
