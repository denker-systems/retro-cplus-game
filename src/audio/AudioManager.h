/**
 * @file AudioManager.h
 * @brief Singleton för musik och ljudeffekter
 */
#pragma once

#include <SDL_mixer.h>
#include <string>
#include <unordered_map>

/**
 * @brief Hanterar all ljud och musik i spelet
 * 
 * Singleton-pattern för global åtkomst.
 * Stödjer bakgrundsmusik och ljudeffekter.
 */
class AudioManager {
public:
    /** @brief Hämta singleton-instansen */
    static AudioManager& instance();
    
    /** @brief Initiera SDL_mixer */
    bool init();
    
    /** @brief Stäng av och frigör resurser */
    void shutdown();
    
    // Musik
    /** @brief Ladda musikfil (OGG, MP3, WAV) */
    bool loadMusic(const std::string& id, const std::string& path);
    
    /** @brief Spela musik (loop = -1 för oändlig loop) */
    void playMusic(const std::string& id, int loops = -1);
    
    /** @brief Pausa musik */
    void pauseMusic();
    
    /** @brief Återuppta musik */
    void resumeMusic();
    
    /** @brief Stoppa musik */
    void stopMusic();
    
    /** @brief Fade out musik (ms) */
    void fadeOutMusic(int ms);
    
    /** @brief Crossfade till ny musik (ms) */
    void crossfadeToMusic(const std::string& id, int fadeMs = 1000);
    
    // Adaptiv musik (iMUSE-inspirerat)
    enum class MusicState {
        Normal,      // Standard rum-tema
        Tense,       // Nära lösning / fara
        Discovery,   // Hittade något viktigt
        Victory,     // Löste pussel
        Cutscene     // Speciell sekvens
    };
    
    /** @brief Sätt musik-state för adaptiv musik */
    void setMusicState(MusicState state);
    
    /** @brief Hämta nuvarande musik-state */
    MusicState getMusicState() const { return m_currentState; }
    
    /** @brief Spela kort "sting" (t.ex. vid pussel-lösning) */
    void playMusicSting(const std::string& id);
    
    // Ljudeffekter
    /** @brief Ladda ljudeffekt (WAV) */
    bool loadSound(const std::string& id, const std::string& path);
    
    /** @brief Spela ljudeffekt */
    void playSound(const std::string& id);
    
    /** @brief Spela ljudeffekt med volym (0-128) */
    void playSound(const std::string& id, int volume);
    
    // Volymkontroll
    /** @brief Sätt musikvolym (0.0 - 1.0) */
    void setMusicVolume(float volume);
    
    /** @brief Sätt SFX-volym (0.0 - 1.0) */
    void setSFXVolume(float volume);
    
    /** @brief Hämta musikvolym */
    float getMusicVolume() const { return m_musicVolume; }
    
    /** @brief Hämta SFX-volym */
    float getSFXVolume() const { return m_sfxVolume; }
    
    /** @brief Tysta all ljud */
    void mute();
    
    /** @brief Återställ ljud */
    void unmute();
    
    /** @brief Toggle mute */
    void toggleMute() { m_muted ? unmute() : mute(); }
    
    bool isMuted() const { return m_muted; }

private:
    AudioManager() = default;
    ~AudioManager();
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    std::unordered_map<std::string, Mix_Music*> m_music;    // Musik cache
    std::unordered_map<std::string, Mix_Chunk*> m_sounds;   // SFX cache
    
    float m_musicVolume = 0.7f;  // 0.0 - 1.0
    float m_sfxVolume = 1.0f;    // 0.0 - 1.0
    bool m_muted = false;
    bool m_initialized = false;
    
    // Adaptiv musik state
    MusicState m_currentState = MusicState::Normal;
    std::string m_currentMusicId;
    bool m_isCrossfading = false;
};
