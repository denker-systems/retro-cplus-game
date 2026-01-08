/**
 * @file Mesh.h
 * @brief 3D mesh geometry management
 */
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>

namespace engine {

/**
 * @struct Vertex3D
 * @brief Vertex data for 3D meshes
 */
struct Vertex3D {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
    
    Vertex3D() = default;
    Vertex3D(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv)
        : position(pos), normal(norm), texCoord(uv) {}
};

/**
 * @class Mesh
 * @brief Manages OpenGL vertex buffers for 3D geometry
 * 
 * Provides a simple interface for creating and rendering 3D meshes
 * with support for positions, normals, and texture coordinates.
 */
class Mesh {
public:
    /**
     * @brief Create mesh from vertex and index data
     * @param vertices Vertex data
     * @param indices Index data for triangles
     */
    Mesh(const std::vector<Vertex3D>& vertices, const std::vector<uint32_t>& indices);
    ~Mesh();
    
    // Non-copyable
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    
    // Moveable
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;
    
    /**
     * @brief Render the mesh
     */
    void render() const;
    
    /**
     * @brief Get vertex count
     */
    size_t getVertexCount() const { return m_vertexCount; }
    
    /**
     * @brief Get index count
     */
    size_t getIndexCount() const { return m_indexCount; }
    
    // Factory methods for primitive shapes
    
    /**
     * @brief Create a unit cube centered at origin
     */
    static std::unique_ptr<Mesh> createCube();
    
    /**
     * @brief Create a flat plane on XZ axis
     * @param size Size of the plane
     */
    static std::unique_ptr<Mesh> createPlane(float size = 10.0f);
    
    /**
     * @brief Create a grid of lines
     * @param lines Number of lines in each direction
     * @param spacing Spacing between lines
     */
    static std::unique_ptr<Mesh> createGrid(int lines = 20, float spacing = 1.0f);

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ibo = 0;
    size_t m_vertexCount = 0;
    size_t m_indexCount = 0;
    GLenum m_primitiveType = GL_TRIANGLES;
    
    void cleanup();
};

} // namespace engine
