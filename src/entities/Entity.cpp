/**
 * @file Entity.cpp
 * @brief Implementation av Entity base class
 */
#include "Entity.h"

Entity::Entity(float x, float y, int width, int height)
    : m_x(x), m_y(y), m_width(width), m_height(height) {}

SDL_Rect Entity::getBounds() const {
    return {
        static_cast<int>(m_x),
        static_cast<int>(m_y),
        m_width,
        m_height
    };
}
