/**
 * @file ViewportPanel.h
 * @brief Viewport panel with tool-based architecture
 */
#pragma once

#include "editor/IEditorPanel.h"
#include "editor/tools/ToolManager.h"
#include "editor/tools/IEditorTool.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <SDL.h>

struct SDL_Texture;
struct SDL_Renderer;
struct ImDrawList;
struct ImVec2;

namespace engine {
    class Scene;
    class World;
    class Level;
    class ActorObjectExtended;
}

class EditorContext;

/**
 * @brief View mode for the viewport
 */
enum class ViewMode {
    World,      // Shows all levels
    Level,      // Shows all scenes in a level
    Scene       // Shows scene content (actors)
};

/**
 * @brief Viewport panel using tool-based architecture
 */
class ViewportPanel : public IEditorPanel {
public:
    explicit ViewportPanel(EditorContext& context);
    ~ViewportPanel();
    
    // IEditorPanel interface
    void update(float deltaTime) override;
    void render() override;
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    // Hierarchy navigation
    void setWorld(engine::World* world);
    void setLevel(engine::Level* level);
    void setScene(engine::Scene* scene);
    
    // Renderer access
    void setRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }
    SDL_Renderer* getRenderer() const { return m_renderer; }
    
    // Selection
    engine::ActorObjectExtended* getSelectedActor() const { return m_selectedActor; }
    void setSelectedActor(engine::ActorObjectExtended* actor) { m_selectedActor = actor; }
    
    // Tool access
    ToolManager& getToolManager() { return m_toolManager; }
    
private:
    // Rendering methods
    void renderToolbar();
    void renderBreadcrumbs();
    void renderViewModeContent();
    
    // View-specific rendering
    void renderWorldView();
    void renderLevelView();
    void renderSceneView();
    
    // Scene rendering helpers
    void renderSceneBackground(ImDrawList* drawList, ImVec2 offset);
    void renderSceneActors(ImDrawList* drawList, ImVec2 offset);
    void renderSceneGrid(ImDrawList* drawList, ImVec2 offset, ImVec2 size);
    void renderSelectionOverlay(ImDrawList* drawList, ImVec2 offset);
    
    // Input handling
    void handleInput(ImVec2 renderPos, ImVec2 roomSize);
    void handleDragDrop(ImVec2 renderPos);
    
    // Texture management
    SDL_Texture* loadTexture(const std::string& path);
    void clearTextureCache();
    
    // Context
    EditorContext& m_context;
    std::string m_id = "viewport_refactored";
    std::string m_title = "Viewport";
    
    // Renderer
    SDL_Renderer* m_renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> m_textureCache;
    
    // Hierarchy
    engine::World* m_world = nullptr;
    engine::Level* m_level = nullptr;
    engine::Scene* m_scene = nullptr;
    ViewMode m_viewMode = ViewMode::Scene;
    
    // Tools
    ToolManager m_toolManager;
    ToolContext m_toolContext;
    
    // Selection
    engine::ActorObjectExtended* m_selectedActor = nullptr;
    
    // Viewport state
    float m_zoom = 1.0f;
    float m_panX = 0.0f;
    float m_panY = 0.0f;
    
    // Options
    bool m_showGrid = true;
    bool m_showHotspots = true;
    bool m_showWalkArea = true;
    bool m_snapToGrid = true;
    int m_gridSize = 32;
};
