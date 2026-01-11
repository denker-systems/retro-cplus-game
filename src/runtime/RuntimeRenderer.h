/**
 * @file RuntimeRenderer.h
 * @brief 3D Renderer för Runtime - renderar ground plane och player
 */
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL_ttf.h>

// Forward declarations
struct ExitData;
class RuntimeQuestSystem;

namespace editor {
    class EditorCamera3D;
}

namespace engine {
    class Player3DActor;
    class Scene;
    class ActorObjectExtended;
}

/**
 * @brief Basic 3D renderer för runtime
 * 
 * Renderar:
 * - Ground plane (grå grid)
 * - Player (grön kub)
 * - Basic lighting
 */
class RuntimeRenderer {
public:
    RuntimeRenderer();
    ~RuntimeRenderer();
    
    /**
     * @brief Initiera renderer (ladda shaders, skapa meshes)
     */
    bool initialize();
    
    /**
     * @brief Rendera scene
     */
    void render(editor::EditorCamera3D* camera, engine::Player3DActor* player, engine::Scene* scene,
                const std::string& backgroundPath = "", bool showLabels = true,
                const std::vector<ExitData>* exits = nullptr);
    
    /**
     * @brief Rendera in-game quest log UI
     */
    void renderQuestLog(RuntimeQuestSystem* questSystem, int windowWidth, int windowHeight);
    
    /**
     * @brief Cleanup
     */
    void shutdown();
    
private:
    bool loadShaders();
    bool loadSpriteShader();
    void createGroundMesh();
    void createPlayerMesh();
    void createNPCMesh();
    void createSpriteMesh();
    void createBackgroundMesh();
    void createWireframeCubeMesh();
    void renderWireframeCube(const glm::vec3& position, const glm::vec3& size, 
                             const glm::vec3& color, const glm::mat4& view, 
                             const glm::mat4& projection);
    GLuint loadTexture(const std::string& path);
    void renderSprite(const glm::vec3& position, const std::string& spriteName, 
                      const glm::mat4& view, const glm::mat4& projection);
    void renderBackground(const std::string& backgroundPath);
    void renderWorldLabel(const std::string& text, const glm::vec3& worldPos,
                          const glm::mat4& view, const glm::mat4& projection,
                          const glm::vec3& color = glm::vec3(1.0f));
    void initTextRendering();
    
    // Waypoint/Compass rendering
    void renderCompass(int windowWidth, int windowHeight, float playerYaw);
    void renderWaypointMarker(const glm::vec3& waypointPos, const glm::vec3& color,
                              const std::string& label, float distance,
                              const glm::mat4& view, const glm::mat4& projection);
    void renderScreenText(const std::string& text, int x, int y, 
                          const glm::vec3& color, bool centered = false);
    
    // Dialog UI
    void renderDialogUI(const std::string& speaker, const std::string& text,
                        const std::vector<std::pair<int, std::string>>& choices,
                        int windowWidth, int windowHeight);
    
    GLuint compileShader(const std::string& source, GLenum type);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    std::string loadShaderFile(const std::string& path);
    
    // Shader program
    GLuint m_shaderProgram = 0;
    
    // Ground mesh (grid lines)
    GLuint m_groundVAO = 0;
    GLuint m_groundVBO = 0;
    GLuint m_groundEBO = 0;
    int m_groundIndexCount = 0;
    bool m_useGridLines = false;
    
    // Player mesh
    GLuint m_playerVAO = 0;
    GLuint m_playerVBO = 0;
    GLuint m_playerEBO = 0;
    int m_playerIndexCount = 0;
    
    // NPC mesh
    GLuint m_npcVAO = 0;
    GLuint m_npcVBO = 0;
    GLuint m_npcEBO = 0;
    int m_npcIndexCount = 0;
    
    // Wireframe cube mesh (for interaction volumes)
    GLuint m_wireframeCubeVAO = 0;
    GLuint m_wireframeCubeVBO = 0;
    GLuint m_wireframeCubeEBO = 0;
    int m_wireframeCubeIndexCount = 0;
    
    // Sprite mesh (billboard quad)
    GLuint m_spriteVAO = 0;
    GLuint m_spriteVBO = 0;
    GLuint m_spriteEBO = 0;
    GLuint m_spriteShader = 0;
    
    // Background mesh (fullscreen quad)
    GLuint m_bgVAO = 0;
    GLuint m_bgVBO = 0;
    GLuint m_bgShader = 0;
    
    // Texture cache
    std::unordered_map<std::string, GLuint> m_textures;
    
    // Text rendering
    TTF_Font* m_font = nullptr;
    std::unordered_map<std::string, GLuint> m_textTextures;
    
    // Lighting
    glm::vec3 m_lightPos = glm::vec3(10.0f, 20.0f, 10.0f);
    glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
};
