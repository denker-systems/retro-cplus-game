/**
 * @file GLTextureManager.cpp
 * @brief OpenGL texture manager implementation
 */
#include "GLTextureManager.h"
#include <iostream>

// stb_image is available via vcpkg
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace engine {

GLTextureManager& GLTextureManager::instance() {
    static GLTextureManager instance;
    return instance;
}

void GLTextureManager::initialize() {
    if (m_initialized) return;
    m_initialized = true;
    std::cout << "GLTextureManager initialized" << std::endl;
}

void GLTextureManager::shutdown() {
    clear();
    m_initialized = false;
}

GLuint GLTextureManager::load(const std::string& path) {
    // Check cache first
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second.id;
    }
    
    // Load image with stb_image
    int width, height, channels;
    stbi_set_flip_vertically_on_load(false);  // Don't flip for ImGui
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 4);  // Force RGBA
    
    if (!data) {
        std::cerr << "[GLTextureManager] Failed to load: " << path << std::endl;
        return 0;
    }
    
    // Create OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    // Upload texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    
    // Free image data
    stbi_image_free(data);
    
    // Cache texture
    TextureData texData;
    texData.id = textureID;
    texData.width = width;
    texData.height = height;
    m_textures[path] = texData;
    
    std::cout << "[GLTextureManager] Loaded: " << path << " (" << width << "x" << height << ")" << std::endl;
    
    return textureID;
}

GLuint GLTextureManager::get(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second.id;
    }
    // Try to load if not cached
    return load(path);
}

bool GLTextureManager::getSize(const std::string& path, int& width, int& height) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        width = it->second.width;
        height = it->second.height;
        return true;
    }
    return false;
}

bool GLTextureManager::has(const std::string& path) const {
    return m_textures.find(path) != m_textures.end();
}

void GLTextureManager::unload(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        glDeleteTextures(1, &it->second.id);
        m_textures.erase(it);
    }
}

void GLTextureManager::clear() {
    for (auto& pair : m_textures) {
        glDeleteTextures(1, &pair.second.id);
    }
    m_textures.clear();
}

} // namespace engine
