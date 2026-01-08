/**
 * @file Framebuffer.h
 * @brief OpenGL Framebuffer Object (FBO) for off-screen rendering
 */
#pragma once

#include <GL/glew.h>
#include <cstdint>

namespace engine {

/**
 * @struct FramebufferSpec
 * @brief Specification for framebuffer creation
 */
struct FramebufferSpec {
    int width = 1280;
    int height = 720;
    bool hasDepth = true;
    bool hasStencil = false;
    int samples = 1;  // 1 = no multisampling
};

/**
 * @class Framebuffer
 * @brief Manages OpenGL Framebuffer Objects for off-screen rendering
 * 
 * Used for rendering 3D scenes to textures that can be displayed
 * in ImGui viewports or used for post-processing effects.
 */
class Framebuffer {
public:
    Framebuffer(const FramebufferSpec& spec);
    ~Framebuffer();
    
    // Non-copyable
    Framebuffer(const Framebuffer&) = delete;
    Framebuffer& operator=(const Framebuffer&) = delete;
    
    // Moveable
    Framebuffer(Framebuffer&& other) noexcept;
    Framebuffer& operator=(Framebuffer&& other) noexcept;
    
    /**
     * @brief Bind framebuffer for rendering
     */
    void bind();
    
    /**
     * @brief Unbind framebuffer (bind default framebuffer)
     */
    void unbind();
    
    /**
     * @brief Resize framebuffer to new dimensions
     * @param width New width in pixels
     * @param height New height in pixels
     */
    void resize(int width, int height);
    
    /**
     * @brief Clear the framebuffer with specified color
     * @param r Red component (0-1)
     * @param g Green component (0-1)
     * @param b Blue component (0-1)
     * @param a Alpha component (0-1)
     */
    void clear(float r = 0.1f, float g = 0.1f, float b = 0.15f, float a = 1.0f);
    
    /**
     * @brief Get the color attachment texture ID
     * @return OpenGL texture ID for the color attachment
     */
    GLuint getColorAttachment() const { return m_colorTexture; }
    
    /**
     * @brief Get the depth attachment renderbuffer ID
     * @return OpenGL renderbuffer ID for depth attachment
     */
    GLuint getDepthAttachment() const { return m_depthBuffer; }
    
    /**
     * @brief Get framebuffer width
     */
    int getWidth() const { return m_spec.width; }
    
    /**
     * @brief Get framebuffer height
     */
    int getHeight() const { return m_spec.height; }
    
    /**
     * @brief Get the framebuffer specification
     */
    const FramebufferSpec& getSpec() const { return m_spec; }
    
    /**
     * @brief Check if framebuffer is valid and complete
     */
    bool isValid() const { return m_valid; }

private:
    FramebufferSpec m_spec;
    GLuint m_fbo = 0;
    GLuint m_colorTexture = 0;
    GLuint m_depthBuffer = 0;
    bool m_valid = false;
    
    /**
     * @brief Create or recreate framebuffer attachments
     */
    void invalidate();
    
    /**
     * @brief Delete all framebuffer resources
     */
    void cleanup();
};

} // namespace engine
