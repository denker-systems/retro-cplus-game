/**
 * @file FontManager.cpp
 * @brief Implementation av FontManager
 */
#include "FontManager.h"
#include <iostream>

FontManager& FontManager::instance() {
    static FontManager instance;
    return instance;
}

FontManager::~FontManager() {
    shutdown();
}

bool FontManager::init() {
    if (m_initialized) return true;
    
    if (TTF_Init() < 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << std::endl;
        return false;
    }
    
    m_initialized = true;
    std::cout << "FontManager initialized" << std::endl;
    return true;
}

void FontManager::shutdown() {
    for (auto& pair : m_fonts) {
        if (pair.second) {
            TTF_CloseFont(pair.second);
        }
    }
    m_fonts.clear();
    
    if (m_initialized) {
        TTF_Quit();
        m_initialized = false;
    }
}

bool FontManager::loadFont(const std::string& name, const std::string& path, int size) {
    // Ladda font med skalad storlek för hög DPI
    // Detta ger skarpare text än att låta SDL skala upp en liten font
    int scaledSize = static_cast<int>(size * m_scale);
    if (scaledSize < size) scaledSize = size;  // Minimum original size
    
    TTF_Font* font = TTF_OpenFont(path.c_str(), scaledSize);
    if (!font) {
        std::cerr << "Failed to load font " << path << ": " << TTF_GetError() << std::endl;
        return false;
    }
    
    // Stäng gammal font om den finns
    if (m_fonts.count(name) && m_fonts[name]) {
        TTF_CloseFont(m_fonts[name]);
    }
    
    m_fonts[name] = font;
    m_fontBaseSizes[name] = size;  // Spara bas-storlek för skalning vid rendering
    std::cout << "Loaded font: " << name << " (" << path << ", " << size << "pt scaled to " << scaledSize << "pt)" << std::endl;
    return true;
}

void FontManager::renderText(SDL_Renderer* renderer, const std::string& fontName,
                              const std::string& text, int x, int y, SDL_Color color) {
    auto it = m_fonts.find(fontName);
    if (it == m_fonts.end() || !it->second) {
        // Fallback: rita enkel text-placeholder
        renderFallbackText(renderer, text, x, y, color);
        return;
    }
    
    SDL_Surface* surface = TTF_RenderText_Blended(it->second, text.c_str(), color);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    // Skala ner till logiska koordinater (font är skalad upp, men vi renderar i 640x400)
    int destW = static_cast<int>(surface->w / m_scale);
    int destH = static_cast<int>(surface->h / m_scale);
    
    SDL_Rect destRect = { x, y, destW, destH };
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void FontManager::renderTextCentered(SDL_Renderer* renderer, const std::string& fontName,
                                      const std::string& text, int centerX, int y, SDL_Color color) {
    int w, h;
    getTextSize(fontName, text, &w, &h);
    renderText(renderer, fontName, text, centerX - w / 2, y, color);
}

void FontManager::getTextSize(const std::string& fontName, const std::string& text,
                               int* width, int* height) {
    auto it = m_fonts.find(fontName);
    if (it == m_fonts.end() || !it->second) {
        // Fallback storlek
        *width = static_cast<int>(text.length()) * 6;
        *height = 10;
        return;
    }
    
    TTF_SizeText(it->second, text.c_str(), width, height);
    
    // Returnera skalade värden (för logiska koordinater)
    *width = static_cast<int>(*width / m_scale);
    *height = static_cast<int>(*height / m_scale);
}

void FontManager::renderFallbackText(SDL_Renderer* renderer, const std::string& text,
                                     int x, int y, SDL_Color color) {
    // Enkel bitmap-font fallback (6x8 per tecken)
    const int charW = 6;
    const int charH = 10;
    int textW = static_cast<int>(text.length()) * charW;
    
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (size_t i = 0; i < text.length(); i++) {
        char c = text[i];
        int cx = x + static_cast<int>(i) * charW;
        
        if (c == ' ') continue;
        
        // Rita enkel rektangel för varje tecken
        SDL_Rect charRect = {cx, y + 2, charW - 1, charH - 2};
        SDL_RenderFillRect(renderer, &charRect);
    }
}
