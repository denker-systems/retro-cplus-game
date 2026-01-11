/**
 * @file RuntimeRenderer.cpp
 * @brief Implementation av RuntimeRenderer
 */
#include "RuntimeRenderer.h"
#include "editor/viewport/3d/EditorCamera3D.h"
#include "engine/actors/Character3DActor.h"
#include "engine/actors/NPC3DActor.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/PlayerConfigActor.h"
#include "engine/actors/StaticMeshActor.h"
#include "engine/world/Scene.h"
#include "engine/utils/Logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <vector>

RuntimeRenderer::RuntimeRenderer() = default;

RuntimeRenderer::~RuntimeRenderer() {
    shutdown();
}

bool RuntimeRenderer::initialize() {
    LOG_INFO("[RuntimeRenderer] Initializing...");
    
    if (!loadShaders()) {
        LOG_ERROR("[RuntimeRenderer] Failed to load shaders");
        return false;
    }
    
    createGroundMesh();
    createPlayerMesh();
    createNPCMesh();
    
    LOG_INFO("[RuntimeRenderer] Initialized successfully");
    return true;
}

void RuntimeRenderer::render(editor::EditorCamera3D* camera, engine::Player3DActor* player, engine::Scene* scene) {
    if (!camera || !player) return;
    
    // Enable depth test and disable face culling (render all faces)
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);  // Show all faces of cubes
    
    glUseProgram(m_shaderProgram);
    
    // Set camera matrices
    glm::mat4 view = camera->getViewMatrix();
    glm::mat4 projection = camera->getProjectionMatrix();
    
    GLint viewLoc = glGetUniformLocation(m_shaderProgram, "uView");
    GLint projLoc = glGetUniformLocation(m_shaderProgram, "uProjection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Set lighting uniforms
    GLint lightPosLoc = glGetUniformLocation(m_shaderProgram, "uLightPos");
    GLint lightColorLoc = glGetUniformLocation(m_shaderProgram, "uLightColor");
    GLint viewPosLoc = glGetUniformLocation(m_shaderProgram, "uViewPos");
    
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(m_lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(m_lightColor));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(camera->getPosition()));
    
    // Use vertex colors for ground/player
    GLint useUniformColorLoc = glGetUniformLocation(m_shaderProgram, "uUseUniformColor");
    glUniform1i(useUniformColorLoc, 0);
    
    // Render grid (same as editor)
    {
        glm::mat4 model = glm::mat4(1.0f);
        // Grid at Y=0 (same as editor)
        
        GLint modelLoc = glGetUniformLocation(m_shaderProgram, "uModel");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(m_groundVAO);
        if (m_useGridLines) {
            glDrawElements(GL_LINES, m_groundIndexCount, GL_UNSIGNED_INT, 0);
        } else {
            glDrawElements(GL_TRIANGLES, m_groundIndexCount, GL_UNSIGNED_INT, 0);
        }
    }
    
    // Render player (same as editor: position + rotation + scale)
    {
        glm::vec3 playerPos = player->getPosition3D();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPos);
        model = glm::rotate(model, glm::radians(player->getYaw()), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around Y-axis
        model = glm::scale(model, glm::vec3(0.4f, 0.9f, 0.4f));  // Capsule-ish (same as editor)
        
        GLint modelLoc = glGetUniformLocation(m_shaderProgram, "uModel");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(m_playerVAO);
        glDrawElements(GL_TRIANGLES, m_playerIndexCount, GL_UNSIGNED_INT, 0);
    }
    
    // Render ALL actors (same as editor viewport)
    if (scene) {
        static bool logged = false;
        int actorCount = 0;
        int totalActors = static_cast<int>(scene->getActors().size());
        
        if (!logged) {
            LOG_INFO("[RuntimeRenderer] Scene has " + std::to_string(totalActors) + " actors");
        }
        
        GLint modelLoc = glGetUniformLocation(m_shaderProgram, "uModel");
        GLint colorLoc = glGetUniformLocation(m_shaderProgram, "uColor");
        
        for (const auto& actor : scene->getActors()) {
            if (!actor) continue;
            // Skip player (already rendered)
            if (actor.get() == player) continue;
            
            // Get position based on actor type - MATCH EDITOR EXACTLY
            glm::vec3 pos;
            float scale = 0.5f;  // Same as editor
            glm::vec3 color(0.6f, 0.6f, 0.6f);  // Default gray
            
            // MATCH EDITOR EXACTLY - render ALL actors with appropriate colors
            if (actor->getName() == "Background") {
                continue;  // Skip background actor (rendered separately)
            }
            
            // Get position based on actor type
            if (auto* char3d = dynamic_cast<engine::Character3DActor*>(actor.get())) {
                pos = char3d->getPosition3D();
                color = glm::vec3(0.8f, 0.5f, 0.5f);  // Pink for NPCs
            } else if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
                pos = npc->getPosition3D();
                color = glm::vec3(0.8f, 0.5f, 0.5f);  // Pink for NPCs
            } else if (auto* meshActor = dynamic_cast<engine::StaticMeshActor*>(actor.get())) {
                pos = meshActor->getPosition3D();
                color = meshActor->getMeshColor();
            } else if (auto* playerStart = dynamic_cast<engine::PlayerStartActor*>(actor.get())) {
                pos = playerStart->getSpawnPosition();
                color = glm::vec3(0.2f, 0.8f, 0.2f);  // Green for spawn
            } else if (auto* playerConfig = dynamic_cast<engine::PlayerConfigActor*>(actor.get())) {
                // Position at PlayerStart location, slightly below
                pos = glm::vec3(0.0f, 0.5f, 0.0f);
                for (const auto& a : scene->getActors()) {
                    if (auto* ps = dynamic_cast<engine::PlayerStartActor*>(a.get())) {
                        pos = ps->getSpawnPosition();
                        pos.y = 0.5f;
                        break;
                    }
                }
                color = glm::vec3(0.2f, 0.9f, 0.9f);  // Cyan for camera config
            } else {
                // Legacy 2D actor - convert to 3D (same as editor)
                auto pos2D = actor->getPosition();
                float posZ = actor->getZ();
                pos = glm::vec3(pos2D.x / 100.0f, posZ / 100.0f + scale / 2.0f, pos2D.y / 100.0f);
                // Vary color by index (same as editor)
                int index = actorCount;
                float hue = (float)index / std::max(1, (int)scene->getActors().size());
                color = glm::vec3(0.8f - hue * 0.3f, 0.4f + hue * 0.2f, 0.3f + hue * 0.4f);
            }
            
            if (!logged) {
                LOG_INFO("[RuntimeRenderer] Rendering '" + actor->getName() + "' at (" +
                         std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z) + ")");
            }
            
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(scale));
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glUniform3fv(colorLoc, 1, glm::value_ptr(color));
            glUniform1i(glGetUniformLocation(m_shaderProgram, "uUseUniformColor"), 1);
            
            glBindVertexArray(m_npcVAO);
            glDrawElements(GL_TRIANGLES, m_npcIndexCount, GL_UNSIGNED_INT, 0);
            actorCount++;
        }
        
        if (!logged && actorCount > 0) {
            LOG_INFO("[RuntimeRenderer] Rendered " + std::to_string(actorCount) + " actors");
            logged = true;
        }
    }
    
    glBindVertexArray(0);
    glUseProgram(0);
}

