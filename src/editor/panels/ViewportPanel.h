/**
 * @file ViewportPanel.h
 * @brief Viewport för rum-preview och editing
 */
#pragma once

#include "editor/IEditorPanel.h"
#include <string>
#include <SDL.h>

struct SDL_Texture;
struct SDL_Renderer;
struct ImDrawList;
struct ImVec2;

namespace engine {
    class Scene;
    class Node;
    class World;
    class Level;
}

class EditorContext;

/**
 * @brief Viewport för att visa och redigera rum
 */
class ViewportPanel : public IEditorPanel {
public:
    explicit ViewportPanel(EditorContext& context);
    ~ViewportPanel();
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    void update(float deltaTime) override;
    
    // Sätt renderer för texture loading
    void setRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }
    void loadRoom(const std::string& roomId);
    
    // World/Level/Scene navigation
    void setWorld(engine::World* world) { m_world = world; }
    void setLevel(engine::Level* level);
    void setScene(engine::Scene* scene); 

private:
    void loadRoomPreview();
    void renderRoomPreview();
    void renderToolbar();
    void renderBreadcrumbs();
    void renderWorldView();
    void renderLevelView();
    void renderSceneView();
    void renderSceneNode(engine::Node* node, ImDrawList* drawList, ImVec2 offset);
    
    EditorContext& m_context;
    std::string m_id = "viewport";
    std::string m_title = "Viewport";
    
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_roomTexture = nullptr;
    std::string m_loadedRoomId;
    
    // World/Level/Scene hierarchy
    engine::World* m_world = nullptr;
    engine::Level* m_level = nullptr;
    engine::Scene* m_scene = nullptr;
    
    // Viewport state
    float m_zoom = 1.0f;
    float m_panX = 0.0f;
    float m_panY = 0.0f;
    bool m_showGrid = true;
    bool m_showHotspots = true;
    bool m_showWalkArea = true;
    
    // Drag state
    bool m_isDragging = false;
    int m_draggedHotspotIndex = -1;
    bool m_draggingSpawn = false;
    bool m_draggingWalkArea = false;
    int m_walkAreaCorner = -1; // 0=TL, 1=TR, 2=BR, 3=BL
    float m_dragOffsetX = 0.0f;
    float m_dragOffsetY = 0.0f;
    
    void handleMouseDown(float mouseX, float mouseY, float previewX, float previewY,
                        float previewW, float previewH, float roomX, float roomY, const struct RoomData* room);
    void handleMouseDrag(float roomX, float roomY);
};
