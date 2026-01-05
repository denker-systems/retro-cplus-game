#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <unordered_map>

class Renderer {
public:
    static SDL_Texture* loadTexture(SDL_Renderer* renderer, const std::string& path);
    static void drawTexture(SDL_Renderer* renderer, SDL_Texture* texture, int x, int y);
    static void drawTextureEx(SDL_Renderer* renderer, SDL_Texture* texture, 
                              const SDL_Rect* src, const SDL_Rect* dst, bool flipH = false);
};
