/**
 * @file Renderer.h
 * @brief Utility functions for SDL2 texture operations
 */
#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <unordered_map>

/**
 * @brief Static utility class for SDL2 rendering operations
 * 
 * Provides helper functions for common texture operations:
 * - Loading textures from files
 * - Drawing textures at specific positions
 * - Drawing with source rectangles and flipping
 */
class Renderer {
public:
    /**
     * @brief Load a texture from file using SDL2_image
     * @param renderer SDL renderer instance
     * @param path File path to the texture
     * @return Loaded texture or nullptr on failure
     */
    static SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path);
    
    /**
     * @brief Draw a texture at the specified position
     * @param renderer SDL renderer instance
     * @param texture Texture to draw
     * @param x X position in screen coordinates
     * @param y Y position in screen coordinates
     */
    static void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);
    
    /**
     * @brief Draw a texture with advanced options
     * @param renderer SDL renderer instance
     * @param texture Texture to draw
     * @param src Source rectangle (nullptr for full texture)
     * @param dst Destination rectangle
     * @param flipH Whether to flip horizontally
     */
    static void drawTextureEx(SDL_Renderer* renderer, SDL_Texture* texture, 
                              const SDL_Rect* src, const SDL_Rect* dst, bool flipH = false);
};
