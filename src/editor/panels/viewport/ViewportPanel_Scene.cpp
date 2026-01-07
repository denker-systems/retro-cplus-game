/**
 * @file ViewportPanel_Scene.cpp
 * @brief ViewportPanel Scene view rendering and interaction
 */
#include "ViewportPanel.h"
#include "editor/core/EditorContext.h"
#include "engine/world/Scene.h"
#include "engine/components/SpriteComponent.h"
#include "engine/core/ActorObjectExtended.h"
#include "engine/utils/Logger.h"
#include <algorithm>
#include <cmath>
#include <filesystem>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

void ViewportPanel::renderSceneView() {
#ifdef HAS_IMGUI
    if (!m_scene) {
        ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Scene selected");
        return;
    }
    
    ImVec2 contentSize = ImGui::GetContentRegionAvail();
    ImVec2 cursorPos = ImGui::GetCursorScreenPos();
    
    // Get dynamic room size from Scene's GridPosition
    const auto& gridPos = m_scene->getGridPosition();
    float baseWidth = static_cast<float>(gridPos.pixelWidth);
    float baseHeight = static_cast<float>(gridPos.pixelHeight);
    ImVec2 roomSize(baseWidth * m_zoom, baseHeight * m_zoom);
    
    // Center the room in the viewport if it's smaller
    ImVec2 renderPos = cursorPos;
    if (roomSize.x < contentSize.x) {
        renderPos.x += (contentSize.x - roomSize.x) / 2.0f;
    }
    if (roomSize.y < contentSize.y) {
        renderPos.y += (contentSize.y - roomSize.y) / 2.0f;
    }
    
    // Background (room boundaries)
    ImGui::GetWindowDrawList()->AddRectFilled(
        renderPos,
        ImVec2(renderPos.x + roomSize.x, renderPos.y + roomSize.y),
        IM_COL32(40, 40, 50, 255));
    
    // Draw grid overlay
    if (m_showSceneGrid) {
        renderSceneGrid(ImGui::GetWindowDrawList(), renderPos, roomSize);
    }
    
    // Draw room border
    ImGui::GetWindowDrawList()->AddRect(
        renderPos,
        ImVec2(renderPos.x + roomSize.x, renderPos.y + roomSize.y),
        IM_COL32(100, 100, 100, 255), 0, 0, 1.0f);
    
    // Render actors directly from Scene
    if (m_scene) {
        renderSceneActors(ImGui::GetWindowDrawList(), 
                         ImVec2(renderPos.x + m_panX, renderPos.y + m_panY));
    }
    
    // Render tool overlay (scale handles, etc.)
    m_toolContext.selectedActor = m_selectedActor;
    m_toolContext.zoom = m_zoom;
    m_toolContext.panX = m_panX;
    m_toolContext.panY = m_panY;
    m_toolContext.viewportX = renderPos.x;
    m_toolContext.viewportY = renderPos.y;
    m_toolManager.renderOverlay(ImGui::GetWindowDrawList(), m_toolContext);
    
    // Make area interactive (use full content size for interaction)
    ImGui::SetCursorScreenPos(cursorPos);
    ImGui::InvisibleButton("scene_canvas", contentSize);
    
    // Handle drag-and-drop from Asset Browser or Place Actors
    if (ImGui::BeginDragDropTarget()) {
        // Asset path drop (sprites/backgrounds)
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ASSET_PATH")) {
            const char* assetPath = static_cast<const char*>(payload->Data);
            
            // Calculate drop position in room coordinates
            ImVec2 mousePos = ImGui::GetMousePos();
            float dropX = (mousePos.x - renderPos.x - m_panX) / m_zoom;
            float dropY = (mousePos.y - renderPos.y - m_panY) / m_zoom;
            
            // Snap to grid if enabled
            if (m_snapToGrid) {
                dropX = round(dropX / m_sceneGridSize) * m_sceneGridSize;
                dropY = round(dropY / m_sceneGridSize) * m_sceneGridSize;
            }
            
            // Create new actor with sprite
            std::string filename = std::filesystem::path(assetPath).stem().string();
            auto newActor = std::make_unique<engine::ActorObjectExtended>(filename);
            newActor->setPosition(dropX, dropY);
            
            // Add sprite component
            auto* spriteComp = newActor->addComponent<engine::SpriteComponent>();
            if (spriteComp && m_renderer) {
                spriteComp->loadTexture(assetPath, m_renderer);
            }
            
            // Add to scene and select
            m_selectedActor = newActor.get();
            m_scene->addActor(std::move(newActor));
            
            LOG_DEBUG("ViewportPanel: Created sprite actor '" + filename + "' at (" + 
                     std::to_string((int)dropX) + ", " + std::to_string((int)dropY) + ")");
        }
        
        // Actor template drop (from Place Actors panel)
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ACTOR_TEMPLATE")) {
            const char* templateName = static_cast<const char*>(payload->Data);
            
            // Calculate drop position
            ImVec2 mousePos = ImGui::GetMousePos();
            float dropX = (mousePos.x - renderPos.x - m_panX) / m_zoom;
            float dropY = (mousePos.y - renderPos.y - m_panY) / m_zoom;
            
            if (m_snapToGrid) {
                dropX = round(dropX / m_sceneGridSize) * m_sceneGridSize;
                dropY = round(dropY / m_sceneGridSize) * m_sceneGridSize;
            }
            
            // Create actor based on template
            auto newActor = std::make_unique<engine::ActorObjectExtended>(templateName);
            newActor->setPosition(dropX, dropY);
            
            // Add components based on template type
            std::string tname = templateName;
            if (tname == "Sprite Actor" || tname == "Static Prop" || tname == "Interactive Prop" ||
                tname == "NPC" || tname == "Enemy" || tname == "Container") {
                newActor->addComponent<engine::SpriteComponent>();
            }
            
            // Add to scene and select
            m_selectedActor = newActor.get();
            m_scene->addActor(std::move(newActor));
            
            LOG_DEBUG("ViewportPanel: Created actor '" + tname + "' at (" + 
                     std::to_string((int)dropX) + ", " + std::to_string((int)dropY) + ")");
        }
        
        ImGui::EndDragDropTarget();
    }
    
    // Handle mouse interactions
    if (ImGui::IsItemHovered() || m_draggedActor) {
        ImVec2 mousePos = ImGui::GetMousePos();
        
        // Convert mouse to room coordinates
        float roomX = (mousePos.x - renderPos.x - m_panX) / m_zoom;
        float roomY = (mousePos.y - renderPos.y - m_panY) / m_zoom;
        
        // Update tool context
        m_toolContext.selectedActor = m_selectedActor;
        m_toolContext.zoom = m_zoom;
        m_toolContext.panX = m_panX;
        m_toolContext.panY = m_panY;
        m_toolContext.gridSize = m_sceneGridSize;
        m_toolContext.snapToGrid = m_snapToGrid;
        
        // Left click - let tool handle first, then select actor
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            bool toolHandled = m_toolManager.onMouseDown(roomX, roomY, m_toolContext);
            
            if (!toolHandled) {
                m_selectedActor = nullptr;
                m_draggedActor = nullptr;
                
                // Find actor under mouse
                for (auto& actor : m_scene->getActors()) {
                    engine::Vec2 pos = actor->getPosition();
                    float ax = pos.x;
                    float ay = pos.y;
                    float aw = 64.0f;
                    float ah = 64.0f;
                    
                    auto* sprite = actor->getComponent<engine::SpriteComponent>();
                    if (sprite) {
                        aw = sprite->getWidth();
                        ah = sprite->getHeight();
                    }
                    
                    if (roomX >= ax && roomX <= ax + aw &&
                        roomY >= ay && roomY <= ay + ah) {
                        m_selectedActor = actor.get();
                        m_draggedActor = actor.get();
                        m_actorDragOffsetX = roomX - ax;
                        m_actorDragOffsetY = roomY - ay;
                        m_toolContext.selectedActor = m_selectedActor;
                        break;
                    }
                }
            }
        }
        
        // Drag - let tool handle first
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            bool toolHandled = m_toolManager.onMouseDrag(roomX, roomY, delta.x / m_zoom, delta.y / m_zoom, m_toolContext);
            
            if (!toolHandled && m_draggedActor) {
                float newX = roomX - m_actorDragOffsetX;
                float newY = roomY - m_actorDragOffsetY;
                
                if (m_snapToGrid) {
                    newX = round(newX / m_sceneGridSize) * m_sceneGridSize;
                    newY = round(newY / m_sceneGridSize) * m_sceneGridSize;
                }
                
                m_draggedActor->setPosition(newX, newY);
            }
        }
        
        // Release drag
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
            m_toolManager.onMouseUp(roomX, roomY, m_toolContext);
            m_draggedActor = nullptr;
        }
        
        // Middle mouse drag - pan view
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle)) {
            ImVec2 delta = ImGui::GetIO().MouseDelta;
            m_panX += delta.x;
            m_panY += delta.y;
        }
        
        // Mouse wheel - zoom
        float wheel = ImGui::GetIO().MouseWheel;
        if (wheel != 0.0f && !m_draggedActor) {
            m_zoom = std::max(0.25f, std::min(4.0f, m_zoom + wheel * 0.1f));
        }
    }
    
    // Cancel drag with Escape
    if (m_draggedActor && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        m_draggedActor = nullptr;
    }
    
    // UE5-style keyboard shortcuts for tools
    if (ImGui::IsKeyPressed(ImGuiKey_W)) {
        m_toolManager.setActiveTool(ToolType::Select);  // W = Translate/Move
        LOG_DEBUG("ViewportPanel: Tool changed to Select (W)");
    }
    if (ImGui::IsKeyPressed(ImGuiKey_E)) {
        // E = Rotate (not implemented yet, use Select)
        m_toolManager.setActiveTool(ToolType::Select);
        LOG_DEBUG("ViewportPanel: Rotate not implemented, using Select (E)");
    }
    if (ImGui::IsKeyPressed(ImGuiKey_R)) {
        m_toolManager.setActiveTool(ToolType::Scale);   // R = Scale
        LOG_DEBUG("ViewportPanel: Tool changed to Scale (R)");
    }
    if (ImGui::IsKeyPressed(ImGuiKey_Space)) {
        // Spacebar = Toggle between tools
        if (m_toolManager.getActiveToolType() == ToolType::Select) {
            m_toolManager.setActiveTool(ToolType::Scale);
            LOG_DEBUG("ViewportPanel: Tool toggled to Scale (Space)");
        } else {
            m_toolManager.setActiveTool(ToolType::Select);
            LOG_DEBUG("ViewportPanel: Tool toggled to Select (Space)");
        }
    }
    
    // Show info overlay
    ImGui::SetCursorScreenPos(ImVec2(cursorPos.x + 10, cursorPos.y + 10));
    ImGui::BeginGroup();
    ImGui::Text("Scene: %s", m_scene->getName().c_str());
    ImGui::Text("Actors: %zu", m_scene->getActors().size());
    if (m_selectedActor) {
        ImGui::Text("Selected: %s", m_selectedActor->getName().c_str());
    }
    ImGui::Text("Zoom: %.0f%% | Pan: %.0f, %.0f", m_zoom * 100, m_panX, m_panY);
    const char* tip = m_draggedActor ? "Drag to move, Escape to cancel" :
                      m_selectedActor ? "Click to select, drag to move" :
                      "Click actor to select, Middle-mouse to pan";
    ImGui::Text("Tip: %s", tip);
    ImGui::EndGroup();
