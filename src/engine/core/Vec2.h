/**
 * @file Vec2.h
 * @brief 2D vector mathematics
 */
#pragma once

#include <cmath>

namespace engine {

/**
 * @brief 2D vector for positions, velocities, etc.
 */
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
    
    Vec2() = default;
    Vec2(float x_, float y_) : x(x_), y(y_) {}
    
    // Arithmetic operators
    Vec2 operator+(const Vec2& other) const { return Vec2(x + other.x, y + other.y); }
    Vec2 operator-(const Vec2& other) const { return Vec2(x - other.x, y - other.y); }
    Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }
    Vec2 operator/(float scalar) const { return Vec2(x / scalar, y / scalar); }
    
    Vec2& operator+=(const Vec2& other) { x += other.x; y += other.y; return *this; }
    Vec2& operator-=(const Vec2& other) { x -= other.x; y -= other.y; return *this; }
    Vec2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
    Vec2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }
    
    Vec2 operator-() const { return Vec2(-x, -y); }
    
    bool operator==(const Vec2& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Vec2& other) const { return !(*this == other); }
    
    // Vector operations
    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSquared() const { return x * x + y * y; }
    
    Vec2 normalized() const {
        float len = length();
        if (len > 0.0001f) return *this / len;
        return Vec2(0, 0);
    }
    
    void normalize() {
        float len = length();
        if (len > 0.0001f) {
            x /= len;
            y /= len;
        }
    }
    
    float dot(const Vec2& other) const { return x * other.x + y * other.y; }
    float cross(const Vec2& other) const { return x * other.y - y * other.x; }
    
    float distanceTo(const Vec2& other) const { return (*this - other).length(); }
    float distanceSquaredTo(const Vec2& other) const { return (*this - other).lengthSquared(); }
    
    Vec2 lerp(const Vec2& target, float t) const {
        return *this + (target - *this) * t;
    }
    
    Vec2 rotated(float angle) const {
        float c = std::cos(angle);
        float s = std::sin(angle);
        return Vec2(x * c - y * s, x * s + y * c);
    }
    
    float angle() const { return std::atan2(y, x); }
    float angleTo(const Vec2& other) const { return std::atan2(other.y - y, other.x - x); }
    
    // Static helpers
    static Vec2 zero() { return Vec2(0, 0); }
    static Vec2 one() { return Vec2(1, 1); }
    static Vec2 up() { return Vec2(0, -1); }
    static Vec2 down() { return Vec2(0, 1); }
    static Vec2 left() { return Vec2(-1, 0); }
    static Vec2 right() { return Vec2(1, 0); }
    
    static Vec2 fromAngle(float angle) { return Vec2(std::cos(angle), std::sin(angle)); }
};

// Scalar * Vec2
inline Vec2 operator*(float scalar, const Vec2& v) { return v * scalar; }

} // namespace engine
