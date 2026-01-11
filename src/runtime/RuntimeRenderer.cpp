/**
 * @file RuntimeRenderer.cpp
 * @brief Implementation av RuntimeRenderer
 */
#include "RuntimeRenderer.h"
#include "RuntimeWorld.h"
#include "RuntimeQuestSystem.h"
#include "editor/viewport/3d/EditorCamera3D.h"
#include "engine/actors/Character3DActor.h"
#include "engine/actors/NPC3DActor.h"
#include "engine/actors/PlayerStartActor.h"
#include "engine/actors/PlayerConfigActor.h"
#include "engine/actors/StaticMeshActor.h"
#include "engine/systems/WaypointSystem.h"
#include "engine/world/Scene.h"
#include "engine/utils/Logger.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/constants.hpp>
#include <fstream>
#include <sstream>
#include <vector>

#include <stb_image.h>

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
    
    if (!loadSpriteShader()) {
        LOG_WARNING("[RuntimeRenderer] Failed to load sprite shader - sprites disabled");
    }
    
    createGroundMesh();
    createPlayerMesh();
    createNPCMesh();
    createWireframeCubeMesh();
    createSpriteMesh();
    createBackgroundMesh();
    initTextRendering();
    
    LOG_INFO("[RuntimeRenderer] Initialized successfully");
    return true;
}

