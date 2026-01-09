/**
 * @file ViewportSceneRenderer.cpp
 * @brief Implementation of Scene-level viewport renderer
 */
#include "editor/viewport/ViewportSceneRenderer.h"
#include "editor/core/EditorContext.h"
#include "editor/core/SelectionManager.h"
#include "editor/core/ImGuiManager.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/SpriteComponent.h"
#include "engine/components/Collider2DComponent.h"
#include "engine/components/RigidBody2DComponent.h"
#include "editor/properties/actors/LockableComponent.h"
#include <algorithm>
#include <cmath>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {

void ViewportSceneRenderer::initialize(EditorContext* context, SelectionManager* selectionManager) {
    m_context = context;
    m_selectionManager = selectionManager;
}

void ViewportSceneRenderer::update(float deltaTime) {
}

void ViewportSceneRenderer::render(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    if (!m_scene || !drawList) return;
    
    renderBackground(drawList, ctx);
    
    if (m_showGrid) {
        renderGrid(drawList, ctx);
    }
    
    renderActors(drawList, ctx);
    
    if (m_showPhysicsDebug) {
        renderPhysicsDebug(drawList, ctx);
    }
    
    // Tool overlay
    m_toolContext.selectedActor = m_selectedActor;
    m_toolContext.zoom = ctx.zoom;
    m_toolContext.panX = ctx.panX;
    m_toolContext.panY = ctx.panY;
    m_toolContext.viewportX = ctx.viewportPos.x;
    m_toolContext.viewportY = ctx.viewportPos.y;
    m_toolManager.renderOverlay(drawList, m_toolContext);
    
    renderInfoOverlay(drawList, ctx);
}

void ViewportSceneRenderer::renderBackground(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    const auto& gridPos = m_scene->getGridPosition();
    float roomW = static_cast<float>(gridPos.pixelWidth) * ctx.zoom;
    float roomH = static_cast<float>(gridPos.pixelHeight) * ctx.zoom;
    
    float renderX = ctx.viewportPos.x + ctx.panX;
    float renderY = ctx.viewportPos.y + ctx.panY;
    
    // Center if smaller than viewport
    if (roomW < ctx.viewportSize.x) {
        renderX += (ctx.viewportSize.x - roomW) / 2.0f;
    }
    if (roomH < ctx.viewportSize.y) {
        renderY += (ctx.viewportSize.y - roomH) / 2.0f;
    }
    
    // Room background
    drawList->AddRectFilled(
        ImVec2(renderX, renderY),
        ImVec2(renderX + roomW, renderY + roomH),
        IM_COL32(40, 40, 50, 255));
    
    // Room border
    drawList->AddRect(
        ImVec2(renderX, renderY),
        ImVec2(renderX + roomW, renderY + roomH),
        IM_COL32(100, 100, 100, 255), 0, 0, 1.0f);
}

void ViewportSceneRenderer::renderGrid(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    const auto& gridPos = m_scene->getGridPosition();
    float roomW = static_cast<float>(gridPos.pixelWidth) * ctx.zoom;
    float roomH = static_cast<float>(gridPos.pixelHeight) * ctx.zoom;
    
    float renderX = ctx.viewportPos.x + ctx.panX;
    float renderY = ctx.viewportPos.y + ctx.panY;
    
    if (roomW < ctx.viewportSize.x) {
        renderX += (ctx.viewportSize.x - roomW) / 2.0f;
    }
    if (roomH < ctx.viewportSize.y) {
        renderY += (ctx.viewportSize.y - roomH) / 2.0f;
    }
    
    float gridSize = m_gridSize * ctx.zoom;
    ImU32 gridColor = IM_COL32(60, 60, 70, 100);
    ImU32 majorGridColor = IM_COL32(80, 80, 90, 150);
    
    // Vertical lines
    for (float x = 0; x <= roomW; x += gridSize) {
        bool major = (int)(x / gridSize) % 4 == 0;
        drawList->AddLine(
            ImVec2(renderX + x, renderY),
            ImVec2(renderX + x, renderY + roomH),
            major ? majorGridColor : gridColor);
    }
    
    // Horizontal lines
    for (float y = 0; y <= roomH; y += gridSize) {
        bool major = (int)(y / gridSize) % 4 == 0;
        drawList->AddLine(
            ImVec2(renderX, renderY + y),
            ImVec2(renderX + roomW, renderY + y),
            major ? majorGridColor : gridColor);
    }
}

