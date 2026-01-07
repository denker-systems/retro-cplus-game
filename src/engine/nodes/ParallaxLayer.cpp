/**
 * @file ParallaxLayer.cpp
 * @brief Implementation of ParallaxLayer
 */
#include "ParallaxLayer.h"
#include <cmath>

namespace engine {

ParallaxLayer::ParallaxLayer() : Sprite("ParallaxLayer") {}

ParallaxLayer::ParallaxLayer(const std::string& name) : Sprite(name) {}

ParallaxLayer::ParallaxLayer(const std::string& name, SDL_Texture* texture)
    : Sprite(name, texture) {}

void ParallaxLayer::render(SDL_Renderer* renderer) {
    if (!getTexture() || !renderer) {
        Sprite::render(renderer);
        return;
    }
    
    // Get camera position (if we have access to it)
    // For now, use global position as reference
    Vec2 globalPos = getGlobalPosition();
    
    // Apply parallax offset
    Vec2 parallaxPos = globalPos;
    parallaxPos.x *= m_parallaxX;
    parallaxPos.y *= m_parallaxY;
    parallaxPos += m_scrollOffset;
    
    Vec2 size = getSize();
    
    if (m_repeatX || m_repeatY) {
        // Repeating background
        // Calculate how many times to repeat
        int viewportW = 640;  // TODO: Get from camera
        int viewportH = 400;
        
        // Wrap position for seamless repeat
        float wrapX = m_repeatX ? std::fmod(parallaxPos.x, size.x) : parallaxPos.x;
        float wrapY = m_repeatY ? std::fmod(parallaxPos.y, size.y) : parallaxPos.y;
        
        // Ensure positive wrap
        if (wrapX > 0) wrapX -= size.x;
        if (wrapY > 0) wrapY -= size.y;
        
        int tilesX = m_repeatX ? static_cast<int>(std::ceil(viewportW / size.x)) + 2 : 1;
        int tilesY = m_repeatY ? static_cast<int>(std::ceil(viewportH / size.y)) + 2 : 1;
        
        // Render repeated tiles
        for (int ty = 0; ty < tilesY; ty++) {
            for (int tx = 0; tx < tilesX; tx++) {
                Vec2 tilePos = Vec2(wrapX + tx * size.x, wrapY + ty * size.y);
                
                // Temporarily set position for rendering
                Vec2 oldPos = getPosition();
                setPosition(tilePos);
                Sprite::render(renderer);
                setPosition(oldPos);
            }
        }
    } else {
        // Single render
        Vec2 oldPos = getPosition();
        setPosition(parallaxPos);
        Sprite::render(renderer);
        setPosition(oldPos);
    }
}

} // namespace engine
