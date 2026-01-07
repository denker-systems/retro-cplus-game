/**
 * @file Renderer.cpp
 * @brief Implementation of Renderer utility functions for SDL2 texture operations
 */
#include "Renderer.h"
#include <iostream>

SDL_Texture* Renderer::loadTexture(SDL_Renderer* renderer, const std::string& path) {
    // Load texture from file using SDL2_image
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << path << " - " << IMG_GetError() << std::endl;
    }
    return texture;
}

void Renderer::drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y) {
    if (!texture) return;
    
    // Get texture dimensions and create destination rectangle
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    SDL_Rect dst = {x, y, w, h};
    SDL_RenderCopy(renderer, texture, nullptr, &dst);
}

void Renderer::drawTextureEx(SDL_Renderer* renderer, SDL_Texture* texture,
                             const SDL_Rect* src, const SDL_Rect* dst, bool flipH) {
    if (!texture) return;
    
    // Set flip mode and render with optional source rectangle
    SDL_RendererFlip flip = flipH ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(renderer, texture, src, dst, 0.0, nullptr, flip);
}