#endif
}

void ViewportPanel::renderSceneGrid(ImDrawList* drawList, ImVec2 offset, ImVec2 size) {
#ifdef HAS_IMGUI
    float gridSize = m_sceneGridSize * m_zoom;
    ImU32 gridColor = IM_COL32(60, 60, 70, 100);
    ImU32 majorGridColor = IM_COL32(80, 80, 90, 150);
    
    // Draw vertical lines
    for (float x = 0; x <= size.x; x += gridSize) {
        bool major = (int)(x / gridSize) % 4 == 0;
        drawList->AddLine(
            ImVec2(offset.x + x, offset.y),
            ImVec2(offset.x + x, offset.y + size.y),
            major ? majorGridColor : gridColor);
    }
    
    // Draw horizontal lines
    for (float y = 0; y <= size.y; y += gridSize) {
        bool major = (int)(y / gridSize) % 4 == 0;
        drawList->AddLine(
            ImVec2(offset.x, offset.y + y),
            ImVec2(offset.x + size.x, offset.y + y),
            major ? majorGridColor : gridColor);
    }
    
    // Grid size indicator
    char gridInfo[32];
    snprintf(gridInfo, sizeof(gridInfo), "Grid: %dpx", m_sceneGridSize);
    drawList->AddText(ImVec2(offset.x + 5, offset.y + size.y - 18), 
                     IM_COL32(100, 100, 100, 200), gridInfo);
#endif
}

