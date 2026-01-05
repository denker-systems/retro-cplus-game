/**
 * @file TileMapEditorPanel.h
 * @brief TileMap editor panel
 */
#pragma once

#include "editor/IEditorPanel.h"
#include "nodes/TileMapLayer.h"

class EditorContext;

/**
 * @brief Panel for editing tilemaps
 */
class TileMapEditorPanel : public IEditorPanel {
public:
    explicit TileMapEditorPanel(EditorContext& context);
    ~TileMapEditorPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    void render() override;
    
    void setTileMap(engine::TileMapLayer* tilemap) { m_tilemap = tilemap; }
    
private:
    void renderTilePalette();
    void renderTileMapView();
    void renderToolbar();
    void autoTileNeighbors(int x, int y);
    
    EditorContext& m_context;
    std::string m_id = "TileMapEditor";
    std::string m_title = "TileMap Editor";
    
    engine::TileMapLayer* m_tilemap = nullptr;
    int m_selectedTile = 1;
    bool m_paintMode = true;  // true = paint, false = erase
    bool m_collisionMode = false;
};
