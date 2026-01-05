/**
 * @file AudioManager.cpp
 * @brief Implementation av ljudhantering
 */
#include "AudioManager.h"
#include <iostream>

AudioManager& AudioManager::instance() {
    static AudioManager instance;
    return instance;
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::init() {
    if (m_initialized) return true;
    
    // Initiera SDL_mixer med standard-inställningar
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer init failed: " << Mix_GetError() << std::endl;
        return false;
    }
    
    m_initialized = true;
    std::cout << "AudioManager initialized" << std::endl;
    return true;
}

void AudioManager::shutdown() {
    if (!m_initialized) return;
    
    // Frigör all musik
    for (auto& pair : m_music) {
        Mix_FreeMusic(pair.second);
    }
    m_music.clear();
    
    // Frigör alla ljudeffekter
    for (auto& pair : m_sounds) {
        Mix_FreeChunk(pair.second);
    }
    m_sounds.clear();
    
    Mix_CloseAudio();
    m_initialized = false;
    std::cout << "AudioManager shutdown" << std::endl;
}

// === Musik ===

bool AudioManager::loadMusic(const std::string& id, const std::string& path) {
    // Kolla om redan laddad
    if (m_music.find(id) != m_music.end()) {
        return true;
    }
    
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music) {
        std::cerr << "Failed to load music: " << path << " - " << Mix_GetError() << std::endl;
        return false;
    }
    
    m_music[id] = music;
    std::cout << "Loaded music: " << id << " from " << path << std::endl;
    return true;
}

void AudioManager::playMusic(const std::string& id, int loops) {
    auto it = m_music.find(id);
    if (it == m_music.end()) {
        std::cerr << "Music not found: " << id << std::endl;
        return;
    }
    
    if (m_muted) return;
    
    // Sätt volym och spela
    Mix_VolumeMusic(static_cast<int>(m_musicVolume * MIX_MAX_VOLUME));
    Mix_PlayMusic(it->second, loops);
    m_currentMusicId = id;
}

void AudioManager::pauseMusic() {
    Mix_PauseMusic();
}

void AudioManager::resumeMusic() {
    Mix_ResumeMusic();
}

void AudioManager::stopMusic() {
    Mix_HaltMusic();
}

void AudioManager::fadeOutMusic(int ms) {
    Mix_FadeOutMusic(ms);
}

void AudioManager::crossfadeToMusic(const std::string& id, int fadeMs) {
    if (m_currentMusicId == id) {
        return; // Redan spelar denna musik
    }
    
    // Fade ut nuvarande musik
    if (Mix_PlayingMusic()) {
        Mix_FadeOutMusic(fadeMs);
    }
    
    // Vänta lite och spela ny musik med fade in
    // OBS: Detta är en enkel implementation - för production skulle man
    // använda en timer/callback för bättre timing
    auto it = m_music.find(id);
    if (it != m_music.end() && !m_muted) {
        Mix_VolumeMusic(static_cast<int>(m_musicVolume * MIX_MAX_VOLUME));
        Mix_FadeInMusic(it->second, -1, fadeMs);
        m_currentMusicId = id;
    }
}

void AudioManager::setMusicState(MusicState state) {
    if (m_currentState == state) return;
    
    MusicState oldState = m_currentState;
    m_currentState = state;
    
    // Här kan vi i framtiden lägga till logik för att byta musik-lager
    // baserat på state (t.ex. lägga till tension-layer)
    std::cout << "Music state changed: " << static_cast<int>(oldState) 
              << " -> " << static_cast<int>(state) << std::endl;
}

void AudioManager::playMusicSting(const std::string& id) {
    // Spela kort musikstycke (sting) utan att stoppa bakgrundsmusiken
    // Detta kräver att stinget är laddat som ljudeffekt, inte musik
    auto it = m_sounds.find(id);
    if (it != m_sounds.end() && !m_muted) {
        Mix_VolumeChunk(it->second, static_cast<int>(m_musicVolume * MIX_MAX_VOLUME));
        Mix_PlayChannel(-1, it->second, 0);
    }
}

// === Ljudeffekter ===

bool AudioManager::loadSound(const std::string& id, const std::string& path) {
    // Kolla om redan laddad
    if (m_sounds.find(id) != m_sounds.end()) {
        return true;
    }
    
    Mix_Chunk* sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        std::cerr << "Failed to load sound: " << path << " - " << Mix_GetError() << std::endl;
        return false;
    }
    
    m_sounds[id] = sound;
    std::cout << "Loaded sound: " << id << " from " << path << std::endl;
    return true;
}

void AudioManager::playSound(const std::string& id) {
    playSound(id, static_cast<int>(m_sfxVolume * MIX_MAX_VOLUME));
}

void AudioManager::playSound(const std::string& id, int volume) {
    auto it = m_sounds.find(id);
    if (it == m_sounds.end()) {
        // Tyst miss - inte logga för varje försök
        return;
    }
    
    if (m_muted) return;
    
    // Sätt volym och spela på första lediga kanal (-1)
    Mix_VolumeChunk(it->second, volume);
    Mix_PlayChannel(-1, it->second, 0);
}

// === Volymkontroll ===

void AudioManager::setMusicVolume(float volume) {
    m_musicVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
    
    // Uppdatera direkt om musik spelas
    if (Mix_PlayingMusic()) {
        Mix_VolumeMusic(static_cast<int>(m_musicVolume * MIX_MAX_VOLUME));
    }
}

void AudioManager::setSFXVolume(float volume) {
    m_sfxVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
}

void AudioManager::mute() {
    m_muted = true;
    Mix_VolumeMusic(0);
    // Stoppa alla ljudeffekter
    Mix_HaltChannel(-1);
}

void AudioManager::unmute() {
    m_muted = false;
    Mix_VolumeMusic(static_cast<int>(m_musicVolume * MIX_MAX_VOLUME));
}
