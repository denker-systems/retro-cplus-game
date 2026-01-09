/**
 * @file ViewportPanel.h
 * @brief Viewport för rum-preview och editing
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include "editor/core/SelectionManager.h"
#include "editor/tools/ToolManager.h"
#include <string>
#include <memory>
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
    class ActorObjectExtended;
}

class EditorContext;

namespace editor {
    class Viewport3DPanel;
    class EditorPlayMode;
}

/**
 * @brief View mode for viewport
 */
enum class ViewportMode {
    Mode2D,
    Mode3D
};

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
    
    // World/Level/Scene navigation (syncs with SelectionManager)
    void setWorld(engine::World* world);
    void setLevel(engine::Level* level);
    void setScene(engine::Scene* scene); 
    engine::Scene* getScene() const { return m_scene; }
    
    // Sync navigation from SelectionManager
    void syncFromSelectionManager();
    
    // Actor operations
    void deleteSelectedActor();
    void duplicateSelectedActor();
    
    // Get selection manager for external access
    SelectionManager* getSelectionManager() const { return m_selectionManager; }
    
    // Play mode
    void setPlayMode(editor::EditorPlayMode* playMode) { m_playMode = playMode; }
    editor::EditorPlayMode* getPlayMode() const { return m_playMode; }

private:
    void loadRoomPreview();
    void renderRoomPreview();
    void renderToolbar();
    void renderBreadcrumbs();
    void renderWorldView();

    void renderWorldSpatialView();  // NEW: Spatial view for World
    void renderWorld3D();           // 3D view for World (levels as boxes)
    void renderLevelView();
    void renderLevel3D();           // 3D view for Level (scenes as tiles)
    void renderSceneView();
    void renderSpatialView();  // NEW: Spatial grid view for levels
    // renderSceneNode() removed - Node system deprecated
    void renderSceneActors(ImDrawList* drawList, ImVec2 offset);
    void renderSceneGrid(ImDrawList* drawList, ImVec2 offset, ImVec2 size);  // NEW: Grid overlay
    
    EditorContext& m_context;
    SelectionManager* m_selectionManager = nullptr;
    std::string m_id = "viewport";
    std::string m_title = "Viewport";
    
    SDL_Renderer* m_renderer = nullptr;
    SDL_Texture* m_roomTexture = nullptr;
    std::string m_loadedRoomId;
    
    // World/Level/Scene hierarchy
    engine::World* m_world = nullptr;
    engine::Level* m_level = nullptr;
    engine::Scene* m_scene = nullptr;
    
    // World view mode: 0=Spatial, 1=Grid
    int m_worldViewMode = 0;
    float m_worldSpatialZoom = 1.0f;
    float m_worldSpatialPanX = 0.0f;
    float m_worldSpatialPanY = 0.0f;
    
    // World view editing state (Level drag)
    engine::Level* m_selectedLevel = nullptr;
    engine::Level* m_draggedLevel = nullptr;
    float m_dragLevelStartX = 0.0f;
    float m_dragLevelStartY = 0.0f;
    int m_dragLevelStartGridX = 0;
    int m_dragLevelStartGridY = 0;
    
    // Level view mode: 0=Spatial, 1=Grid, 2=List
    int m_levelViewMode = 0;  // Default to Spatial
    float m_spatialZoom = 1.0f;
    float m_spatialPanX = 0.0f;
    float m_spatialPanY = 0.0f;
    
    // Scene grid overlay
    bool m_showSceneGrid = true;
    int m_sceneGridSize = 32;  // Grid cell size in pixels
    
    // Physics debug visualization
    bool m_showPhysicsDebug = true;
    
    // Spatial View editing state
    engine::Scene* m_selectedScene = nullptr;
    engine::Scene* m_draggedScene = nullptr;
    float m_dragStartX = 0.0f;
    float m_dragStartY = 0.0f;
    int m_dragStartGridX = 0;
    int m_dragStartGridY = 0;
    bool m_snapToGrid = true;
    
    // Scene View actor editing state
    engine::ActorObjectExtended* m_selectedActor = nullptr;
    engine::ActorObjectExtended* m_draggedActor = nullptr;
    float m_actorDragOffsetX = 0.0f;
    float m_actorDragOffsetY = 0.0f;
    
    // Collision box selection state
    int m_selectedCollisionBoxIndex = -1;
    bool m_isDraggingCollisionBox = false;
    float m_collisionBoxDragOffsetX = 0.0f;
    float m_collisionBoxDragOffsetY = 0.0f;
    
    // Tool system
    ToolManager m_toolManager;
    ToolContext m_toolContext;
    
    // Viewport state
    float m_zoom = 1.0f;
    float m_panX = 0.0f;
    float m_panY = 0.0f;
    bool m_showGrid = true;
    bool m_showHotspots = true;
    bool m_showWalkArea = true;
    
    // 2D/3D mode
    ViewportMode m_viewportMode = ViewportMode::Mode2D;
    std::unique_ptr<editor::Viewport3DPanel> m_viewport3D;
    
    // Play mode reference
    editor::EditorPlayMode* m_playMode = nullptr;
    
    // Drag state
    bool m_isDragging = false;
    int m_draggedHotspotIndex = -1;
    bool m_draggingSpawn = false;
    bool m_draggingWalkArea = false;
    int m_walkAreaCorner = -1; // 0=TL, 1=TR, 2=BR, 3=BL
    float m_dragOffsetX = 0.0f;
    float m_dragOffsetY = 0.0f;
    
    void handleMouseDown(float mouseX, float mouseY, float previewX, float previewY,
                        float previewW, float previewH, float roomX, float roomY, const struct SceneData* room);
    void handleMouseDrag(float roomX, float roomY);
};