void ViewportSceneRenderer::renderActors(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    const auto& actors = m_scene->getActors();
    
    // Sort by render order
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
    
    const auto& gridPos = m_scene->getGridPosition();
    float roomW = static_cast<float>(gridPos.pixelWidth) * ctx.zoom;
    float roomH = static_cast<float>(gridPos.pixelHeight) * ctx.zoom;
    
    float offsetX = ctx.viewportPos.x + ctx.panX;
    float offsetY = ctx.viewportPos.y + ctx.panY;
    
    if (roomW < ctx.viewportSize.x) {
        offsetX += (ctx.viewportSize.x - roomW) / 2.0f;
    }
    if (roomH < ctx.viewportSize.y) {
        offsetY += (ctx.viewportSize.y - roomH) / 2.0f;
    }
    
    for (auto* actor : sortedActors) {
        engine::Vec2 pos = actor->getPosition();
        float worldX = offsetX + pos.x * ctx.zoom;
        float worldY = offsetY + pos.y * ctx.zoom;
        
        auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
        if (spriteComp) {
            renderActorSprite(drawList, actor, worldX, worldY, ctx.zoom);
        } else {
            renderActorPlaceholder(drawList, actor, worldX, worldY, ctx.zoom);
        }
        
        if (actor == m_selectedActor) {
            renderActorSelection(drawList, actor, worldX, worldY, ctx.zoom);
        }
    }
}