void RuntimeRenderer::render(editor::EditorCamera3D* camera, engine::Player3DActor* player, engine::Scene* scene,
                              const std::string& backgroundPath, bool showLabels,
                              const std::vector<ExitData>* exits) {
    if (!camera || !player) return;
    
    glm::vec3 playerPos = player->getPosition3D();
    
    // Render background first (behind everything)
    if (!backgroundPath.empty()) {
        renderBackground(backgroundPath);
    }
    
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
            if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
                pos = npc->getPosition3D();
                
                // Render NPC name label
                if (showLabels && m_font) {
                    float distToPlayer = glm::length(playerPos - pos);
                    glm::vec3 labelPos = pos + glm::vec3(0, 1.5f, 0);
                    
                    // Always show name
                    renderWorldLabel(npc->getName(), labelPos, view, projection, glm::vec3(1.0f, 1.0f, 0.5f));
                    
                    // Show [E] Interact if close
                    if (distToPlayer < 3.0f) {
                        renderWorldLabel("[E] Interact", labelPos + glm::vec3(0, 0.4f, 0), view, projection, glm::vec3(0.5f, 1.0f, 0.5f));
                    }
                }
                
                // Try to render as sprite if sprite name exists
                std::string spriteName = npc->getSpriteName();
                if (!spriteName.empty() && m_spriteShader) {
                    renderSprite(pos + glm::vec3(0, 0.5f, 0), spriteName, view, projection);
                    actorCount++;
                    continue;
                }
                color = glm::vec3(0.8f, 0.5f, 0.5f);  // Pink for NPCs (fallback)
            } else if (auto* char3d = dynamic_cast<engine::Character3DActor*>(actor.get())) {
                pos = char3d->getPosition3D();
                color = glm::vec3(0.8f, 0.5f, 0.5f);  // Pink for characters
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
    
    // Render exits with labels
    if (exits && showLabels) {
        static bool exitLogged = false;
        if (!exitLogged && !exits->empty()) {
            LOG_INFO("[RuntimeRenderer] Rendering " + std::to_string(exits->size()) + " exits");
            for (const auto& e : *exits) {
                LOG_INFO("[RuntimeRenderer] Exit: " + e.name + " at (" + 
                         std::to_string(e.position.x) + ", " + std::to_string(e.position.y) + ", " + 
                         std::to_string(e.position.z) + ") -> " + 
                         (e.isLevelTransition() ? e.targetLevel + "/" : "") + e.targetScene);
            }
            exitLogged = true;
        }
        
        GLint modelLoc = glGetUniformLocation(m_shaderProgram, "uModel");
        GLint colorLoc = glGetUniformLocation(m_shaderProgram, "uColor");
        
        for (const auto& exit : *exits) {
            glm::vec3 pos = exit.position;
            float distToPlayer = glm::length(playerPos - pos);
            
            // Render exit cube (blue)
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pos);
            model = glm::scale(model, glm::vec3(0.4f, 0.6f, 0.4f));  // Door-shaped
            
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glm::vec3 exitColor = exit.isLevelTransition() 
                ? glm::vec3(0.8f, 0.3f, 0.8f)  // Purple for level transitions
                : glm::vec3(0.3f, 0.5f, 0.9f); // Blue for scene transitions
            glUniform3fv(colorLoc, 1, glm::value_ptr(exitColor));
            glUniform1i(glGetUniformLocation(m_shaderProgram, "uUseUniformColor"), 1);
            
            glBindVertexArray(m_npcVAO);
            glDrawElements(GL_TRIANGLES, m_npcIndexCount, GL_UNSIGNED_INT, 0);
            
            // Render labels
            if (m_font) {
                glm::vec3 labelPos = pos + glm::vec3(0, 0.8f, 0);
                
                // Show exit name
                std::string label = exit.isLevelTransition() 
                    ? exit.name + " [Level]"
                    : exit.name;
                renderWorldLabel(label, labelPos, view, projection, glm::vec3(0.7f, 0.7f, 1.0f));
                
                // Show [E] Use if close
                if (distToPlayer < exit.interactRadius) {
                    renderWorldLabel("[E] Use Exit", labelPos + glm::vec3(0, 0.35f, 0), view, projection, glm::vec3(0.5f, 1.0f, 0.5f));
                }
            }
            
            // Render interaction volume wireframe (cyan for exits)
            float radius = exit.interactRadius;
            glm::vec3 volumeColor = exit.isLevelTransition() 
                ? glm::vec3(0.8f, 0.4f, 0.8f)  // Purple for level transitions
                : glm::vec3(0.3f, 0.7f, 1.0f); // Cyan for scene transitions
            renderWireframeCube(pos, glm::vec3(radius * 2.0f, 1.5f, radius * 2.0f), volumeColor, view, projection);
        }
    }
    
    // Render NPC interaction volumes (after solid actors)
    if (scene) {
        for (const auto& actor : scene->getActors()) {
            if (auto* npc = dynamic_cast<engine::NPC3DActor*>(actor.get())) {
                glm::vec3 pos = npc->getPosition3D();
                glm::vec3 volumeSize = npc->getInteractionVolume();
                glm::vec3 volumeColor(1.0f, 0.8f, 0.3f);  // Yellow/orange for NPCs
                renderWireframeCube(pos, volumeSize, volumeColor, view, projection);
            }
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
    
    if (m_wireframeCubeVAO) glDeleteVertexArrays(1, &m_wireframeCubeVAO);
    if (m_wireframeCubeVBO) glDeleteBuffers(1, &m_wireframeCubeVBO);
    if (m_wireframeCubeEBO) glDeleteBuffers(1, &m_wireframeCubeEBO);
    
    if (m_spriteVAO) glDeleteVertexArrays(1, &m_spriteVAO);
    if (m_spriteVBO) glDeleteBuffers(1, &m_spriteVBO);
    if (m_spriteEBO) glDeleteBuffers(1, &m_spriteEBO);
    
    // Delete textures
    for (auto& pair : m_textures) {
        glDeleteTextures(1, &pair.second);
    }
    m_textures.clear();
    
    if (m_bgVAO) glDeleteVertexArrays(1, &m_bgVAO);
    if (m_bgVBO) glDeleteBuffers(1, &m_bgVBO);
    
    // Delete text textures
    for (auto& pair : m_textTextures) {
        glDeleteTextures(1, &pair.second);
    }
    m_textTextures.clear();
    
    if (m_font) {
        TTF_CloseFont(m_font);
        m_font = nullptr;
    }
    
    if (m_shaderProgram) glDeleteProgram(m_shaderProgram);
    if (m_spriteShader) glDeleteProgram(m_spriteShader);
    if (m_bgShader) glDeleteProgram(m_bgShader);
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

void RuntimeRenderer::createWireframeCubeMesh() {
    // Unit cube vertices (corners)
    float vertices[] = {
        // Position only (no normals/colors for wireframe)
        -0.5f, -0.5f, -0.5f,  // 0: back-bottom-left
         0.5f, -0.5f, -0.5f,  // 1: back-bottom-right
         0.5f,  0.5f, -0.5f,  // 2: back-top-right
        -0.5f,  0.5f, -0.5f,  // 3: back-top-left
        -0.5f, -0.5f,  0.5f,  // 4: front-bottom-left
         0.5f, -0.5f,  0.5f,  // 5: front-bottom-right
         0.5f,  0.5f,  0.5f,  // 6: front-top-right
        -0.5f,  0.5f,  0.5f,  // 7: front-top-left
    };
    
    // Line indices (12 edges of a cube)
    unsigned int indices[] = {
        // Back face
        0, 1,  1, 2,  2, 3,  3, 0,
        // Front face
        4, 5,  5, 6,  6, 7,  7, 4,
        // Connecting edges
        0, 4,  1, 5,  2, 6,  3, 7
    };
    
    m_wireframeCubeIndexCount = 24;
    
    glGenVertexArrays(1, &m_wireframeCubeVAO);
    glGenBuffers(1, &m_wireframeCubeVBO);
    glGenBuffers(1, &m_wireframeCubeEBO);
    
    glBindVertexArray(m_wireframeCubeVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_wireframeCubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_wireframeCubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    LOG_INFO("[RuntimeRenderer] Wireframe cube mesh created");
}

void RuntimeRenderer::renderWireframeCube(const glm::vec3& position, const glm::vec3& size, 
                                          const glm::vec3& color, const glm::mat4& view, 
                                          const glm::mat4& projection) {
    glUseProgram(m_shaderProgram);
    
    // Create model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, size);
    
    // Set uniforms
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uView"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uProjection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniform3fv(glGetUniformLocation(m_shaderProgram, "uColor"), 1, glm::value_ptr(color));
    glUniform1i(glGetUniformLocation(m_shaderProgram, "uUseUniformColor"), 1);
    
    // Draw wireframe
    glBindVertexArray(m_wireframeCubeVAO);
    glDrawElements(GL_LINES, m_wireframeCubeIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
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

bool RuntimeRenderer::loadSpriteShader() {
    LOG_INFO("[RuntimeRenderer] Loading sprite shader...");
    
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec3 aPosition;
        layout(location = 1) in vec2 aTexCoord;
        
        out vec2 TexCoord;
        
        uniform mat4 uMVP;
        
        void main() {
            gl_Position = uMVP * vec4(aPosition, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* fragmentShaderSource = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        
        uniform sampler2D uTexture;
        
        void main() {
            vec4 texColor = texture(uTexture, TexCoord);
            if (texColor.a < 0.1) discard;  // Alpha cutoff
            FragColor = texColor;
        }
    )";
    
    GLuint vertexShader = compileShader(vertexShaderSource, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);
    
    if (!vertexShader || !fragmentShader) {
        return false;
    }
    
    m_spriteShader = linkProgram(vertexShader, fragmentShader);
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    return m_spriteShader != 0;
}

void RuntimeRenderer::createSpriteMesh() {
    LOG_INFO("[RuntimeRenderer] Creating sprite mesh...");
    
    // Billboard quad (1x1 centered at origin)
    float vertices[] = {
        // Position          TexCoord
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f,  // Bottom-left
         0.5f, -0.5f, 0.0f,  1.0f, 1.0f,  // Bottom-right
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f,  // Top-right
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f   // Top-left
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &m_spriteVAO);
    glGenBuffers(1, &m_spriteVBO);
    glGenBuffers(1, &m_spriteEBO);
    
    glBindVertexArray(m_spriteVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_spriteEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // TexCoord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    LOG_INFO("[RuntimeRenderer] Sprite mesh created");
}

GLuint RuntimeRenderer::loadTexture(const std::string& path) {
    // Check cache
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }
    
    // Load with stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);
    
    if (!data) {
        LOG_ERROR("[RuntimeRenderer] Failed to load texture: " + path);
        return 0;
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    stbi_image_free(data);
    
    m_textures[path] = textureID;
    LOG_INFO("[RuntimeRenderer] Loaded texture: " + path + " (" + std::to_string(width) + "x" + std::to_string(height) + ")");
    
    return textureID;
}

void RuntimeRenderer::renderSprite(const glm::vec3& position, const std::string& spriteName,
                                    const glm::mat4& view, const glm::mat4& projection) {
    if (!m_spriteShader || !m_spriteVAO) return;
    
    // Build texture path
    std::string texturePath = "assets/sprites/" + spriteName + ".png";
    GLuint textureID = loadTexture(texturePath);
    
    if (textureID == 0) return;
    
    glUseProgram(m_spriteShader);
    
    // Billboard: Extract camera right and up vectors from view matrix
    glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
    glm::vec3 cameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);
    
    // Build billboard model matrix
    float scale = 1.0f;  // Sprite size in world units
    glm::mat4 model = glm::mat4(1.0f);
    model[0] = glm::vec4(cameraRight * scale, 0.0f);
    model[1] = glm::vec4(cameraUp * scale, 0.0f);
    model[2] = glm::vec4(glm::cross(cameraRight, cameraUp) * scale, 0.0f);
    model[3] = glm::vec4(position, 1.0f);
    
    glm::mat4 mvp = projection * view * model;
    
    glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    
    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(m_spriteShader, "uTexture"), 0);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glBindVertexArray(m_spriteVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glDisable(GL_BLEND);
}

void RuntimeRenderer::createBackgroundMesh() {
    LOG_INFO("[RuntimeRenderer] Creating background mesh...");
    
    // Fullscreen quad in NDC (-1 to 1)
    float vertices[] = {
        // Position        TexCoord
        -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,  // Bottom-left
         1.0f, -1.0f, 0.0f,  1.0f, 0.0f,  // Bottom-right
         1.0f,  1.0f, 0.0f,  1.0f, 1.0f,  // Top-right
        -1.0f,  1.0f, 0.0f,  0.0f, 1.0f   // Top-left
    };
    
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    
    glGenVertexArrays(1, &m_bgVAO);
    glGenBuffers(1, &m_bgVBO);
    
    glBindVertexArray(m_bgVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_bgVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // TexCoord
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    // Background shader (simple texture passthrough)
    const char* bgVertexShader = R"(
        #version 330 core
        layout(location = 0) in vec3 aPosition;
        layout(location = 1) in vec2 aTexCoord;
        out vec2 TexCoord;
        void main() {
            gl_Position = vec4(aPosition, 1.0);
            TexCoord = aTexCoord;
        }
    )";
    
    const char* bgFragmentShader = R"(
        #version 330 core
        in vec2 TexCoord;
        out vec4 FragColor;
        uniform sampler2D uTexture;
        void main() {
            FragColor = texture(uTexture, TexCoord);
        }
    )";
    
    GLuint vs = compileShader(bgVertexShader, GL_VERTEX_SHADER);
    GLuint fs = compileShader(bgFragmentShader, GL_FRAGMENT_SHADER);
    m_bgShader = linkProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    LOG_INFO("[RuntimeRenderer] Background mesh created");
}

void RuntimeRenderer::renderBackground(const std::string& backgroundPath) {
    if (!m_bgShader || !m_bgVAO || backgroundPath.empty()) return;
    
    GLuint textureID = loadTexture(backgroundPath);
    if (textureID == 0) return;
    
    // Disable depth test for background
    glDisable(GL_DEPTH_TEST);
    
    glUseProgram(m_bgShader);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(m_bgShader, "uTexture"), 0);
    
    glBindVertexArray(m_bgVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Re-enable depth test
    glEnable(GL_DEPTH_TEST);
}

void RuntimeRenderer::initTextRendering() {
    if (TTF_WasInit() == 0) {
        if (TTF_Init() == -1) {
            LOG_ERROR("[RuntimeRenderer] TTF_Init failed: " + std::string(TTF_GetError()));
            return;
        }
    }
    
    m_font = TTF_OpenFont("assets/fonts/arial.ttf", 16);
    if (!m_font) {
        LOG_WARNING("[RuntimeRenderer] Failed to load font, trying default path");
        m_font = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 16);
    }
    
    if (m_font) {
        LOG_INFO("[RuntimeRenderer] Font loaded for labels");
    } else {
        LOG_WARNING("[RuntimeRenderer] No font available for labels");
    }
}

void RuntimeRenderer::renderWorldLabel(const std::string& text, const glm::vec3& worldPos,
                                        const glm::mat4& view, const glm::mat4& projection,
                                        const glm::vec3& color) {
    if (!m_font || !m_spriteShader || text.empty()) return;
    
    // Check if we have cached texture for this text
    std::string cacheKey = text + "_" + std::to_string((int)(color.r * 255)) + 
                           "_" + std::to_string((int)(color.g * 255)) + 
                           "_" + std::to_string((int)(color.b * 255));
    
    GLuint textureID = 0;
    auto it = m_textTextures.find(cacheKey);
    
    if (it != m_textTextures.end()) {
        textureID = it->second;
    } else {
        // Render text to surface
        SDL_Color sdlColor = {
            (Uint8)(color.r * 255),
            (Uint8)(color.g * 255),
            (Uint8)(color.b * 255),
            255
        };
        
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, text.c_str(), sdlColor);
        if (!surface) return;
        
        // Convert to RGBA
        SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
        SDL_FreeSurface(surface);
        if (!rgbaSurface) return;
        
        // Create OpenGL texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgbaSurface->w, rgbaSurface->h, 
                     0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);
        
        m_textTextures[cacheKey] = textureID;
        SDL_FreeSurface(rgbaSurface);
    }
    
    if (textureID == 0) return;
    
    glUseProgram(m_spriteShader);
    
    // Billboard facing camera
    glm::vec3 cameraRight = glm::vec3(view[0][0], view[1][0], view[2][0]);
    glm::vec3 cameraUp = glm::vec3(view[0][1], view[1][1], view[2][1]);
    
    // Scale based on text length
    float scaleX = 0.5f + text.length() * 0.05f;
    float scaleY = 0.3f;
    
    glm::mat4 model = glm::mat4(1.0f);
    model[0] = glm::vec4(cameraRight * scaleX, 0.0f);
    model[1] = glm::vec4(cameraUp * scaleY, 0.0f);
    model[2] = glm::vec4(glm::cross(cameraRight, cameraUp), 0.0f);
    model[3] = glm::vec4(worldPos, 1.0f);
    
    glm::mat4 mvp = projection * view * model;
    glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(m_spriteShader, "uTexture"), 0);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    
    glBindVertexArray(m_spriteVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void RuntimeRenderer::renderDialogUI(const std::string& speaker, const std::string& text,
                                      const std::vector<std::pair<int, std::string>>& choices,
                                      int windowWidth, int windowHeight) {
    if (!m_font) return;
    
    // Save OpenGL state
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Setup orthographic projection for 2D UI
    glUseProgram(m_bgShader);
    
    // Dialog box dimensions
    float boxHeight = 150.0f;
    float boxY = windowHeight - boxHeight - 20.0f;
    float boxX = 20.0f;
    float boxWidth = windowWidth - 40.0f;
    float padding = 15.0f;
    
    // Render dark semi-transparent background box
    // We'll use a simple colored quad
    float vertices[] = {
        // Position (NDC)                          // UV
        (boxX / windowWidth) * 2.0f - 1.0f,       1.0f - (boxY / windowHeight) * 2.0f,  0.0f, 0.0f,
        ((boxX + boxWidth) / windowWidth) * 2.0f - 1.0f, 1.0f - (boxY / windowHeight) * 2.0f, 1.0f, 0.0f,
        ((boxX + boxWidth) / windowWidth) * 2.0f - 1.0f, 1.0f - ((boxY + boxHeight) / windowHeight) * 2.0f, 1.0f, 1.0f,
        (boxX / windowWidth) * 2.0f - 1.0f,       1.0f - ((boxY + boxHeight) / windowHeight) * 2.0f, 0.0f, 1.0f
    };
    
    // Create temporary VAO/VBO for dialog box
    GLuint dialogVAO, dialogVBO;
    glGenVertexArrays(1, &dialogVAO);
    glGenBuffers(1, &dialogVBO);
    
    glBindVertexArray(dialogVAO);
    glBindBuffer(GL_ARRAY_BUFFER, dialogVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Render background box (dark blue)
    // Create a 1x1 dark texture for the box
    GLuint boxTexture;
    glGenTextures(1, &boxTexture);
    glBindTexture(GL_TEXTURE_2D, boxTexture);
    unsigned char darkBlue[] = { 20, 30, 60, 220 };  // RGBA
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, darkBlue);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glUniform1i(glGetUniformLocation(m_bgShader, "uTexture"), 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    glDeleteTextures(1, &boxTexture);
    glDeleteBuffers(1, &dialogVBO);
    glDeleteVertexArrays(1, &dialogVAO);
    
    // Render text using sprite shader (billboard text)
    glUseProgram(m_spriteShader);
    
    // Helper lambda to render 2D text
    auto renderText2D = [&](const std::string& str, float x, float y, const glm::vec3& color, float scale = 1.0f) {
        if (str.empty()) return;
        
        std::string cacheKey = str + "_2d_" + std::to_string((int)(color.r * 255)) + 
                               "_" + std::to_string((int)(color.g * 255)) + 
                               "_" + std::to_string((int)(color.b * 255));
        
        GLuint textureID = 0;
        auto it = m_textTextures.find(cacheKey);
        
        if (it != m_textTextures.end()) {
            textureID = it->second;
        } else {
            SDL_Color sdlColor = { (Uint8)(color.r * 255), (Uint8)(color.g * 255), (Uint8)(color.b * 255), 255 };
            SDL_Surface* surface = TTF_RenderText_Blended(m_font, str.c_str(), sdlColor);
            if (!surface) return;
            
            SDL_Surface* rgbaSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
            SDL_FreeSurface(surface);
            if (!rgbaSurface) return;
            
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rgbaSurface->w, rgbaSurface->h, 
                         0, GL_RGBA, GL_UNSIGNED_BYTE, rgbaSurface->pixels);
            
            m_textTextures[cacheKey] = textureID;
            SDL_FreeSurface(rgbaSurface);
        }
        
        if (textureID == 0) return;
        
        // Get text dimensions
        int textW, textH;
        TTF_SizeText(m_font, str.c_str(), &textW, &textH);
        textW = (int)(textW * scale);
        textH = (int)(textH * scale);
        
        // Convert to NDC
        float ndcX = (x / windowWidth) * 2.0f - 1.0f;
        float ndcY = 1.0f - (y / windowHeight) * 2.0f;
        float ndcW = (textW / (float)windowWidth) * 2.0f;
        float ndcH = (textH / (float)windowHeight) * 2.0f;
        
        // Create MVP for 2D quad
        glm::mat4 mvp = glm::mat4(1.0f);
        mvp = glm::translate(mvp, glm::vec3(ndcX + ndcW / 2, ndcY - ndcH / 2, 0.0f));
        mvp = glm::scale(mvp, glm::vec3(ndcW / 2, ndcH / 2, 1.0f));
        
        glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(m_spriteShader, "uTexture"), 0);
        
        glBindVertexArray(m_spriteVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    };
    
    // Render speaker name (yellow)
    float textY = boxY + padding;
    renderText2D(speaker, boxX + padding, textY, glm::vec3(1.0f, 0.9f, 0.3f), 1.2f);
    
    // Render dialog text (white)
    textY += 30.0f;
    renderText2D(text, boxX + padding, textY, glm::vec3(1.0f, 1.0f, 1.0f));
    
    // Render choices (cyan, numbered)
    if (!choices.empty()) {
        textY += 35.0f;
        for (size_t i = 0; i < choices.size(); i++) {
            std::string choiceText = "[" + std::to_string(i + 1) + "] " + choices[i].second;
            renderText2D(choiceText, boxX + padding + 10.0f, textY, glm::vec3(0.5f, 1.0f, 1.0f));
            textY += 22.0f;
        }
    } else {
        // Show "Press E to continue" if no choices
        textY += 35.0f;
        renderText2D("[E] Continue...", boxX + padding + 10.0f, textY, glm::vec3(0.7f, 0.7f, 0.7f));
    }
    
    // Restore OpenGL state
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

// ============================================================================
// COMPASS / WAYPOINT RENDERING
// ============================================================================

void RuntimeRenderer::renderCompass(int windowWidth, int windowHeight, float playerYaw) {
    if (!m_font) return;
    
    // Get waypoints from WaypointSystem
    auto& waypointSystem = engine::WaypointSystem::instance();
    auto waypoints = waypointSystem.getCompassWaypoints();
    
    if (waypoints.empty()) return;
    
    // Setup 2D rendering
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Compass bar dimensions
    float compassWidth = 400.0f;
    float compassHeight = 40.0f;
    float compassX = (windowWidth - compassWidth) / 2.0f;
    float compassY = 20.0f;
    
    // Draw compass background
    // Using sprite shader for 2D rendering
    glUseProgram(m_spriteShader);
    
    // Render each waypoint indicator on compass
    for (const auto* wp : waypoints) {
        if (!wp->showOnCompass) continue;
        
        // Calculate position on compass based on angle
        float normalizedAngle = wp->angleToPlayer / glm::pi<float>();  // -1 to 1
        
        // Only show if within view (-1 to 1 range)
        if (normalizedAngle < -1.0f || normalizedAngle > 1.0f) continue;
        
        float indicatorX = compassX + compassWidth / 2.0f + normalizedAngle * (compassWidth / 2.0f);
        float indicatorY = compassY + compassHeight / 2.0f;
        
        // Distance text
        std::string distText = std::to_string((int)wp->distanceToPlayer) + "m";
        
        // Render indicator (colored dot/marker)
        // For now, render text label with distance
        SDL_Color sdlColor = {
            (Uint8)(wp->color.r * 255),
            (Uint8)(wp->color.g * 255),
            (Uint8)(wp->color.b * 255),
            255
        };
        
        // Render waypoint label
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, "▼", sdlColor);
        if (surface) {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, 
                        GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            
            float ndcX = (indicatorX / windowWidth) * 2.0f - 1.0f;
            float ndcY = 1.0f - (indicatorY / windowHeight) * 2.0f;
            float ndcW = (surface->w / (float)windowWidth) * 2.0f;
            float ndcH = (surface->h / (float)windowHeight) * 2.0f;
            
            glm::mat4 mvp = glm::mat4(1.0f);
            mvp = glm::translate(mvp, glm::vec3(ndcX, ndcY, 0.0f));
            mvp = glm::scale(mvp, glm::vec3(ndcW / 2, ndcH / 2, 1.0f));
            
            glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glUniform1i(glGetUniformLocation(m_spriteShader, "uTexture"), 0);
            
            glBindVertexArray(m_spriteVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            
            SDL_FreeSurface(surface);
            glDeleteTextures(1, &textureID);
        }
        
        // Render distance text below indicator
        renderScreenText(distText, (int)indicatorX, (int)(indicatorY + 15), wp->color, true);
    }
    
    // Render primary waypoint info (bottom of compass)
    const auto* primary = waypointSystem.getPrimaryWaypoint();
    if (primary) {
        std::string info = primary->label + " - " + std::to_string((int)primary->distanceToPlayer) + "m";
        renderScreenText(info, windowWidth / 2, (int)(compassY + compassHeight + 5), primary->color, true);
    }
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void RuntimeRenderer::renderWaypointMarker(const glm::vec3& waypointPos, const glm::vec3& color,
                                           const std::string& label, float distance,
                                           const glm::mat4& view, const glm::mat4& projection) {
    // Render 3D marker at waypoint position
    glm::vec3 markerColor = color;
    
    // Pulsing effect based on time
    float pulse = 0.8f + 0.2f * sin(SDL_GetTicks() / 200.0f);
    markerColor *= pulse;
    
    // Render wireframe diamond/beacon
    glm::vec3 markerPos = waypointPos + glm::vec3(0.0f, 1.5f, 0.0f);  // Float above ground
    renderWireframeCube(markerPos, glm::vec3(0.5f, 1.0f, 0.5f), markerColor, view, projection);
    
    // Render label above marker
    glm::vec3 labelPos = markerPos + glm::vec3(0.0f, 1.0f, 0.0f);
    std::string fullLabel = label + " (" + std::to_string((int)distance) + "m)";
    renderWorldLabel(fullLabel, labelPos, view, projection, color);
}

void RuntimeRenderer::renderScreenText(const std::string& text, int x, int y, 
                                        const glm::vec3& color, bool centered) {
    if (!m_font || text.empty()) return;
    
    SDL_Color sdlColor = {
        (Uint8)(color.r * 255),
        (Uint8)(color.g * 255),
        (Uint8)(color.b * 255),
        255
    };
    
    SDL_Surface* surface = TTF_RenderText_Blended(m_font, text.c_str(), sdlColor);
    if (!surface) return;
    
    if (centered) {
        x -= surface->w / 2;
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, 
                GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Get window dimensions
    int windowWidth, windowHeight;
    SDL_GL_GetDrawableSize(SDL_GL_GetCurrentWindow(), &windowWidth, &windowHeight);
    
    float ndcX = (x / (float)windowWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (y / (float)windowHeight) * 2.0f;
    float ndcW = (surface->w / (float)windowWidth) * 2.0f;
    float ndcH = (surface->h / (float)windowHeight) * 2.0f;
    
    glUseProgram(m_spriteShader);
    
    glm::mat4 mvp = glm::mat4(1.0f);
    mvp = glm::translate(mvp, glm::vec3(ndcX + ndcW / 2, ndcY - ndcH / 2, 0.0f));
    mvp = glm::scale(mvp, glm::vec3(ndcW / 2, ndcH / 2, 1.0f));
    
    glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(m_spriteShader, "uTexture"), 0);
    
    glBindVertexArray(m_spriteVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    SDL_FreeSurface(surface);
    glDeleteTextures(1, &textureID);
}

// ============================================================================
// IN-GAME QUEST LOG UI
// ============================================================================

void RuntimeRenderer::renderQuestLog(RuntimeQuestSystem* questSystem, int windowWidth, int windowHeight) {
    if (!m_font || !questSystem) return;
    
    // Setup 2D rendering
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Quest log panel dimensions
    float panelWidth = 400.0f;
    float panelHeight = 500.0f;
    float panelX = (windowWidth - panelWidth) / 2.0f;
    float panelY = (windowHeight - panelHeight) / 2.0f;
    float padding = 15.0f;
    
    // Draw semi-transparent background
    glUseProgram(m_shaderProgram);
    glm::mat4 model = glm::mat4(1.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_shaderProgram, "uModel"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform3f(glGetUniformLocation(m_shaderProgram, "uColor"), 0.1f, 0.1f, 0.15f);
    glUniform1i(glGetUniformLocation(m_shaderProgram, "uUseUniformColor"), 1);
    
    // Helper to render text
    auto renderText2D = [&](const std::string& str, float x, float y, const glm::vec3& color, float scale = 1.0f) {
        if (str.empty()) return;
        
        SDL_Color sdlColor = { (Uint8)(color.r * 255), (Uint8)(color.g * 255), (Uint8)(color.b * 255), 255 };
        SDL_Surface* surface = TTF_RenderText_Blended(m_font, str.c_str(), sdlColor);
        if (!surface) return;
        
        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, 
                    GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        int textW = (int)(surface->w * scale);
        int textH = (int)(surface->h * scale);
        
        float ndcX = (x / windowWidth) * 2.0f - 1.0f;
        float ndcY = 1.0f - (y / windowHeight) * 2.0f;
        float ndcW = (textW / (float)windowWidth) * 2.0f;
        float ndcH = (textH / (float)windowHeight) * 2.0f;
        
        glUseProgram(m_spriteShader);
        glm::mat4 mvp = glm::mat4(1.0f);
        mvp = glm::translate(mvp, glm::vec3(ndcX + ndcW / 2, ndcY - ndcH / 2, 0.0f));
        mvp = glm::scale(mvp, glm::vec3(ndcW / 2, ndcH / 2, 1.0f));
        
        glUniformMatrix4fv(glGetUniformLocation(m_spriteShader, "uMVP"), 1, GL_FALSE, glm::value_ptr(mvp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(m_spriteShader, "uTexture"), 0);
        
        glBindVertexArray(m_spriteVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        SDL_FreeSurface(surface);
        glDeleteTextures(1, &textureID);
    };
    
    // Title
    float textY = panelY + padding;
    renderText2D("=== QUEST LOG ===", panelX + panelWidth / 2 - 80, textY, glm::vec3(1.0f, 0.9f, 0.3f), 1.3f);
    textY += 40.0f;
    
    // Get active quests
    auto activeQuests = questSystem->getActiveQuests();
    
    if (activeQuests.empty()) {
        renderText2D("No active quests", panelX + padding, textY, glm::vec3(0.6f, 0.6f, 0.6f));
    } else {
        renderText2D("Active Quests:", panelX + padding, textY, glm::vec3(0.5f, 1.0f, 0.5f));
        textY += 25.0f;
        
        for (const auto* quest : activeQuests) {
            if (!quest) continue;
            
            // Quest title
            renderText2D(quest->title, panelX + padding + 10, textY, glm::vec3(1.0f, 1.0f, 1.0f));
            textY += 22.0f;
            
            // Objectives
            for (const auto& obj : quest->objectives) {
                std::string marker = obj.completed ? "[X] " : "[ ] ";
                glm::vec3 objColor = obj.completed ? glm::vec3(0.5f, 0.5f, 0.5f) : glm::vec3(0.8f, 0.8f, 0.8f);
                
                std::string objText = marker + obj.description;
                if (obj.optional) objText += " (Optional)";
                
                renderText2D(objText, panelX + padding + 25, textY, objColor, 0.9f);
                textY += 18.0f;
                
                // Avoid overflow
                if (textY > panelY + panelHeight - 50) break;
            }
            
            textY += 10.0f;  // Space between quests
            
            if (textY > panelY + panelHeight - 50) {
                renderText2D("...", panelX + padding, textY, glm::vec3(0.6f, 0.6f, 0.6f));
                break;
            }
        }
    }
    
    // Footer
    renderText2D("Press J to close", panelX + panelWidth / 2 - 60, panelY + panelHeight - 25, glm::vec3(0.5f, 0.5f, 0.5f), 0.8f);
    
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}
