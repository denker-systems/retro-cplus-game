/**
 * @file TileMapEditorPanel.cpp
 * @brief Implementation of TileMapEditorPanel
 */
#include "TileMapEditorPanel.h"
#include "editor/core/EditorContext.h"
#include <imgui.h>

TileMapEditorPanel::TileMapEditorPanel(EditorContext& context)
    : m_context(context) {}

void TileMapEditorPanel::render() {
    if (!m_visible) return;
    
    if (!ImGui::Begin(m_title.c_str(), &m_visible)) {
        ImGui::End();
        return;
    }
    
    if (!m_tilemap) {
        ImGui::TextDisabled("No tilemap loaded");
        ImGui::End();
        return;
    }
    
    renderToolbar();
    ImGui::Separator();
    
    ImGui::Columns(2);
    
    renderTilePalette();
    ImGui::NextColumn();
    renderTileMapView();
    
    ImGui::Columns(1);
    ImGui::End();
}

void TileMapEditorPanel::renderToolbar() {
    if (ImGui::RadioButton("Paint", m_paintMode)) {
        m_paintMode = true;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Erase", !m_paintMode)) {
        m_paintMode = false;
    }
    ImGui::SameLine();
    ImGui::Checkbox("Collision Mode", &m_collisionMode);
}

void TileMapEditorPanel::renderTilePalette() {
    ImGui::Text("Tile Palette");
    ImGui::Separator();
    
    // Simple tile selector (16 tiles in 4x4 grid)
    for (int i = 0; i < 16; i++) {
        if (i % 4 != 0) ImGui::SameLine();
        
        bool selected = (m_selectedTile == i + 1);
        if (ImGui::Selectable(std::to_string(i + 1).c_str(), selected, 0, ImVec2(40, 40))) {
            m_selectedTile = i + 1;
        }
    }
}

void TileMapEditorPanel::renderTileMapView() {
    ImGui::Text("TileMap: %dx%d", m_tilemap->getWidth(), m_tilemap->getHeight());
    ImGui::Separator();
    
    // Interactive grid painting
    const float cellSize = 32.0f;
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Draw grid
    for (int y = 0; y < m_tilemap->getHeight(); y++) {
        for (int x = 0; x < m_tilemap->getWidth(); x++) {
            ImVec2 p1(canvasPos.x + x * cellSize, canvasPos.y + y * cellSize);
            ImVec2 p2(p1.x + cellSize, p1.y + cellSize);
            
            int tile = m_tilemap->getTile(x, y);
            
            // Draw tile background
            ImU32 color = tile > 0 ? IM_COL32(100, 100, 200, 255) : IM_COL32(50, 50, 50, 255);
            drawList->AddRectFilled(p1, p2, color);
            
            // Draw tile number
            if (tile > 0) {
                char buf[8];
                snprintf(buf, sizeof(buf), "%d", tile);
                drawList->AddText(ImVec2(p1.x + 8, p1.y + 8), IM_COL32(255, 255, 255, 255), buf);
            }
            
            // Draw grid lines
            drawList->AddRect(p1, p2, IM_COL32(100, 100, 100, 255));
        }
    }
    
    // Handle mouse input for painting
    ImVec2 canvasSize(m_tilemap->getWidth() * cellSize, m_tilemap->getHeight() * cellSize);
    ImGui::InvisibleButton("canvas", canvasSize);
    
    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(0)) {
        ImVec2 mousePos = ImGui::GetMousePos();
        int gridX = (int)((mousePos.x - canvasPos.x) / cellSize);
        int gridY = (int)((mousePos.y - canvasPos.y) / cellSize);
        
        if (gridX >= 0 && gridX < m_tilemap->getWidth() && 
            gridY >= 0 && gridY < m_tilemap->getHeight()) {
            
            if (m_paintMode) {
                m_tilemap->setTile(gridX, gridY, m_selectedTile);
                
                // Auto-tiling: adjust neighbors
                if (m_selectedTile > 0) {
                    autoTileNeighbors(gridX, gridY);
                }
            } else {
                m_tilemap->setTile(gridX, gridY, 0);
            }
        }
    }
    
    ImGui::Text("Selected Tile: %d", m_selectedTile);
    ImGui::Text("Mode: %s", m_paintMode ? "Paint" : "Erase");
}

void TileMapEditorPanel::autoTileNeighbors(int x, int y) {
    // Simple auto-tiling: adjust tile variant based on neighbors
    // This is a basic implementation - can be expanded with more complex rules
    
    int neighbors = 0;
    
    // Check 4 cardinal directions
    if (x > 0 && m_tilemap->getTile(x - 1, y) > 0) neighbors |= 1;  // Left
    if (x < m_tilemap->getWidth() - 1 && m_tilemap->getTile(x + 1, y) > 0) neighbors |= 2;  // Right
    if (y > 0 && m_tilemap->getTile(x, y - 1) > 0) neighbors |= 4;  // Up
    if (y < m_tilemap->getHeight() - 1 && m_tilemap->getTile(x, y + 1) > 0) neighbors |= 8;  // Down
    
    // Adjust tile based on neighbor configuration
    // This is a simple example - real auto-tiling would use a tileset with variants
    // For now, we just ensure consistency
    int currentTile = m_tilemap->getTile(x, y);
    if (currentTile > 0 && neighbors > 0) {
        // Tile has neighbors, could adjust variant here
        // Example: m_tilemap->setTile(x, y, baseType + neighborVariant);
    }
}