void ViewportSceneRenderer::renderActorSprite(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                               float worldX, float worldY, float zoom) {
    auto* spriteComp = actor->getComponent<engine::SpriteComponent>();
    if (!spriteComp) return;
    
    ImTextureID texID = 0;
    int w = 0, h = 0;
    
    if (ImGuiManager::instance().isUsingOpenGL()) {
        unsigned int glTexID = spriteComp->getGLTextureID();
        if (glTexID == 0 && !spriteComp->getTexturePath().empty()) {
            glTexID = const_cast<engine::SpriteComponent*>(spriteComp)->loadGLTexture(spriteComp->getTexturePath());
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
        // Special handling for background
        if (actor->getName() == "Background" && m_scene) {
            const auto& sceneGrid = m_scene->getGridPosition();
            float roomWidth = static_cast<float>(sceneGrid.pixelWidth) * zoom;
            float roomHeight = static_cast<float>(sceneGrid.pixelHeight) * zoom;
            
            float scaleX = roomWidth / w;
            float scaleY = roomHeight / h;
            float scale = std::min(scaleX, scaleY);
            
            float scaledW = w * scale;
            float scaledH = h * scale;
            float offsetX = (roomWidth - scaledW) / 2.0f;
            float offsetY = (roomHeight - scaledH) / 2.0f;
            
            drawList->AddImage(texID,
                ImVec2(worldX + offsetX, worldY + offsetY),
                ImVec2(worldX + offsetX + scaledW, worldY + offsetY + scaledH));
        } else {
            drawList->AddImage(texID,
                ImVec2(worldX, worldY),
                ImVec2(worldX + w * zoom, worldY + h * zoom));
        }
    }
}

void ViewportSceneRenderer::renderActorPlaceholder(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                                    float worldX, float worldY, float zoom) {
    std::string name = actor->getName();
    ImU32 color = IM_COL32(200, 200, 200, 255);
    
    if (name == "Player") {
        color = IM_COL32(255, 100, 100, 255);
        drawList->AddRectFilled(
            ImVec2(worldX - 16 * zoom, worldY - 24 * zoom),
            ImVec2(worldX + 16 * zoom, worldY + 24 * zoom),
            color);
        drawList->AddRect(
            ImVec2(worldX - 16 * zoom, worldY - 24 * zoom),
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
    else if (name == "WalkArea" && m_scene) {
        color = IM_COL32(100, 255, 100, 200);
        const auto& sceneGrid = m_scene->getGridPosition();
        drawList->AddRect(
            ImVec2(worldX, worldY),
            ImVec2(worldX + sceneGrid.pixelWidth * zoom, worldY + sceneGrid.pixelHeight * zoom),
            color, 0, 0, 2.0f);
    }
    else {
        color = IM_COL32(100, 255, 255, 255);
        drawList->AddRect(
            ImVec2(worldX, worldY),
            ImVec2(worldX + 64 * zoom, worldY + 64 * zoom),
            color, 0, 0, 2.0f);
        drawList->AddText(ImVec2(worldX + 5, worldY + 5), 
                         IM_COL32(255, 255, 255, 255), name.c_str());
    }
}

void ViewportSceneRenderer::renderActorSelection(ImDrawList* drawList, engine::ActorObjectExtended* actor,
                                                  float worldX, float worldY, float zoom) {
    float aw = 64.0f * zoom;
    float ah = 64.0f * zoom;
    
    auto* sprite = actor->getComponent<engine::SpriteComponent>();
    if (sprite) {
        aw = sprite->getWidth() * zoom;
        ah = sprite->getHeight() * zoom;
    }
    
    ImU32 selColor = m_draggedActor == actor ? 
                     IM_COL32(255, 200, 100, 255) : 
                     IM_COL32(100, 200, 255, 255);
    
    drawList->AddRect(
        ImVec2(worldX - 2, worldY - 2),
        ImVec2(worldX + aw + 2, worldY + ah + 2),
        selColor, 0, 0, 3.0f);
    
    // Corner handles
    float handleSize = 6.0f;
    ImU32 handleColor = IM_COL32(255, 255, 255, 255);
    drawList->AddRectFilled(ImVec2(worldX - handleSize/2, worldY - handleSize/2), 
                           ImVec2(worldX + handleSize/2, worldY + handleSize/2), handleColor);
    drawList->AddRectFilled(ImVec2(worldX + aw - handleSize/2, worldY - handleSize/2), 
                           ImVec2(worldX + aw + handleSize/2, worldY + handleSize/2), handleColor);
    drawList->AddRectFilled(ImVec2(worldX - handleSize/2, worldY + ah - handleSize/2), 
                           ImVec2(worldX + handleSize/2, worldY + ah + handleSize/2), handleColor);
    drawList->AddRectFilled(ImVec2(worldX + aw - handleSize/2, worldY + ah - handleSize/2), 
                           ImVec2(worldX + aw + handleSize/2, worldY + ah + handleSize/2), handleColor);
}

void ViewportSceneRenderer::renderPhysicsDebug(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    // Physics debug rendering - simplified for now
}

void ViewportSceneRenderer::renderInfoOverlay(ImDrawList* drawList, const ViewportRenderContext& ctx) {
    float x = ctx.viewportPos.x + 10;
    float y = ctx.viewportPos.y + 10;
    
    char info[256];
    snprintf(info, sizeof(info), "Scene: %s | Actors: %zu | Zoom: %.0f%%",
             m_scene->getName().c_str(), m_scene->getActors().size(), ctx.zoom * 100);
    drawList->AddText(ImVec2(x, y), IM_COL32(255, 255, 255, 200), info);
    
    if (m_selectedActor) {
        snprintf(info, sizeof(info), "Selected: %s", m_selectedActor->getName().c_str());
        drawList->AddText(ImVec2(x, y + 16), IM_COL32(255, 255, 100, 200), info);
    }
}

bool ViewportSceneRenderer::onMouseClick(float x, float y, const ViewportRenderContext& ctx) {
    // Convert to room coordinates
    const auto& gridPos = m_scene->getGridPosition();
    float roomW = static_cast<float>(gridPos.pixelWidth) * ctx.zoom;
    float roomH = static_cast<float>(gridPos.pixelHeight) * ctx.zoom;
    
    float offsetX = ctx.viewportPos.x + ctx.panX;
    float offsetY = ctx.viewportPos.y + ctx.panY;
    
    if (roomW < ctx.viewportSize.x) {
        offsetX += (ctx.viewportSize.x - roomW) / 2.0f;
    }
    if (roomH < ctx.viewportSize.y) {
        offsetY += (ctx.viewportSize.y - roomH) / 2.0f;
    }
    
    float roomX = (x - offsetX) / ctx.zoom;
    float roomY = (y - offsetY) / ctx.zoom;
    
    // Update tool context
    m_toolContext.selectedActor = m_selectedActor;
    m_toolContext.zoom = ctx.zoom;
    m_toolContext.panX = ctx.panX;
    m_toolContext.panY = ctx.panY;
    m_toolContext.gridSize = m_gridSize;
    m_toolContext.snapToGrid = m_snapToGrid;
    
    // Let tool handle first
    bool toolHandled = m_toolManager.onMouseDown(roomX, roomY, m_toolContext);
    
    if (!toolHandled) {
        m_selectedActor = findActorAt(roomX, roomY);
        
        if (m_selectedActor) {
            m_draggedActor = m_selectedActor;
            engine::Vec2 pos = m_selectedActor->getPosition();
            m_dragOffsetX = roomX - pos.x;
            m_dragOffsetY = roomY - pos.y;
            m_toolContext.selectedActor = m_selectedActor;
            
            if (m_selectionManager) {
                m_selectionManager->selectActor(m_selectedActor);
            }
            return true;
        } else {
            if (m_selectionManager) {
                m_selectionManager->clearSelection();
            }
        }
    }
    
    return toolHandled;
}

bool ViewportSceneRenderer::onMouseDrag(float x, float y, float deltaX, float deltaY,
                                         const ViewportRenderContext& ctx) {
    if (!m_draggedActor) return false;
    
    // Convert to room coordinates
    const auto& gridPos = m_scene->getGridPosition();
    float roomW = static_cast<float>(gridPos.pixelWidth) * ctx.zoom;
    float roomH = static_cast<float>(gridPos.pixelHeight) * ctx.zoom;
    
    float offsetX = ctx.viewportPos.x + ctx.panX;
    float offsetY = ctx.viewportPos.y + ctx.panY;
    
    if (roomW < ctx.viewportSize.x) {
        offsetX += (ctx.viewportSize.x - roomW) / 2.0f;
    }
    if (roomH < ctx.viewportSize.y) {
        offsetY += (ctx.viewportSize.y - roomH) / 2.0f;
    }
    
    float roomX = (x - offsetX) / ctx.zoom;
    float roomY = (y - offsetY) / ctx.zoom;
    
    float newX = roomX - m_dragOffsetX;
    float newY = roomY - m_dragOffsetY;
    
    if (m_snapToGrid) {
        newX = round(newX / m_gridSize) * m_gridSize;
        newY = round(newY / m_gridSize) * m_gridSize;
    }
    
    m_draggedActor->setPosition(newX, newY);
    return true;
}

void ViewportSceneRenderer::onMouseRelease(float x, float y, const ViewportRenderContext& ctx) {
    m_draggedActor = nullptr;
    
    float roomX = (x - ctx.viewportPos.x - ctx.panX) / ctx.zoom;
    float roomY = (y - ctx.viewportPos.y - ctx.panY) / ctx.zoom;
    m_toolManager.onMouseUp(roomX, roomY, m_toolContext);
}

bool ViewportSceneRenderer::onDoubleClick(float x, float y, const ViewportRenderContext& ctx) {
    // Double-click doesn't navigate in Scene view
    return false;
}

void ViewportSceneRenderer::onScroll(float delta, const ViewportRenderContext& ctx) {
}

void ViewportSceneRenderer::clearSelection() {
    m_selectedActor = nullptr;
    m_draggedActor = nullptr;
}

engine::ActorObjectExtended* ViewportSceneRenderer::findActorAt(float roomX, float roomY) {
    if (!m_scene) return nullptr;
    
    for (auto& actor : m_scene->getActors()) {
        // Skip locked actors
        auto* lockable = actor->getComponent<engine::LockableComponent>();
        if (lockable && lockable->isLocked()) {
            continue;
        }
        
        engine::Vec2 pos = actor->getPosition();
        float aw = 64.0f;
        float ah = 64.0f;
        
        auto* sprite = actor->getComponent<engine::SpriteComponent>();
        if (sprite) {
            aw = sprite->getWidth();
            ah = sprite->getHeight();
        }
        
        if (roomX >= pos.x && roomX <= pos.x + aw &&
            roomY >= pos.y && roomY <= pos.y + ah) {
            return actor.get();
        }
    }
    
    return nullptr;
}

void ViewportSceneRenderer::deleteSelectedActor() {
    if (!m_scene || !m_selectedActor) return;
    
    // TODO: Use command system
    m_selectedActor = nullptr;
    m_draggedActor = nullptr;
}

void ViewportSceneRenderer::duplicateSelectedActor() {
    if (!m_scene || !m_selectedActor) return;
    
    // TODO: Implement duplication
}

} // namespace editor

#endif // HAS_IMGUI
