/**
 * @file Viewport3DPanel.h
 * @brief 3D viewport panel for editor
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

class SelectionManager;

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
    
    // Selection
    int getSelectedIndex() const { return m_selectedIndex; }
    engine::ActorObjectExtended* getSelectedActor() const { return m_selectedActor; }
    void syncSelectionFromManager();  // Update selection from SelectionManager
    bool wasDoubleClicked() const { return m_doubleClicked; }
    void clearDoubleClick() { m_doubleClicked = false; }
    void resetSelection() { m_selectedIndex = -1; m_hoveredIndex = -1; m_objectBounds.clear(); }

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
     * @brief Ray-AABB intersection test
     */
    bool rayIntersectsAABB(const glm::vec3& rayOrigin, const glm::vec3& rayDir,
                           const glm::vec3& boxMin, const glm::vec3& boxMax, float& t);
    
    /**
     * @brief Create shaders
     */
    bool createShaders();
};

} // namespace editor

#endif // HAS_IMGUI
