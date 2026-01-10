/**
 * @file ViewportActorRenderer.cpp
 * @brief Implementation of actor rendering
 */
#include "ViewportActorRenderer.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/SpriteComponent.h"
// 2D components removed - using 3D physics only
#include "editor/properties/actors/LockableComponent.h"
#include "editor/core/ImGuiManager.h"
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

void ViewportActorRenderer::renderActors(ImDrawList* drawList, engine::Scene* scene, 
                                         const RenderContext& ctx, float offsetX, float offsetY,
                                         engine::ActorObjectExtended* selectedActor,
                                         engine::ActorObjectExtended* draggedActor) {
    if (!scene || !drawList) return;
    
    // Sort actors by render order
    std::vector<engine::ActorObjectExtended*> sortedActors;
    for (const auto& actor : scene->getActors()) {
        if (actor && actor->isActive()) {
            sortedActors.push_back(actor.get());
        }
    }
    
    std::sort(sortedActors.begin(), sortedActors.end(),
              [](const engine::ActorObjectExtended* a, const engine::ActorObjectExtended* b) {
                  return a->getRenderOrder() < b->getRenderOrder();
              });
    
    // Render each actor
    for (auto* actor : sortedActors) {
        engine::Vec2 pos = actor->getPosition();
        float worldX = offsetX + pos.x * ctx.zoom;
        float worldY = offsetY + pos.y * ctx.zoom;
        
        auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
        if (spriteComp) {
            renderActorSprite(drawList, actor, worldX, worldY, ctx.zoom, scene);
        } else {
            renderActorPlaceholder(drawList, actor, worldX, worldY, ctx.zoom, scene);
        }
        
        if (actor == selectedActor) {
            renderActorSelection(drawList, actor, worldX, worldY, ctx.zoom, actor == draggedActor);
        }
        
        renderActorLockIcon(drawList, actor, worldX, worldY);
        
        if (ctx.showPhysicsDebug) {
            renderActorPhysicsDebug(drawList, actor, worldX, worldY, ctx.zoom);
        }
    }
}

void ViewportActorRenderer::renderActorSprite(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                              float worldX, float worldY, float zoom, engine::Scene* scene) {
    auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
    if (!spriteComp) return;
    
    ImTextureID texID = 0;
    int w = 0, h = 0;
    
    if (ImGuiManager::instance().isUsingOpenGL()) {
        unsigned int glTexID = spriteComp->getGLTextureID();
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
        // Special handling for background actors
        if (actor->getName() == "Background" && scene) {
            const auto& sceneGrid = scene->getGridPosition();
            float roomWidth = sceneGrid.pixelWidth * zoom;
            float roomHeight = sceneGrid.pixelHeight * zoom;
            float scaleX = roomWidth / w;
            float scaleY = roomHeight / h;
            float scale = std::min(scaleX, scaleY);
            float scaledW = w * scale;
            float scaledH = h * scale;
            float bgOffsetX = (roomWidth - scaledW) / 2.0f;
            float bgOffsetY = (roomHeight - scaledH) / 2.0f;
            drawList->AddImage(texID, ImVec2(worldX + bgOffsetX, worldY + bgOffsetY),
                              ImVec2(worldX + bgOffsetX + scaledW, worldY + bgOffsetY + scaledH));
        } else {
            drawList->AddImage(texID, ImVec2(worldX, worldY),
                              ImVec2(worldX + w * zoom, worldY + h * zoom));
        }
    }
}

