/**
 * @file HintSystem.h
 * @brief Tiered hint system för att hjälpa spelare utan att spoila
 * 
 * Baserat på Ron Gilberts princip: "Sub-goals need to be obvious"
 * Hints ges i 4 nivåer för att bevara "aha-känslan"
 */
#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

/**
 * @brief Hint-nivåer (progressivt mer specifika)
 */
enum class HintLevel {
    Goal,       // Vad är målet? "Jag behöver komma in i biblioteket"
    Location,   // Var ska jag leta? "Någon i tavernan kanske vet något"
    Method,     // Hur gör jag? "Bartendern verkar veta mer än han säger"
    Solution    // Exakt lösning "Prata med bartendern om bibliotekarien"
};

/**
 * @brief Data för en hint-sekvens kopplad till ett quest/objective
 */
struct HintData {
    std::string questId;
    std::string objectiveId;
    std::vector<std::string> hints;  // Index 0-3 för Goal→Solution
};

/**
 * @brief Singleton för hint-hantering
 */
class HintSystem {
public:
    static HintSystem& instance();
    
    /** @brief Registrera hints för ett quest/objective */
    void registerHints(const std::string& questId, 
                       const std::string& objectiveId,
                       const std::vector<std::string>& hints);
    
    /** @brief Hämta hint på specifik nivå */
    std::string getHint(const std::string& questId, 
                        const std::string& objectiveId,
                        HintLevel level) const;
    
    /** @brief Hämta nästa hint (ökar nivå automatiskt) */
    std::string requestHint(const std::string& questId,
                           const std::string& objectiveId);
    
    /** @brief Hämta hint för aktivt quest/objective */
    std::string requestHintForActiveQuest();
    
    /** @brief Återställ hint-nivå för ett objective */
    void resetHintLevel(const std::string& questId,
                        const std::string& objectiveId);
    
    /** @brief Återställ alla hint-nivåer */
    void resetAllHints();
    
    /** @brief Hämta nuvarande hint-nivå */
    HintLevel getCurrentLevel(const std::string& questId,
                              const std::string& objectiveId) const;
    
    /** @brief Callback när hint visas */
    void setOnHintShown(std::function<void(const std::string&, HintLevel)> callback) {
        m_onHintShown = callback;
    }
    
    /** @brief Cooldown mellan hints (sekunder) */
    void setHintCooldown(float seconds) { m_cooldown = seconds; }
    float getHintCooldown() const { return m_cooldown; }
    
    /** @brief Kolla om cooldown är aktiv */
    bool isOnCooldown() const;
    
    /** @brief Uppdatera cooldown timer */
    void update(float deltaTime);

private:
    HintSystem() = default;
    ~HintSystem() = default;
    HintSystem(const HintSystem&) = delete;
    HintSystem& operator=(const HintSystem&) = delete;
    
    std::string makeKey(const std::string& questId, 
                        const std::string& objectiveId) const;
    
    // Quest/Objective -> HintData
    std::unordered_map<std::string, HintData> m_hints;
    
    // Quest/Objective -> nuvarande hint-nivå
    std::unordered_map<std::string, int> m_currentLevels;
    
    // Callback
    std::function<void(const std::string&, HintLevel)> m_onHintShown;
    
    // Cooldown
    float m_cooldown = 30.0f;      // Sekunder mellan hints
    float m_cooldownTimer = 0.0f;  // Nuvarande timer
};
