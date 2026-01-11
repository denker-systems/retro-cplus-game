/**
 * @file Viewport3DPanel.h
 * @brief 3D viewport panel for editor
 */
#pragma once

#ifdef HAS_IMGUI

#include "EditorCamera3D.h"
#include "editor/gizmos/TransformGizmo3D.h"
#include "engine/graphics/Framebuffer.h"
#include "engine/graphics/Shader.h"
#include "engine/graphics/Mesh.h"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

class SelectionManager;

namespace editor {
    class EditorPlayMode;
}

namespace engine {
    class World;
    class Level;
    class Scene;
    class ActorObjectExtended;
}

namespace editor {

/**
 * @brief View level for 3D viewport
 */
enum class View3DLevel {
    World,  // Show levels as boxes
    Level,  // Show scenes as tiles
    Scene   // Show full scene
};

/**
 * @class Viewport3DPanel
 * @brief Renders a 3D scene in an ImGui window
 * 
 * Features:
 * - Framebuffer rendering to ImGui image
 * - Orbit camera controls
 * - Grid and axis display
 * - Basic 3D object rendering
 */
class Viewport3DPanel {
public:
    Viewport3DPanel();
    ~Viewport3DPanel();
    
    /**
     * @brief Initialize OpenGL resources
     * @return true if initialization succeeded
     */
    bool initialize();
    
    /**
     * @brief Render the 3D viewport panel
     */
    void render();
    
    /**
     * @brief Update logic (called each frame)
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);
    
    /**
     * @brief Check if panel is initialized
     */
    bool isInitialized() const { return m_initialized; }
    
    /**
     * @brief Set what to render
     */
    void setViewLevel(View3DLevel level) { m_viewLevel = level; }
    void setWorld(engine::World* world) { m_world = world; }
    void setLevel(engine::Level* level) { m_level = level; }
    void setScene(engine::Scene* scene) { m_scene = scene; }
    void setSelectionManager(SelectionManager* selMgr);
    void setPlayMode(EditorPlayMode* playMode);
    
    // Selection
    int getSelectedIndex() const { return m_selectedIndex; }
    engine::ActorObjectExtended* getSelectedActor() const { return m_selectedActor; }
    void syncSelectionFromManager();  // Update selection from SelectionManager
    bool wasDoubleClicked() const { return m_doubleClicked; }
    void clearDoubleClick() { m_doubleClicked = false; }
    void resetSelection() { m_selectedIndex = -1; m_hoveredIndex = -1; m_objectBounds.clear(); }
    
    // Navigation - get selected Level/Scene for double-click navigation
    View3DLevel getViewLevel() const { return m_viewLevel; }
    engine::Level* getSelectedLevel() const;   // Returns selected level in World view
    engine::Scene* getSelectedScene() const;   // Returns selected scene in Level view
    
    // Camera access
    EditorCamera3D* getCamera() { return m_camera.get(); }
    
    // Actor creation
    void handleActorDrop(const std::string& templateName);

private:
    bool m_initialized = false;
    
    // Rendering resources
    std::unique_ptr<engine::Framebuffer> m_framebuffer;
    std::unique_ptr<EditorCamera3D> m_camera;
    std::unique_ptr<engine::Shader> m_shader;
    std::unique_ptr<engine::Shader> m_gridShader;
    std::unique_ptr<engine::Mesh> m_gridMesh;
    std::unique_ptr<engine::Mesh> m_cubeMesh;
    std::unique_ptr<engine::Mesh> m_boxMesh;    // For levels
    std::unique_ptr<engine::Mesh> m_planeMesh;  // For scenes
    std::unique_ptr<engine::Mesh> m_wireframeCubeMesh;  // For interaction volumes
    
    // Viewport state
    glm::vec2 m_viewportSize{0.0f, 0.0f};
    glm::vec2 m_viewportPos{0.0f, 0.0f};
    bool m_viewportFocused = false;
    bool m_viewportHovered = false;
    
    // Mouse state for camera control
    bool m_isOrbiting = false;
    bool m_isPanning = false;
    glm::vec2 m_lastMousePos{0.0f, 0.0f};
    
    // View data
    View3DLevel m_viewLevel = View3DLevel::Scene;
    engine::World* m_world = nullptr;
    engine::Level* m_level = nullptr;
    engine::Scene* m_scene = nullptr;
    
    // Selection state
    int m_selectedIndex = -1;
    int m_hoveredIndex = -1;
    bool m_doubleClicked = false;
    engine::ActorObjectExtended* m_selectedActor = nullptr;
    SelectionManager* m_selectionManager = nullptr;
    EditorPlayMode* m_playMode = nullptr;
    
    // Drag state for 3D movement
    bool m_isDragging = false;
    glm::vec3 m_dragStartPos{0.0f};      // World position where drag started
    glm::vec3 m_actorStartPos{0.0f};     // Actor position when drag started
    int m_dragAxis = -1;                  // -1=XZ plane, 0=X, 1=Y, 2=Z
    
    // Transform gizmo
    TransformGizmo3D m_gizmo;
    
    // Object bounds for picking (stores actor pointers for Scene view)
    std::vector<std::pair<glm::vec3, glm::vec3>> m_objectBounds; // min, max pairs
    std::vector<engine::ActorObjectExtended*> m_actorBounds;     // Corresponding actors
    
    /**
     * @brief Render the 3D scene to framebuffer
     */
    void renderScene();
    
    /**
     * @brief Render based on view level
     */
    void renderWorldView();   // Levels as boxes
    void renderLevelView();   // Scenes as tiles
    void renderSceneView();   // Full scene
    
    /**
     * @brief Render grid at Y=0
     */
    void renderGrid();
    
    /**
     * @brief Handle mouse input for camera
     */
    void handleInput();
    
    /**
     * @brief Handle object picking
     */
    void handlePicking();
    
    /**
     * @brief Handle 3D dragging of selected actor
     */
    void handleDragging();
    
    /**
     * @brief Get world position from mouse ray on XZ plane
     */
    glm::vec3 getWorldPosOnPlane(float planeY);
    
    /**
     * @brief Ray-AABB intersection test
     */
    bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                           const glm::vec3& boxMin, const glm::vec3& boxMax, float& t);
    
    /**
     * @brief Create shaders
     */
    bool createShaders();
    
    /**
     * @brief Render interaction volume wireframe
     */
    void renderInteractionVolume(const glm::vec3& position, const glm::vec3& size, 
                                  const glm::vec3& color, const glm::mat4& view, 
                                  const glm::mat4& projection);
};

} // namespace editor

#endif // HAS_IMGUI