void ViewportActorRenderer::renderActorPlaceholder(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                                   float worldX, float worldY, float zoom, engine::Scene* scene) {
    std::string name = actor->getName();
    ImU32 color = IM_COL32(200, 200, 200, 255);
    
    if (name == "Player") {
        color = IM_COL32(255, 100, 100, 255);
        drawList->AddRectFilled(ImVec2(worldX - 16 * zoom, worldY - 24 * zoom),
                               ImVec2(worldX + 16 * zoom, worldY + 24 * zoom), color);
        drawList->AddRect(ImVec2(worldX - 16 * zoom, worldY - 24 * zoom),
                         ImVec2(worldX + 16 * zoom, worldY + 24 * zoom),
                         IM_COL32(255, 255, 255, 255), 0, 0, 2.0f);
        drawList->AddText(ImVec2(worldX - 20, worldY + 30 * zoom), 
                         IM_COL32(255, 255, 255, 255), "Player");
    }
    else if (name == "PlayerSpawn") {
        color = IM_COL32(255, 0, 255, 255);
        drawList->AddCircleFilled(ImVec2(worldX, worldY), 8.0f * zoom, color);
        drawList->AddText(ImVec2(worldX + 10, worldY - 8), color, "Spawn");
    }
    else if (name == "WalkArea" && scene) {
        color = IM_COL32(100, 255, 100, 200);
        const auto& sceneGrid = scene->getGridPosition();
        drawList->AddRect(ImVec2(worldX, worldY),
                         ImVec2(worldX + sceneGrid.pixelWidth * zoom, worldY + sceneGrid.pixelHeight * zoom),
                         color, 0, 0, 2.0f);
    }
    else {
        color = IM_COL32(100, 255, 255, 255);
        drawList->AddRect(ImVec2(worldX, worldY), ImVec2(worldX + 64 * zoom, worldY + 64 * zoom),
                         color, 0, 0, 2.0f);
        drawList->AddText(ImVec2(worldX + 5, worldY + 5), 
                         IM_COL32(255, 255, 255, 255), name.c_str());
    }
}

void ViewportActorRenderer::renderActorSelection(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                                 float worldX, float worldY, float zoom, bool isDragged) {
    float aw = 64.0f, ah = 64.0f;
    auto* sprite = actor->getComponent<engine::SpriteComponent>();
    if (sprite) {
        aw = static_cast<float>(sprite->getWidth());
        ah = static_cast<float>(sprite->getHeight());
    }
    
    ImU32 selColor = isDragged ? IM_COL32(255, 200, 100, 255) : IM_COL32(100, 200, 255, 255);
    drawList->AddRect(ImVec2(worldX - 2, worldY - 2),
                     ImVec2(worldX + aw * zoom + 2, worldY + ah * zoom + 2),
                     selColor, 0, 0, 3.0f);
    
    // Corner handles
    float handleSize = 6.0f;
    ImU32 handleColor = IM_COL32(255, 255, 255, 255);
    drawList->AddRectFilled(ImVec2(worldX - handleSize/2, worldY - handleSize/2),
                           ImVec2(worldX + handleSize/2, worldY + handleSize/2), handleColor);
    drawList->AddRectFilled(ImVec2(worldX + aw * zoom - handleSize/2, worldY - handleSize/2),
                           ImVec2(worldX + aw * zoom + handleSize/2, worldY + handleSize/2), handleColor);
    drawList->AddRectFilled(ImVec2(worldX - handleSize/2, worldY + ah * zoom - handleSize/2),
                           ImVec2(worldX + handleSize/2, worldY + ah * zoom + handleSize/2), handleColor);
    drawList->AddRectFilled(ImVec2(worldX + aw * zoom - handleSize/2, worldY + ah * zoom - handleSize/2),
                           ImVec2(worldX + aw * zoom + handleSize/2, worldY + ah * zoom + handleSize/2), handleColor);
}

void ViewportActorRenderer::renderActorLockIcon(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                                float worldX, float worldY) {
    auto* lockable = actor->getComponent<engine::LockableComponent>();
    if (lockable && lockable->isLocked()) {
        drawList->AddText(ImVec2(worldX + 2, worldY + 2), IM_COL32(255, 100, 100, 255), "LOCKED");
    }
}

void ViewportActorRenderer::renderActorPhysicsDebug(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                                    float worldX, float worldY, float zoom) {
    // 2D collider debug removed - using 3D physics visualization
    // TODO: Add 3D physics debug visualization if needed
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
