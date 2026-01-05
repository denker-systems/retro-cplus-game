/**
 * @file RecapSystem.h
 * @brief System för "recap barks" - karaktären tänker högt vid inaktivitet
 * 
 * Baserat på LucasArts-principen att hjälpa spelaren utan att bryta immersion
 */
#pragma once

#include <string>
#include <vector>
#include <functional>

/**
 * @brief Hanterar automatiska hints/påminnelser när spelaren är inaktiv
 */
class RecapSystem {
public:
    static RecapSystem& instance();
    
    /** @brief Uppdatera timer */
    void update(float deltaTime);
    
    /** @brief Återställ inaktivitetstimer (anropas vid spelarinput) */
    void resetTimer();
    
    /** @brief Sätt callback för att visa recap */
    void setOnRecap(std::function<void(const std::string&)> callback) {
        m_onRecap = callback;
    }
    
    /** @brief Sätt tid innan recap triggas (sekunder) */
    void setRecapDelay(float seconds) { m_recapDelay = seconds; }
    
    /** @brief Hämta nuvarande delay */
    float getRecapDelay() const { return m_recapDelay; }
    
    /** @brief Aktivera/inaktivera systemet */
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    
    /** @brief Registrera recap-meddelanden för ett quest/objective */
    void registerRecaps(const std::string& questId,
                        const std::string& objectiveId,
                        const std::vector<std::string>& recaps);
    
    /** @brief Ta bort recaps för ett quest/objective */
    void clearRecaps(const std::string& questId, const std::string& objectiveId);
    
    /** @brief Rensa alla recaps */
    void clearAllRecaps();

private:
    RecapSystem() = default;
    ~RecapSystem() = default;
    RecapSystem(const RecapSystem&) = delete;
    RecapSystem& operator=(const RecapSystem&) = delete;
    
    std::string makeKey(const std::string& questId, 
                        const std::string& objectiveId) const;
    
    std::string getRecapForActiveQuest() const;
    
    // Quest/Objective -> recap-meddelanden
    std::unordered_map<std::string, std::vector<std::string>> m_recaps;
    
    // Callback
    std::function<void(const std::string&)> m_onRecap;
    
    // Timer
    float m_inactivityTimer = 0.0f;
    float m_recapDelay = 60.0f;  // Sekunder innan recap
    bool m_enabled = true;
    bool m_hasShownRecap = false;  // Visa bara en gång per inaktivitetsperiod
};
