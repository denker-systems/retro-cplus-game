/**
 * @file SpriteSheet.cpp
 * @brief Implementation av sprite sheet rendering
 */
#include "SpriteSheet.h"
#include "TextureManager.h"
#include <iostream>

bool SpriteSheet::load(const std::string& path, int frameWidth, int frameHeight) {
    // Ladda textur via TextureManager (cachas automatiskt)
    m_texture = TextureManager::instance().load(path);
    if (!m_texture) {
        return false;
    }

    m_frameWidth = frameWidth;
    m_frameHeight = frameHeight;

    // Hämta texturens totala storlek för att beräkna rutnät
    SDL_QueryTexture(m_texture, nullptr, nullptr, &m_textureWidth, &m_textureHeight);
    
    // Beräkna antal kolumner och rader baserat på frame-storlek
    m_columns = m_textureWidth / frameWidth;
    m_rows = m_textureHeight / frameHeight;

    if (m_columns < 1) m_columns = 1;
    if (m_rows < 1) m_rows = 1;

    std::cout << "SpriteSheet loaded: " << path 
              << " (" << m_columns << "x" << m_rows << " frames)" << std::endl;
    return true;
}

SDL_Rect SpriteSheet::getFrameRect(int frameIndex) const {
    // Konvertera frame-index till kolumn/rad position
    int col = frameIndex % m_columns;
    int row = frameIndex / m_columns;
    
    return {
        col * m_frameWidth,
        row * m_frameHeight,
        m_frameWidth,
        m_frameHeight
    };
}

void SpriteSheet::render(SDL_Renderer* renderer, int frameIndex, int x, int y, bool flipH) {
    if (!m_texture) return;

    SDL_Rect src = getFrameRect(frameIndex);  // Källrektangel i texturen
    SDL_Rect dst = { x, y, m_frameWidth, m_frameHeight };  // Målrektangel på skärmen
    
    // Flippa horisontellt om karaktären går vänster
    SDL_RendererFlip flip = flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, m_texture, &src, &dst, 0.0, nullptr, flip);
}

void SpriteSheet::renderScaled(SDL_Renderer* renderer, int frameIndex, int x, int y, float scale, bool flipH) {
    if (!m_texture) return;

    SDL_Rect src = getFrameRect(frameIndex);
    SDL_Rect dst = { 
        x, 
        y, 
        static_cast<int>(m_frameWidth * scale), 
        static_cast<int>(m_frameHeight * scale) 
    };
    
    SDL_RendererFlip flip = flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, m_texture, &src, &dst, 0.0, nullptr, flip);
}

void SpriteSheet::renderScaled(SDL_Renderer* renderer, int frameIndex, int x, int y, int w, int h, bool flipH) {
    if (!m_texture) return;

    SDL_Rect src = getFrameRect(frameIndex);
    SDL_Rect dst = { x, y, w, h };
    
    SDL_RendererFlip flip = flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, m_texture, &src, &dst, 0.0, nullptr, flip);
}
