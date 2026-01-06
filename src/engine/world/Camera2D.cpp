/**
 * @file Camera2D.cpp
 * @brief Implementation of Camera2D
 */
#include "Camera2D.h"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace engine {

Camera2D::Camera2D()
    : ActorObject("Camera2D") {}

Camera2D::Camera2D(const std::string& name)
    : ActorObject(name) {}

void Camera2D::update(float deltaTime) {
    updateFollow(deltaTime);
    updateShake(deltaTime);
    applyLimits();
    
    ActorObject::update(deltaTime);
}

// ═══════════════════════════════════════════════════════════════════════════
// VIEWPORT
// ═══════════════════════════════════════════════════════════════════════════

void Camera2D::setViewportSize(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
}

void Camera2D::setViewportSize(const Vec2& size) {
    m_viewportWidth = static_cast<int>(size.x);
    m_viewportHeight = static_cast<int>(size.y);
}

// ═══════════════════════════════════════════════════════════════════════════
// ZOOM
// ═══════════════════════════════════════════════════════════════════════════

void Camera2D::setZoom(float zoom) {
    m_zoom = std::max(0.1f, std::min(10.0f, zoom));
}

// ═══════════════════════════════════════════════════════════════════════════
// LIMITS
// ═══════════════════════════════════════════════════════════════════════════

void Camera2D::setLimits(int minX, int minY, int maxX, int maxY) {
    m_limits = {minX, minY, maxX - minX, maxY - minY};
    m_hasLimits = true;
}

void Camera2D::applyLimits() {
    if (!m_hasLimits) return;
    
    float halfW = (m_viewportWidth / m_zoom) * 0.5f;
    float halfH = (m_viewportHeight / m_zoom) * 0.5f;
    
    float minX = m_limits.x + halfW;
    float maxX = m_limits.x + m_limits.w - halfW;
    float minY = m_limits.y + halfH;
    float maxY = m_limits.y + m_limits.h - halfH;
    
    Vec2 pos = getPosition();
    pos.x = std::max(minX, std::min(maxX, pos.x));
    pos.y = std::max(minY, std::min(maxY, pos.y));
    setPosition(pos);
}

// ═══════════════════════════════════════════════════════════════════════════
// FOLLOW
// ═══════════════════════════════════════════════════════════════════════════

void Camera2D::updateFollow(float deltaTime) {
    if (!m_target) return;
    
    Vec2 targetPos = m_target->getGlobalPosition() + m_offset;
    
    if (m_smoothing > 0.0001f) {
        // Smooth follow
        Vec2 currentPos = getPosition();
        Vec2 newPos = currentPos.lerp(targetPos, 1.0f - std::pow(m_smoothing, deltaTime * 60.0f));
        setPosition(newPos);
    } else {
        // Instant follow
        setPosition(targetPos);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// SCREEN SHAKE
// ═══════════════════════════════════════════════════════════════════════════

void Camera2D::shake(float intensity, float duration) {
    m_shakeIntensity = intensity;
    m_shakeDuration = duration;
    m_shakeTimer = 0.0f;
}

void Camera2D::stopShake() {
    m_shakeIntensity = 0.0f;
    m_shakeDuration = 0.0f;
    m_shakeTimer = 0.0f;
    m_shakeOffset = Vec2::zero();
}

void Camera2D::updateShake(float deltaTime) {
    if (m_shakeTimer >= m_shakeDuration) {
        m_shakeOffset = Vec2::zero();
        return;
    }
    
    m_shakeTimer += deltaTime;
    
    // Decay over time
    float t = m_shakeTimer / m_shakeDuration;
    float currentIntensity = m_shakeIntensity * (1.0f - t);
    
    // Random offset
    float angle = static_cast<float>(rand()) / RAND_MAX * 6.28318f;
    float distance = (static_cast<float>(rand()) / RAND_MAX) * currentIntensity;
    
    m_shakeOffset = Vec2::fromAngle(angle) * distance;
}

// ═══════════════════════════════════════════════════════════════════════════
// COORDINATE CONVERSION
// ═══════════════════════════════════════════════════════════════════════════

Vec2 Camera2D::worldToScreen(const Vec2& worldPos) const {
    Vec2 camPos = getGlobalPosition() + m_shakeOffset;
    
    Vec2 offset = worldPos - camPos;
    offset = offset * m_zoom;
    
    return Vec2(
        m_viewportWidth * 0.5f + offset.x,
        m_viewportHeight * 0.5f + offset.y
    );
}

Vec2 Camera2D::screenToWorld(const Vec2& screenPos) const {
    Vec2 camPos = getGlobalPosition() + m_shakeOffset;
    
    Vec2 offset = Vec2(
        screenPos.x - m_viewportWidth * 0.5f,
        screenPos.y - m_viewportHeight * 0.5f
    );
    
    offset = offset / m_zoom;
    return camPos + offset;
}

SDL_Rect Camera2D::getViewRect() const {
    Vec2 camPos = getGlobalPosition();
    
    float halfW = (m_viewportWidth / m_zoom) * 0.5f;
    float halfH = (m_viewportHeight / m_zoom) * 0.5f;
    
    return SDL_Rect{
        static_cast<int>(camPos.x - halfW),
        static_cast<int>(camPos.y - halfH),
        static_cast<int>(halfW * 2),
        static_cast<int>(halfH * 2)
    };
}

bool Camera2D::isVisible(const Vec2& worldPos) const {
    SDL_Rect view = getViewRect();
    return worldPos.x >= view.x && worldPos.x <= view.x + view.w &&
           worldPos.y >= view.y && worldPos.y <= view.y + view.h;
}

bool Camera2D::isVisible(const SDL_Rect& worldRect) const {
    SDL_Rect view = getViewRect();
    return SDL_HasIntersection(&view, &worldRect);
}

} // namespace engine
