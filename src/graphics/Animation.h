/**
 * @file Animation.h
 * @brief Tidsbaserad animation från sprite sheet
 */
#pragma once

#include <SDL.h>

class SpriteSheet;

/**
 * @brief Hanterar tidsbaserad animation av sprite frames
 * 
 * Spelar upp en sekvens av frames från en sprite sheet
 * med konfigurerbar hastighet och loop-beteende.
 */
class Animation {
public:
    Animation() = default;
    
    /**
     * @brief Skapa animation med alla parametrar
     * @param sheet Sprite sheet att animera
     * @param startFrame Första frame i sekvensen
     * @param endFrame Sista frame i sekvensen
     * @param frameTime Sekunder per frame
     * @param loop Upprepa när sekvensen är klar
     */
    Animation(SpriteSheet* sheet, int startFrame, int endFrame, float frameTime, bool loop = true);

    /** @brief Initiera animation (alternativ till konstruktor) */
    void init(SpriteSheet* sheet, int startFrame, int endFrame, float frameTime, bool loop = true);
    
    /** @brief Uppdatera animation, anropas varje frame */
    void update(float deltaTime);
    
    /** @brief Rita nuvarande frame */
    void render(SDL_Renderer* renderer, int x, int y, bool flipH = false);
    void renderScaled(SDL_Renderer* renderer, int x, int y, float scale, bool flipH = false);

    // Kontroll
    void play();
    void pause();
    void reset();
    void setFrame(int frame);

    // Status
    bool isPlaying() const { return m_playing; }
    bool isFinished() const { return m_finished; }
    int getCurrentFrame() const { return m_currentFrame; }

private:
    SpriteSheet* m_sheet = nullptr;
    int m_startFrame = 0;      // Första frame i animationen
    int m_endFrame = 0;        // Sista frame i animationen
    int m_currentFrame = 0;    // Nuvarande frame
    float m_frameTime = 0.1f;  // Sekunder per frame
    float m_timer = 0.0f;      // Tid sedan senaste frame-byte
    bool m_loop = true;        // Upprepa animation
    bool m_playing = true;     // Animation spelar
    bool m_finished = false;   // Animation har nått slutet (om !loop)
};
