/**
 * @file GLContext.h
 * @brief OpenGL context management for SDL2 windows
 */
#pragma once

#include <SDL.h>
#include <GL/glew.h>
#include <SDL_opengl.h>

namespace engine {

/**
 * @class GLContext
 * @brief Manages OpenGL context creation and lifecycle for SDL2 windows
 * 
 * Handles:
 * - OpenGL 3.3 Core Profile context creation
 * - GLEW initialization for extension loading
 * - VSync control
 * - Context switching and cleanup
 */
class GLContext {
public:
    GLContext() = default;
    ~GLContext();
    
    // Non-copyable
    GLContext(const GLContext&) = delete;
    GLContext& operator=(const GLContext&) = delete;
    
    // Moveable
    GLContext(GLContext&& other) noexcept;
    GLContext& operator=(GLContext&& other) noexcept;
    
    /**
     * @brief Initialize OpenGL context for the given SDL window
     * @param window SDL window to create context for
     * @param majorVersion OpenGL major version (default: 3)
     * @param minorVersion OpenGL minor version (default: 3)
     * @return true if initialization succeeded
     */
    bool initialize(SDL_Window* window, int majorVersion = 3, int minorVersion = 3);
    
    /**
     * @brief Shutdown and cleanup OpenGL context
     */
    void shutdown();
    
    /**
     * @brief Swap buffers (present rendered frame)
     */
    void swapBuffers();
    
    /**
     * @brief Make this context current for the thread
     * @return true if successful
     */
    bool makeCurrent();
    
    /**
     * @brief Enable or disable VSync
     * @param enabled true to enable VSync
     * @return true if successful
     */
    bool setVSync(bool enabled);
    
    /**
     * @brief Check if context is valid and initialized
     */
    bool isValid() const { return m_context != nullptr; }
    
    /**
     * @brief Get the raw SDL_GLContext handle
     */
    SDL_GLContext getContext() const { return m_context; }
    
    /**
     * @brief Get OpenGL version string
     */
    const char* getGLVersion() const;
    
    /**
     * @brief Get GLSL version string
     */
    const char* getGLSLVersion() const;
    
    /**
     * @brief Get GPU renderer string
     */
    const char* getRenderer() const;

private:
    SDL_Window* m_window = nullptr;
    SDL_GLContext m_context = nullptr;
    bool m_initialized = false;
};

} // namespace engine
