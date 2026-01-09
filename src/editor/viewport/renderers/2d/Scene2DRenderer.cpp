/**
 * @file Scene2DRenderer.cpp
 * @brief Implementation of 2D scene renderer
 */
#include "Scene2DRenderer.h"
#include "../../ui/ViewportActorRenderer.h"
#include "engine/world/Scene.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/components/SpriteComponent.h"
#include "editor/properties/actors/LockableComponent.h"
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>

namespace editor {
namespace viewport {

Scene2DRenderer::Scene2DRenderer() {
    m_actorRenderer = std::make_unique<ViewportActorRenderer>();
}

Scene2DRenderer::~Scene2DRenderer() = default;

void Scene2DRenderer::render2DContent(ImDrawList* drawList, const RenderContext& ctx) {
    if (m_scene) {
        renderSceneContent(drawList, ctx);
    } else if (m_level) {
        // Fallback to parent level rendering
        Level2DRenderer::render2DContent(drawList, ctx);
    } else {
        // Fallback to grandparent world rendering
        World2DRenderer::render2DContent(drawList, ctx);
    }
}

void Scene2DRenderer::renderSceneContent(ImDrawList* drawList, const RenderContext& ctx) {
    if (!m_scene || !drawList) return;
    
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    
    // Get room dimensions
    const auto& gridPos = m_scene->getGridPosition();
    float roomW = static_cast<float>(gridPos.pixelWidth) * m_zoom;
    float roomH = static_cast<float>(gridPos.pixelHeight) * m_zoom;
    
    // Center the room
    float offsetX = canvasPos.x + m_panX;
    float offsetY = canvasPos.y + m_panY;
    if (roomW < canvasSize.x) offsetX += (canvasSize.x - roomW) / 2;
    if (roomH < canvasSize.y) offsetY += (canvasSize.y - roomH) / 2;
    
    // Room area
    drawList->AddRectFilled(ImVec2(offsetX, offsetY), ImVec2(offsetX + roomW, offsetY + roomH),
                            IM_COL32(40, 40, 50, 255));
    drawList->AddRect(ImVec2(offsetX, offsetY), ImVec2(offsetX + roomW, offsetY + roomH),
                     IM_COL32(80, 80, 100, 255), 0, 0, 1.0f);
    
    // Grid
    if (ctx.showGrid) {
        float gridSize = m_gridSize * m_zoom;
        for (float x = 0; x <= roomW; x += gridSize) {
            drawList->AddLine(ImVec2(offsetX + x, offsetY), ImVec2(offsetX + x, offsetY + roomH),
                             IM_COL32(50, 50, 60, 100));
        }
        for (float y = 0; y <= roomH; y += gridSize) {
            drawList->AddLine(ImVec2(offsetX, offsetY + y), ImVec2(offsetX + roomW, offsetY + y),
                             IM_COL32(50, 50, 60, 100));
        }
    }
    
    // Render actors
    if (m_actorRenderer) {
        RenderContext sceneCtx = ctx;
        sceneCtx.zoom = m_zoom;
        m_actorRenderer->renderActors(drawList, m_scene, sceneCtx, offsetX, offsetY,
                                      m_selectedActor, m_draggedActor);
    }
}

void Scene2DRenderer::handle2DInput(const RenderContext& ctx, bool hovered) {
    if (m_scene) {
        // Scene-specific input
        Base2DRenderer::handle2DInput(ctx, hovered);
        
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();
        
        const auto& gridPos = m_scene->getGridPosition();
        float roomW = static_cast<float>(gridPos.pixelWidth) * m_zoom;
        float roomH = static_cast<float>(gridPos.pixelHeight) * m_zoom;
        
        float offsetX = canvasPos.x + m_panX;
        float offsetY = canvasPos.y + m_panY;
        if (roomW < canvasSize.x) offsetX += (canvasSize.x - roomW) / 2;
        if (roomH < canvasSize.y) offsetY += (canvasSize.y - roomH) / 2;
        
        ImVec2 mousePos = io.MousePos;
        float roomX = (mousePos.x - offsetX) / m_zoom;
        float roomY = (mousePos.y - offsetY) / m_zoom;
        
        // Actor selection
        if (ImGui::IsMouseClicked(0)) {
            m_selectedActor = nullptr;
            
            for (auto& actor : m_scene->getActors()) {
                if (!actor) continue;
                
                auto* lockable = actor->getComponent<engine::LockableComponent>();
                if (lockable && lockable->isLocked()) continue;
                
                engine::Vec2 pos = actor->getPosition();
                float aw = 64.0f, ah = 64.0f;
                
                auto* sprite = actor->getComponent<engine::SpriteComponent>();
                if (sprite) {
                    aw = static_cast<float>(sprite->getWidth());
                    ah = static_cast<float>(sprite->getHeight());
                }
                
                if (roomX >= pos.x && roomX <= pos.x + aw &&
                    roomY >= pos.y && roomY <= pos.y + ah) {
                    m_selectedActor = actor.get();
                    m_draggedActor = actor.get();
                    m_actorDragOffsetX = roomX - pos.x;
                    m_actorDragOffsetY = roomY - pos.y;
                    break;
                }
            }
        }
        
        // Drag actor
        if (m_draggedActor && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            float newX = roomX - m_actorDragOffsetX;
            float newY = roomY - m_actorDragOffsetY;
            
            if (m_snapToGrid) {
                newX = std::round(newX / m_gridSize) * m_gridSize;
                newY = std::round(newY / m_gridSize) * m_gridSize;
            }
            
            m_draggedActor->setPosition(newX, newY);
        }
        
        if (ImGui::IsMouseReleased(0)) {
            m_draggedActor = nullptr;
        }
        
        // Keyboard shortcuts
        if (ImGui::IsKeyPressed(ImGuiKey_Delete) && m_selectedActor) {
            deleteSelectedActor();
        }
    } else if (m_level) {
        Level2DRenderer::handle2DInput(ctx, hovered);
    } else {
        World2DRenderer::handle2DInput(ctx, hovered);
    }
}

void Scene2DRenderer::deleteSelectedActor() {
    if (!m_selectedActor || !m_scene) return;
    
    m_scene->removeActor(m_selectedActor);
    m_selectedActor = nullptr;
    m_draggedActor = nullptr;
}

void Scene2DRenderer::duplicateSelectedActor() {
    // TODO: Implement actor cloning
}

} // namespace viewport
} // namespace editor

#endif // HAS_IMGUI
