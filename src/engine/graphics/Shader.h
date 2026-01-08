/**
 * @file Shader.h
 * @brief OpenGL shader program management
 */
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <memory>
#include <unordered_map>

namespace engine {

/**
 * @class Shader
 * @brief Manages OpenGL shader programs with uniform caching
 * 
 * Provides a simple interface for loading, compiling, and using
 * GLSL shaders with automatic uniform location caching.
 */
class Shader {
public:
    /**
     * @brief Create shader from source strings
     * @param vertexSource GLSL vertex shader source code
     * @param fragmentSource GLSL fragment shader source code
     */
    Shader(const std::string& vertexSource, const std::string& fragmentSource);
    
    ~Shader();
    
    // Non-copyable
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    
    // Moveable
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;
    
    /**
     * @brief Bind shader program for use
     */
    void bind() const;
    
    /**
     * @brief Unbind shader program
     */
    void unbind() const;
    
    /**
     * @brief Check if shader compiled and linked successfully
     */
    bool isValid() const { return m_valid; }
    
    /**
     * @brief Get the OpenGL program ID
     */
    GLuint getProgramID() const { return m_programID; }
    
    // Uniform setters
    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec2(const std::string& name, const glm::vec2& value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setVec4(const std::string& name, const glm::vec4& value);
    void setMat3(const std::string& name, const glm::mat3& value);
    void setMat4(const std::string& name, const glm::mat4& value);
    
    /**
     * @brief Create shader from file paths
     * @param vertexPath Path to vertex shader file
     * @param fragmentPath Path to fragment shader file
     * @return Unique pointer to shader, nullptr on failure
     */
    static std::unique_ptr<Shader> loadFromFiles(const std::string& vertexPath, 
                                                  const std::string& fragmentPath);

private:
    GLuint m_programID = 0;
    bool m_valid = false;
    mutable std::unordered_map<std::string, GLint> m_uniformCache;
    
    /**
     * @brief Get cached uniform location
     * @param name Uniform name
     * @return Uniform location, -1 if not found
     */
    GLint getUniformLocation(const std::string& name) const;
    
    /**
     * @brief Compile a shader from source
     * @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
     * @param source GLSL source code
     * @return Shader ID, 0 on failure
     */
    static GLuint compileShader(GLenum type, const std::string& source);
    
    /**
     * @brief Link shader program
     * @param vertexShader Compiled vertex shader ID
     * @param fragmentShader Compiled fragment shader ID
     * @return Program ID, 0 on failure
     */
    static GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
};

} // namespace engine
