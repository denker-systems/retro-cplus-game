/**
 * @file Framebuffer.cpp
 * @brief OpenGL Framebuffer implementation
 */
#include "Framebuffer.h"
#include <iostream>

namespace engine {

Framebuffer::Framebuffer(const FramebufferSpec& spec)
    : m_spec(spec) {
    invalidate();
}

Framebuffer::~Framebuffer() {
    cleanup();
}

Framebuffer::Framebuffer(Framebuffer&& other) noexcept
    : m_spec(other.m_spec)
    , m_fbo(other.m_fbo)
    , m_colorTexture(other.m_colorTexture)
    , m_depthBuffer(other.m_depthBuffer)
    , m_valid(other.m_valid) {
    other.m_fbo = 0;
    other.m_colorTexture = 0;
    other.m_depthBuffer = 0;
    other.m_valid = false;
}

Framebuffer& Framebuffer::operator=(Framebuffer&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_spec = other.m_spec;
        m_fbo = other.m_fbo;
        m_colorTexture = other.m_colorTexture;
        m_depthBuffer = other.m_depthBuffer;
        m_valid = other.m_valid;
        other.m_fbo = 0;
        other.m_colorTexture = 0;
        other.m_depthBuffer = 0;
        other.m_valid = false;
    }
    return *this;
}

void Framebuffer::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_spec.width, m_spec.height);
}

void Framebuffer::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int width, int height) {
    if (width <= 0 || height <= 0) {
        std::cerr << "Framebuffer: Invalid resize dimensions: " << width << "x" << height << std::endl;
        return;
    }
    
    if (m_spec.width == width && m_spec.height == height) {
        return;  // No change needed
    }
    
    m_spec.width = width;
    m_spec.height = height;
    invalidate();
}

void Framebuffer::clear(float r, float g, float b, float a) {
    glClearColor(r, g, b, a);
    GLbitfield clearFlags = GL_COLOR_BUFFER_BIT;
    if (m_spec.hasDepth) {
        clearFlags |= GL_DEPTH_BUFFER_BIT;
    }
    if (m_spec.hasStencil) {
        clearFlags |= GL_STENCIL_BUFFER_BIT;
    }
    glClear(clearFlags);
}

void Framebuffer::invalidate() {
    // Cleanup existing resources
    cleanup();
    
    // Create framebuffer
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    
    // Create color attachment texture
    glGenTextures(1, &m_colorTexture);
    glBindTexture(GL_TEXTURE_2D, m_colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_spec.width, m_spec.height, 
                 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
    
    // Create depth/stencil attachment
    if (m_spec.hasDepth) {
        glGenRenderbuffers(1, &m_depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
        
        GLenum internalFormat = m_spec.hasStencil ? GL_DEPTH24_STENCIL8 : GL_DEPTH_COMPONENT24;
        glRenderbufferStorage(GL_RENDERBUFFER, internalFormat, m_spec.width, m_spec.height);
        
        GLenum attachment = m_spec.hasStencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT;
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_depthBuffer);
    }
    
    // Check framebuffer completeness
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer: Incomplete framebuffer, status: 0x" << std::hex << status << std::endl;
        m_valid = false;
    } else {
        m_valid = true;
    }
    
    // Unbind
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

void Framebuffer::cleanup() {
    if (m_fbo) {
        glDeleteFramebuffers(1, &m_fbo);
        m_fbo = 0;
    }
    if (m_colorTexture) {
        glDeleteTextures(1, &m_colorTexture);
        m_colorTexture = 0;
    }
    if (m_depthBuffer) {
        glDeleteRenderbuffers(1, &m_depthBuffer);
        m_depthBuffer = 0;
    }
    m_valid = false;
}

} // namespace engine
