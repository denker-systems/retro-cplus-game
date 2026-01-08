/**
 * @file GLTextureManager.h
 * @brief OpenGL texture manager for ImGui rendering
 * 
 * Loads textures directly as OpenGL textures for use with ImGui's OpenGL backend.
 * This is separate from SDL's TextureManager to avoid SDL_Texture/OpenGL incompatibility.
 */
#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>

namespace engine {

/**
 * @brief Manages OpenGL textures for ImGui rendering
 * 
 * Singleton that caches OpenGL textures loaded from image files.
 * Uses stb_image for loading.
 */
class GLTextureManager {
public:
    static GLTextureManager& instance();
    
    /**
     * @brief Initialize the manager (call after OpenGL context created)
     */
    void initialize();
    
    /**
     * @brief Shutdown and free all textures
     */
    void shutdown();
    
    /**
     * @brief Load texture from file path
     * @param path Path to image file
     * @return OpenGL texture ID, or 0 on failure
     */
    GLuint load(const std::string& path);
    
    /**
     * @brief Get cached texture by path
     * @param path Path to image file
     * @return OpenGL texture ID, or 0 if not loaded
     */
    GLuint get(const std::string& path);
    
    /**
     * @brief Get texture dimensions
     * @param path Path to image file
     * @param width Output width
     * @param height Output height
     * @return true if texture exists
     */
    bool getSize(const std::string& path, int& width, int& height);
    
    /**
     * @brief Check if texture is loaded
     */
    bool has(const std::string& path) const;
    
    /**
     * @brief Unload specific texture
     */
    void unload(const std::string& path);
    
    /**
     * @brief Unload all textures
     */
    void clear();
    
private:
    GLTextureManager() = default;
    ~GLTextureManager() = default;
    GLTextureManager(const GLTextureManager&) = delete;
    GLTextureManager& operator=(const GLTextureManager&) = delete;
    
    struct TextureData {
        GLuint id = 0;
        int width = 0;
        int height = 0;
    };
    
    std::unordered_map<std::string, TextureData> m_textures;
    bool m_initialized = false;
};

} // namespace engine
