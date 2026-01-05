/**
 * @file FontManager.h
 * @brief Font-hantering och text-rendering med SDL_ttf
 */
#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>

/**
 * @brief Singleton för font-hantering och text-rendering
 */
class FontManager {
public:
    static FontManager& instance();
    
    /** @brief Initiera TTF-systemet */
    bool init();
    
    /** @brief Stäng ner TTF */
    void shutdown();
    
    /** @brief Ladda font med given storlek */
    bool loadFont(const std::string& name, const std::string& path, int size);
    
    /** @brief Rendera text till renderer */
    void renderText(SDL_Renderer* renderer, const std::string& fontName,
                    const std::string& text, int x, int y, SDL_Color color);
    
    /** @brief Rendera text centrerad */
    void renderTextCentered(SDL_Renderer* renderer, const std::string& fontName,
                            const std::string& text, int centerX, int y, SDL_Color color);
    
    /** @brief Hämta text-storlek */
    void getTextSize(const std::string& fontName, const std::string& text, 
                     int* width, int* height);

private:
    FontManager() = default;
    ~FontManager();
    FontManager(const FontManager&) = delete;
    FontManager& operator=(const FontManager&) = delete;
    
    std::unordered_map<std::string, TTF_Font*> m_fonts;
    bool m_initialized = false;
};
