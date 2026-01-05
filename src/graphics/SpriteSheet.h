/**
 * @file SpriteSheet.h
 * @brief Hanterar sprite sheets med flera frames
 */
#pragma once

#include <SDL.h>
#include <string>

/**
 * @brief Laddar och renderar sprite sheets
 * 
 * En sprite sheet är en bild med flera frames i ett rutnät.
 * Varje frame har samma storlek och kan renderas individuellt.
 */
class SpriteSheet {
public:
    SpriteSheet() = default;
    ~SpriteSheet() = default;

    /**
     * @brief Ladda sprite sheet från fil
     * @param path Sökväg till bildfil
     * @param frameWidth Bredd per frame i pixlar
     * @param frameHeight Höjd per frame i pixlar
     * @return true om laddning lyckades
     */
    bool load(const std::string& path, int frameWidth, int frameHeight);
    
    /**
     * @brief Rita en specifik frame
     * @param frameIndex Index för framen (0-baserat)
     * @param x X-position på skärmen
     * @param y Y-position på skärmen
     * @param flipH Spegla horisontellt
     */
    void render(SDL_Renderer* renderer, int frameIndex, int x, int y, bool flipH = false);
    
    /** @brief Rita frame med skalning */
    void renderScaled(SDL_Renderer* renderer, int frameIndex, int x, int y, float scale, bool flipH = false);

    // Getters
    int getFrameWidth() const { return m_frameWidth; }
    int getFrameHeight() const { return m_frameHeight; }
    int getColumns() const { return m_columns; }
    int getRows() const { return m_rows; }
    int getTotalFrames() const { return m_columns * m_rows; }
    bool isLoaded() const { return m_texture != nullptr; }

private:
    /** @brief Beräkna källrektangel för given frame */
    SDL_Rect getFrameRect(int frameIndex) const;

    SDL_Texture* m_texture = nullptr;
    int m_frameWidth = 0;      // Pixlar per frame (bredd)
    int m_frameHeight = 0;     // Pixlar per frame (höjd)
    int m_columns = 1;         // Antal kolumner i sheeten
    int m_rows = 1;            // Antal rader i sheeten
    int m_textureWidth = 0;    // Total bredd på texturen
    int m_textureHeight = 0;   // Total höjd på texturen
};
