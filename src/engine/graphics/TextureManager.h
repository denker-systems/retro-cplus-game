/**
 * @file TextureManager.h
 * @brief Singleton för texturladdning och caching
 */
#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>

/**
 * @brief Hanterar all texturladdning med automatisk caching
 * 
 * Singleton-pattern säkerställer en global åtkomstpunkt.
 * Texturer cachas så samma fil bara laddas en gång.
 */
class TextureManager {
public:
    /** @brief Hämta singleton-instansen */
    static TextureManager& instance();
    
    /** @brief Initiera med SDL renderer */
    void init(SDL_Renderer* renderer);
    
    /** @brief Frigör alla texturer och stäng av */
    void shutdown();
    
    /**
     * @brief Ladda textur från fil
     * @param path Sökväg till bildfil (PNG)
     * @return Pekare till textur, nullptr vid fel
     */
    SDL_Texture* load(const std::string& path);
    
    /**
     * @brief Hämta textur, ladda om den inte finns
     * @param path Sökväg till bildfil
     * @return Pekare till textur
     */
    SDL_Texture* get(const std::string& path);
    
    /** @brief Ta bort specifik textur från cache */
    void unload(const std::string& path);
    
    /** @brief Frigör alla cachade texturer */
    void clear();

private:
    TextureManager() = default;
    ~TextureManager();
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    SDL_Renderer* m_renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> m_textures;  // Cache: path -> texture
};
