/**
 * @file ViewportRenderer.h
 * @brief Unified viewport renderer for 2D and 3D at all hierarchy levels
 * 
 * Renders World, Level, and Scene with the same feature set.
 * Only difference is LOD and what content is visible.
 */
#pragma once

#ifdef HAS_IMGUI

#include "EditorCamera3D.h"
#include "engine/graphics/Framebuffer.h"
#include "engine/graphics/Shader.h"
#include "engine/graphics/Mesh.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include <SDL.h>

struct ImDrawList;

namespace engine {
    class World;
    class Level;
    class Scene;
    class ActorObjectExtended;
}

namespace editor {

/**
 * @brief Current hierarchy level being viewed
 */
enum class HierarchyLevel {
    World,  // Viewing all levels
    Level,  // Viewing all scenes in a level
    Scene   // Viewing all actors in a scene
};

/**
 * @brief Render mode
 */
enum class RenderMode {
    Mode2D,
    Mode3D
};

/**
 * @brief Object bounds for picking
 */
struct PickableObject {
    glm::vec3 boundsMin;
    glm::vec3 boundsMax;
    void* userData = nullptr;  // Level*, Scene*, or Actor*
    int index = -1;
};

/**
 * @class ViewportRenderer
 * @brief Unified renderer for all viewport content
 * 
 * Features:
 * - Same functionality at World, Level, Scene
 * - 2D and 3D rendering modes
 * - Actor rendering with LOD
 * - Selection highlighting
 * - Grid overlay
 */
class ViewportRenderer {
public:
    ViewportRenderer();
    ~ViewportRenderer();
    
    /**
     * @brief Initialize rendering resources
     */
    bool initialize();
    
    /**
     * @brief Render the viewport content
     * @param drawList ImGui draw list for 2D rendering
     * @param viewportPos Top-left position of viewport
     * @param viewportSize Size of viewport
     */
    void render(ImDrawList* drawList, const glm::vec2& viewportPos, const glm::vec2& viewportSize);
    
    /**
     * @brief Update per-frame logic
     */
    void update(float deltaTime);
    
    // Configuration
    void setHierarchyLevel(HierarchyLevel level) { m_hierarchyLevel = level; }
    void setRenderMode(RenderMode mode) { m_renderMode = mode; }
    void setSDLRenderer(SDL_Renderer* renderer) { m_sdlRenderer = renderer; }
    
    // Data sources
    void setWorld(engine::World* world) { m_world = world; }
    void setActiveLevel(engine::Level* level) { m_activeLevel = level; }
    void setActiveScene(engine::Scene* scene) { m_activeScene = scene; }
    
    // Camera control
    void setZoom(float zoom) { m_zoom2D = zoom; }
    void setPan(const glm::vec2& pan) { m_pan2D = pan; }
    float getZoom() const { return m_zoom2D; }
    glm::vec2 getPan() const { return m_pan2D; }
    EditorCamera3D* getCamera3D() { return m_camera3D.get(); }
    
    // Selection
    int getHoveredIndex() const { return m_hoveredIndex; }
    int getSelectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int index) { m_selectedIndex = index; }
    void clearSelection() { m_selectedIndex = -1; m_hoveredIndex = -1; }
    const std::vector<PickableObject>& getPickableObjects() const { return m_pickableObjects; }
    
    // Picking
    int pickObjectAt(const glm::vec2& screenPos, const glm::vec2& viewportPos, const glm::vec2& viewportSize);
    
    // State
    bool isInitialized() const { return m_initialized; }
    RenderMode getRenderMode() const { return m_renderMode; }
    HierarchyLevel getHierarchyLevel() const { return m_hierarchyLevel; }

private:
    bool m_initialized = false;
    
    // Current state
    HierarchyLevel m_hierarchyLevel = HierarchyLevel::Scene;
    RenderMode m_renderMode = RenderMode::Mode2D;
    
    // Data
    engine::World* m_world = nullptr;
    engine::Level* m_activeLevel = nullptr;
    engine::Scene* m_activeScene = nullptr;
    SDL_Renderer* m_sdlRenderer = nullptr;
    
    // 2D camera
    float m_zoom2D = 1.0f;
    glm::vec2 m_pan2D{0.0f, 0.0f};
    
    // 3D resources
    std::unique_ptr<EditorCamera3D> m_camera3D;
    std::unique_ptr<engine::Framebuffer> m_framebuffer;
    std::unique_ptr<engine::Shader> m_shader;
    std::unique_ptr<engine::Shader> m_gridShader;
    std::unique_ptr<engine::Mesh> m_gridMesh;
    std::unique_ptr<engine::Mesh> m_cubeMesh;
    
    // Selection
    int m_hoveredIndex = -1;
    int m_selectedIndex = -1;
    std::vector<PickableObject> m_pickableObjects;
    
    // Viewport state (for 3D)
    glm::vec2 m_viewportSize{0.0f, 0.0f};
    glm::vec2 m_viewportPos{0.0f, 0.0f};
    
    // 2D Rendering
    void render2D(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size);
    void render2DWorld(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size);
    void render2DLevel(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size);
    void render2DScene(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size);
    void render2DActors(ImDrawList* drawList, const glm::vec2& offset, 
                        const std::vector<engine::ActorObjectExtended*>& actors);
    void render2DGrid(ImDrawList* drawList, const glm::vec2& offset, const glm::vec2& size);
    
    // 3D Rendering
    void render3D(const glm::vec2& viewportPos, const glm::vec2& viewportSize);
    void render3DWorld();
    void render3DLevel();
    void render3DScene();
    void render3DActors(const std::vector<engine::ActorObjectExtended*>& actors);
    void render3DGrid();
    
    // Common
    glm::vec3 getColorForIndex(int index, int total, bool selected, bool hovered);
    bool createShaders();
    
    // Picking helpers
    bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                           const glm::vec3& boxMin, const glm::vec3& boxMax, float& t);
};

} // namespace editor

#endif // HAS_IMGUI
