/**
 * @file RuntimeRenderer.h
 * @brief 3D Renderer för Runtime - renderar ground plane och player
 */
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

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
    void render(editor::EditorCamera3D* camera, engine::Player3DActor* player, engine::Scene* scene);
    
    /**
     * @brief Cleanup
     */
    void shutdown();
    
private:
    bool loadShaders();
    void createGroundMesh();
    void createPlayerMesh();
    void createNPCMesh();
    
    GLuint compileShader(const std::string& source, GLenum type);
    GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
    std::string loadShaderFile(const std::string& path);
    
    // Shader program
    GLuint m_shaderProgram = 0;
    
    // Ground mesh
    GLuint m_groundVAO = 0;
    GLuint m_groundVBO = 0;
    GLuint m_groundEBO = 0;
    int m_groundIndexCount = 0;
    
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
    
    // Lighting
    glm::vec3 m_lightPos = glm::vec3(10.0f, 20.0f, 10.0f);
    glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
};
