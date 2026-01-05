/**
 * @file TextureManager.cpp
 * @brief Implementation av texturladdning och caching
 */
#include "TextureManager.h"
#include <SDL_image.h>
#include <iostream>

TextureManager& TextureManager::instance() {
    static TextureManager instance;
    return instance;
}

TextureManager::~TextureManager() {
    clear();
}

void TextureManager::init(SDL_Renderer* renderer) {
    m_renderer = renderer;
    std::cout << "TextureManager initialized" << std::endl;
}

void TextureManager::shutdown() {
    clear();
    m_renderer = nullptr;
}

SDL_Texture* TextureManager::load(const std::string& path) {
    // Kolla om texturen redan finns i cache
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    if (!m_renderer) {
        std::cerr << "TextureManager: Renderer not initialized!" << std::endl;
        return nullptr;
    }

    // Ladda textur via SDL_image
    SDL_Texture* texture = IMG_LoadTexture(m_renderer, path.c_str());
    if (!texture) {
        std::cerr << "Failed to load texture: " << path << " - " << IMG_GetError() << std::endl;
        return nullptr;
    }

    // Spara i cache för återanvändning
    m_textures[path] = texture;
    std::cout << "Loaded texture: " << path << std::endl;
    return texture;
}

SDL_Texture* TextureManager::get(const std::string& path) {
    // Returnera från cache om den finns, annars ladda
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }
    return load(path);
}

void TextureManager::unload(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }
}

void TextureManager::clear() {
    // Frigör alla SDL-texturer
    for (auto& pair : m_textures) {
        SDL_DestroyTexture(pair.second);
    }
    m_textures.clear();
}
