/**
 * @file Shader.cpp
 * @brief OpenGL shader program implementation
 */
#include "Shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace engine {

Shader::Shader(const std::string& vertexSource, const std::string& fragmentSource) {
    // Compile shaders
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource);
    if (vertexShader == 0) {
        return;
    }
    
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return;
    }
    
    // Link program
    m_programID = linkProgram(vertexShader, fragmentShader);
    
    // Cleanup shaders (they're linked into the program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    m_valid = (m_programID != 0);
}

Shader::~Shader() {
    if (m_programID) {
        glDeleteProgram(m_programID);
        m_programID = 0;
    }
}

Shader::Shader(Shader&& other) noexcept
    : m_programID(other.m_programID)
    , m_valid(other.m_valid)
    , m_uniformCache(std::move(other.m_uniformCache)) {
    other.m_programID = 0;
    other.m_valid = false;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (m_programID) {
            glDeleteProgram(m_programID);
        }
        m_programID = other.m_programID;
        m_valid = other.m_valid;
        m_uniformCache = std::move(other.m_uniformCache);
        other.m_programID = 0;
        other.m_valid = false;
    }
    return *this;
}

void Shader::bind() const {
    glUseProgram(m_programID);
}

void Shader::unbind() const {
    glUseProgram(0);
}

GLint Shader::getUniformLocation(const std::string& name) const {
    auto it = m_uniformCache.find(name);
    if (it != m_uniformCache.end()) {
        return it->second;
    }
    
    GLint location = glGetUniformLocation(m_programID, name.c_str());
    m_uniformCache[name] = location;
    
    if (location == -1) {
        std::cerr << "Shader: Uniform '" << name << "' not found" << std::endl;
    }
    
    return location;
}

void Shader::setInt(const std::string& name, int value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::setFloat(const std::string& name, float value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setMat3(const std::string& name, const glm::mat3& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

std::unique_ptr<Shader> Shader::loadFromFiles(const std::string& vertexPath, 
                                               const std::string& fragmentPath) {
    // Read vertex shader
    std::ifstream vFile(vertexPath);
    if (!vFile.is_open()) {
        std::cerr << "Shader: Failed to open vertex shader: " << vertexPath << std::endl;
        return nullptr;
    }
    std::stringstream vStream;
    vStream << vFile.rdbuf();
    std::string vertexSource = vStream.str();
    
    // Read fragment shader
    std::ifstream fFile(fragmentPath);
    if (!fFile.is_open()) {
        std::cerr << "Shader: Failed to open fragment shader: " << fragmentPath << std::endl;
        return nullptr;
    }
    std::stringstream fStream;
    fStream << fFile.rdbuf();
    std::string fragmentSource = fStream.str();
    
    auto shader = std::make_unique<Shader>(vertexSource, fragmentSource);
    if (!shader->isValid()) {
        return nullptr;
    }
    
    return shader;
}

GLuint Shader::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    
    // Check for errors
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader: Compilation error (" 
                  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
                  << "): " << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    
    return shader;
}

GLuint Shader::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    
    // Check for errors
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        GLchar infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader: Link error: " << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }
    
    return program;
}

} // namespace engine