void ViewportPanel::deleteSelectedActor() {
    if (!m_scene || !m_selectedActor) return;
    
    std::string actorName = m_selectedActor->getName();
    
    // Remove from scene using removeActor method
    m_scene->removeActor(m_selectedActor);
    
    m_selectedActor = nullptr;
    m_draggedActor = nullptr;
    
    LOG_DEBUG("ViewportPanel: Deleted actor '" + actorName + "'");
}

void ViewportPanel::duplicateSelectedActor() {
    if (!m_scene || !m_selectedActor) return;
    
    // Create a copy of the selected actor
    std::string newName = m_selectedActor->getName() + "_copy";
    auto newActor = std::make_unique<engine::ActorObjectExtended>(newName);
    
    // Copy position with offset
    engine::Vec2 pos = m_selectedActor->getPosition();
    newActor->setPosition(pos.x + 32, pos.y + 32);
    
    // Copy sprite component if exists
    auto* srcSprite = m_selectedActor->getComponent<engine::SpriteComponent>();
    if (srcSprite) {
        auto* dstSprite = newActor->addComponent<engine::SpriteComponent>();
        if (dstSprite && m_renderer) {
            // Copy texture path would be ideal, for now just copy size
            dstSprite->setSize(srcSprite->getWidth(), srcSprite->getHeight());
        }
    }
    
    // Add to scene and select
    m_selectedActor = newActor.get();
    m_scene->addActor(std::move(newActor));
    
    LOG_DEBUG("ViewportPanel: Duplicated actor as '" + newName + "'");
}
