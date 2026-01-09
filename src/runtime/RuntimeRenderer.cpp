/**
 * @file RuntimeRenderer.cpp
 * @brief Implementation av RuntimeRenderer
 */
#include "RuntimeRenderer.h"
#include "editor/viewport/3d/EditorCamera3D.h"
#include "engine/actors/Character3DActor.h"
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
    
    LOG_INFO("[RuntimeRenderer] Initialized successfully");
    return true;
}

void RuntimeRenderer::render(editor::EditorCamera3D* camera, engine::Player3DActor* player) {
    if (!camera || !player) return;
    
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
    
    // Render ground
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
        
        GLint modelLoc = glGetUniformLocation(m_shaderProgram, "uModel");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(m_groundVAO);
        glDrawElements(GL_TRIANGLES, m_groundIndexCount, GL_UNSIGNED_INT, 0);
    }
    
    // Render player
    {
        glm::vec3 playerPos = player->getPosition3D();
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, playerPos);
        model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f)); // 0.5x1x0.5m capsule-ish
        
        GLint modelLoc = glGetUniformLocation(m_shaderProgram, "uModel");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        
        glBindVertexArray(m_playerVAO);
        glDrawElements(GL_TRIANGLES, m_playerIndexCount, GL_UNSIGNED_INT, 0);
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
            
            vec3 result = (ambient + diffuse + specular) * Color;
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
    LOG_INFO("[RuntimeRenderer] Creating ground mesh...");
    
    // Create a large grid plane (20x20 meters)
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    
    const int gridSize = 20;
    const float cellSize = 1.0f;
    const float halfSize = gridSize * cellSize * 0.5f;
    
    // Generate vertices (position, normal, color)
    for (int z = 0; z <= gridSize; ++z) {
        for (int x = 0; x <= gridSize; ++x) {
            float xPos = x * cellSize - halfSize;
            float zPos = z * cellSize - halfSize;
            
            // Position
            vertices.push_back(xPos);
            vertices.push_back(0.0f);
            vertices.push_back(zPos);
            
            // Normal (up)
            vertices.push_back(0.0f);
            vertices.push_back(1.0f);
            vertices.push_back(0.0f);
            
            // Color (gray with grid pattern)
            float color = ((x + z) % 2 == 0) ? 0.3f : 0.4f;
            vertices.push_back(color);
            vertices.push_back(color);
            vertices.push_back(color);
        }
    }
    
    // Generate indices
    for (int z = 0; z < gridSize; ++z) {
        for (int x = 0; x < gridSize; ++x) {
            int topLeft = z * (gridSize + 1) + x;
            int topRight = topLeft + 1;
            int bottomLeft = (z + 1) * (gridSize + 1) + x;
            int bottomRight = bottomLeft + 1;
            
            // Triangle 1
            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);
            
            // Triangle 2
            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }
    
    m_groundIndexCount = indices.size();
    
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
    
    LOG_INFO("[RuntimeRenderer] Ground mesh created (" + std::to_string(m_groundIndexCount) + " indices)");
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