void RuntimeRenderer::shutdown() {
    if (m_groundVAO) glDeleteVertexArrays(1, &m_groundVAO);
    if (m_groundVBO) glDeleteBuffers(1, &m_groundVBO);
    if (m_groundEBO) glDeleteBuffers(1, &m_groundEBO);
    
    if (m_playerVAO) glDeleteVertexArrays(1, &m_playerVAO);
    if (m_playerVBO) glDeleteBuffers(1, &m_playerVBO);
    if (m_playerEBO) glDeleteBuffers(1, &m_playerEBO);
    
    if (m_npcVAO) glDeleteVertexArrays(1, &m_npcVAO);
    if (m_npcVBO) glDeleteBuffers(1, &m_npcVBO);
    if (m_npcEBO) glDeleteBuffers(1, &m_npcEBO);
    
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
}

bool RuntimeRenderer::loadShaders() {
    LOG_INFO("[RuntimeRenderer] Loading shaders...");
    
    // Hardcoded shaders (embedded in code)
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPosition;
        layout(location = 1) in vec3 aNormal;
        layout(location = 2) in vec3 aColor;
        
        out vec3 FragPos;
        out vec3 Normal;
        out vec3 Color;
        
        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;
        
        void main() {
            FragPos = vec3(uModel * vec4(aPosition, 1.0));
            Normal = mat3(transpose(inverse(uModel))) * aNormal;
            Color = aColor;
            gl_Position = uProjection * uView * vec4(FragPos, 1.0);
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec3 FragPos;
        in vec3 Normal;
        in vec3 Color;
        
        out vec4 FragColor;
        
        uniform vec3 uLightPos;
        uniform vec3 uViewPos;
        uniform vec3 uLightColor;
        uniform vec3 uColor;  // Override color (if set, use this instead of vertex color)
        uniform int uUseUniformColor;  // 1 = use uColor, 0 = use vertex Color
        
        void main() {
            float ambientStrength = 0.3;
            vec3 ambient = ambientStrength * uLightColor;
            
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(uLightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * uLightColor;
            
            float specularStrength = 0.5;
            vec3 viewDir = normalize(uViewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = specularStrength * spec * uLightColor;
            
            vec3 baseColor = (uUseUniformColor == 1) ? uColor : Color;
            vec3 result = (ambient + diffuse + specular) * baseColor;
            FragColor = vec4(result, 1.0);
        }
    )";
    
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    if (!vertexShader || !fragmentShader) {
        return false;
    }
    
    m_shaderProgram = linkProgram(vertexShader, fragmentShader);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return m_shaderProgram != 0;
}

void RuntimeRenderer::createGroundMesh() {
    LOG_INFO("[RuntimeRenderer] Creating grid mesh (same as editor)...");
    
    // Create grid lines (same as editor: Mesh::createGrid)
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    const int lines = 20;
    const float spacing = 1.0f;
    const float halfSize = (lines * spacing) * 0.5f;
    unsigned int index = 0;
    
    // Grid line color (same as editor: 0.5f gray)
    const float gridColor = 0.5f;
    
    // Lines along X axis
    for (int i = 0; i <= lines; ++i) {
        float z = -halfSize + i * spacing;
        
        // Start point
        vertices.push_back(-halfSize);  // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(z);          // z
        vertices.push_back(0.0f);       // normal x
        vertices.push_back(1.0f);       // normal y
        vertices.push_back(0.0f);       // normal z
        vertices.push_back(gridColor);  // color r
        vertices.push_back(gridColor);  // color g
        vertices.push_back(gridColor);  // color b
        indices.push_back(index++);
        
        // End point
        vertices.push_back(halfSize);   // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(z);          // z
        vertices.push_back(0.0f);       // normal x
        vertices.push_back(1.0f);       // normal y
        vertices.push_back(0.0f);       // normal z
        vertices.push_back(gridColor);  // color r
        vertices.push_back(gridColor);  // color g
        vertices.push_back(gridColor);  // color b
        indices.push_back(index++);
    }
    
    // Lines along Z axis
    for (int i = 0; i <= lines; ++i) {
        float x = -halfSize + i * spacing;
        
        // Start point
        vertices.push_back(x);          // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(-halfSize);  // z
        vertices.push_back(0.0f);       // normal x
        vertices.push_back(1.0f);       // normal y
        vertices.push_back(0.0f);       // normal z
        vertices.push_back(gridColor);  // color r
        vertices.push_back(gridColor);  // color g
        vertices.push_back(gridColor);  // color b
        indices.push_back(index++);
        
        // End point
        vertices.push_back(x);          // x
        vertices.push_back(0.0f);       // y
        vertices.push_back(halfSize);   // z
        vertices.push_back(0.0f);       // normal x
        vertices.push_back(1.0f);       // normal y
        vertices.push_back(0.0f);       // normal z
        vertices.push_back(gridColor);  // color r
        vertices.push_back(gridColor);  // color g
        vertices.push_back(gridColor);  // color b
        indices.push_back(index++);
    }
    
    m_groundIndexCount = indices.size();
    m_useGridLines = true;  // Flag to render as GL_LINES
    
    // Create VAO, VBO, EBO
    glGenVertexArrays(1, &m_groundVAO);
    glGenBuffers(1, &m_groundVBO);
    glGenBuffers(1, &m_groundEBO);
    
    glBindVertexArray(m_groundVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_groundVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Color attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    LOG_INFO("[RuntimeRenderer] Grid mesh created (" + std::to_string(m_groundIndexCount / 2) + " lines)");
}

void RuntimeRenderer::createPlayerMesh() {
    LOG_INFO("[RuntimeRenderer] Creating player mesh...");
    
    // Simple cube (will be scaled to capsule-ish shape)
    float vertices[] = {
        // Position          Normal            Color (green)
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.8f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.8f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.8f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.8f, 0.0f,
        
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.2f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.2f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.2f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.2f,
        
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.6f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.6f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.6f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.6f, 0.0f,
        
        // Right face
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.9f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.9f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.9f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.9f, 0.0f,
        
        // Left face
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.7f, 0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.7f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.7f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.7f, 0.0f,
    };
    
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,    // Front
        4, 6, 5,  6, 4, 7,    // Back
        8, 9, 10, 10, 11, 8,  // Top
        12, 14, 13, 14, 12, 15, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 22, 21, 22, 20, 23  // Left
    };
    
    m_playerIndexCount = 36;
    
    glGenVertexArrays(1, &m_playerVAO);
    glGenBuffers(1, &m_playerVBO);
    glGenBuffers(1, &m_playerEBO);
    
    glBindVertexArray(m_playerVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_playerVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_playerEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    LOG_INFO("[RuntimeRenderer] Player mesh created");
}

void RuntimeRenderer::createNPCMesh() {
    LOG_INFO("[RuntimeRenderer] Creating NPC mesh...");
    
    // Simple cube with pink/brown color (similar to editor NPCs)
    float vertices[] = {
        // Position          Normal            Color (pink/brown)
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.8f, 0.5f, 0.5f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.8f, 0.5f, 0.5f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.8f, 0.5f, 0.5f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.8f, 0.5f, 0.5f,
        
        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.7f, 0.4f, 0.4f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.7f, 0.4f, 0.4f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.7f, 0.4f, 0.4f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.7f, 0.4f, 0.4f,
        
        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.9f, 0.6f, 0.6f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.9f, 0.6f, 0.6f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.9f, 0.6f, 0.6f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.9f, 0.6f, 0.6f,
        
        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.6f, 0.3f, 0.3f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.6f, 0.3f, 0.3f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.6f, 0.3f, 0.3f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.6f, 0.3f, 0.3f,
        
        // Right face
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.75f, 0.45f, 0.45f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.75f, 0.45f, 0.45f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.75f, 0.45f, 0.45f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.75f, 0.45f, 0.45f,
        
        // Left face
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.65f, 0.35f, 0.35f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.65f, 0.35f, 0.35f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.65f, 0.35f, 0.35f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.65f, 0.35f, 0.35f,
    };
    
    unsigned int indices[] = {
        0, 1, 2,  2, 3, 0,    // Front
        4, 6, 5,  6, 4, 7,    // Back
        8, 9, 10, 10, 11, 8,  // Top
        12, 14, 13, 14, 12, 15, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 22, 21, 22, 20, 23  // Left
    };
    
    m_npcIndexCount = 36;
    
    glGenVertexArrays(1, &m_npcVAO);
    glGenBuffers(1, &m_npcVBO);
    glGenBuffers(1, &m_npcEBO);
    
    glBindVertexArray(m_npcVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_npcVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_npcEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Color
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
    
    LOG_INFO("[RuntimeRenderer] NPC mesh created");
}

GLuint RuntimeRenderer::compileShader(const std::string& source, GLenum type) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG_ERROR("[RuntimeRenderer] Shader compilation failed: " + std::string(infoLog));
        return 0;
    }
    
    return shader;
}

GLuint RuntimeRenderer::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("[RuntimeRenderer] Shader linking failed: " + std::string(infoLog));
        return 0;
    }
    
    return program;
}

std::string RuntimeRenderer::loadShaderFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("[RuntimeRenderer] Failed to open shader file: " + path);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
