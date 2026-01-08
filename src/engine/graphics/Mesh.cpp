/**
 * @file Mesh.cpp
 * @brief 3D mesh geometry implementation
 */
#include "Mesh.h"
#include <iostream>

namespace engine {

Mesh::Mesh(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& indices) {
    m_vertexCount = vertices.size();
    m_indexCount = indices.size();
    
    // Create VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    
    // Create VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), 
                 vertices.data(), GL_STATIC_DRAW);
    
    // Create IBO
    glGenBuffers(1, &m_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), 
                 indices.data(), GL_STATIC_DRAW);
    
    // Set vertex attributes
    // Position (location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), 
                          (void*)offsetof(Vertex3D, position));
    
    // Normal (location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), 
                          (void*)offsetof(Vertex3D, normal));
    
    // TexCoord (location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), 
                          (void*)offsetof(Vertex3D, texCoord));
    
    // Unbind
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao)
    , m_vbo(other.m_vbo)
    , m_ibo(other.m_ibo)
    , m_vertexCount(other.m_vertexCount)
    , m_indexCount(other.m_indexCount)
    , m_primitiveType(other.m_primitiveType) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ibo = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ibo = other.m_ibo;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;
        m_primitiveType = other.m_primitiveType;
        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ibo = 0;
    }
    return *this;
}

void Mesh::render() const {
    glBindVertexArray(m_vao);
    glDrawElements(m_primitiveType, static_cast<GLsizei>(m_indexCount), 
                   GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::cleanup() {
    if (m_vao) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_ibo) {
        glDeleteBuffers(1, &m_ibo);
        m_ibo = 0;
    }
}

std::unique_ptr<Mesh> Mesh::createCube() {
    std::vector<Vertex3D> vertices = {
        // Front face
        {{-0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
        // Back face
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        // Top face
        {{-0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        // Bottom face
        {{-0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f, -0.5f,  0.5f}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        // Right face
        {{ 0.5f, -0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ 0.5f, -0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ 0.5f,  0.5f, -0.5f}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ 0.5f,  0.5f,  0.5f}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        // Left face
        {{-0.5f, -0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-0.5f, -0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-0.5f,  0.5f,  0.5f}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-0.5f,  0.5f, -0.5f}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
    };
    
    std::vector<uint32_t> indices = {
        0,  1,  2,  2,  3,  0,   // Front
        4,  5,  6,  6,  7,  4,   // Back
        8,  9,  10, 10, 11, 8,   // Top
        12, 13, 14, 14, 15, 12,  // Bottom
        16, 17, 18, 18, 19, 16,  // Right
        20, 21, 22, 22, 23, 20   // Left
    };
    
    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Mesh::createPlane(float size) {
    float half = size * 0.5f;
    
    std::vector<Vertex3D> vertices = {
        {{-half, 0.0f, -half}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ half, 0.0f, -half}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ half, 0.0f,  half}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-half, 0.0f,  half}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    };
    
    std::vector<uint32_t> indices = {0, 1, 2, 2, 3, 0};
    
    return std::make_unique<Mesh>(vertices, indices);
}

std::unique_ptr<Mesh> Mesh::createGrid(int lines, float spacing) {
    std::vector<Vertex3D> vertices;
    std::vector<uint32_t> indices;
    
    float halfSize = (lines * spacing) * 0.5f;
    uint32_t index = 0;
    
    // Lines along X axis
    for (int i = 0; i <= lines; ++i) {
        float z = -halfSize + i * spacing;
        vertices.push_back({{-halfSize, 0.0f, z}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{ halfSize, 0.0f, z}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}});
        indices.push_back(index++);
        indices.push_back(index++);
    }
    
    // Lines along Z axis
    for (int i = 0; i <= lines; ++i) {
        float x = -halfSize + i * spacing;
        vertices.push_back({{x, 0.0f, -halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}});
        vertices.push_back({{x, 0.0f,  halfSize}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}});
        indices.push_back(index++);
        indices.push_back(index++);
    }
    
    auto mesh = std::make_unique<Mesh>(vertices, indices);
    mesh->m_primitiveType = GL_LINES;
    return mesh;
}

} // namespace engine
