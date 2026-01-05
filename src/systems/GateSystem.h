/**
 * @file GateSystem.h
 * @brief System för "gates" med alternativa lösningar (Fate of Atlantis-inspirerat)
 * 
 * Ett "gate" är ett hinder som kan lösas på flera sätt:
 * - Social approach (dialog/övertalning)
 * - Logik approach (observation/utforskning)
 * - Item approach (rätt föremål)
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

/**
 * @brief Typ av approach för att lösa ett gate
 */
enum class ApproachType {
    Social,      // Dialog/övertalning
    Logic,       // Observation/utforskning
    Item,        // Använda rätt föremål
    Stealth      // Smyga/undvika (framtida)
};

/**
 * @brief En approach för att lösa ett gate
 */
struct GateApproach {
    ApproachType type;
    std::string description;        // "Övertyga vakten"
    std::string requiredDialog;     // Dialog som måste slutföras
    std::string requiredItem;       // Item som måste användas
    std::string requiredQuest;      // Quest som måste vara aktiv/klar
    std::string requiredFlag;       // Flag som måste vara satt
    bool completed = false;
};

/**
 * @brief Ett gate som kan lösas på flera sätt
 */
struct Gate {
    std::string id;
    std::string name;
    std::string description;        // "Komma in i biblioteket"
    std::vector<GateApproach> approaches;
    bool isOpen = false;
    std::string onOpenAction;       // Action att köra när gate öppnas
};

/**
 * @brief Hanterar gates med alternativa lösningar
 */
class GateSystem {
public:
    static GateSystem& instance();
    
    /** @brief Registrera ett gate */
    void addGate(const Gate& gate);
    
    /** @brief Hämta gate */
    Gate* getGate(const std::string& gateId);
    
    /** @brief Kolla om gate är öppet */
    bool isGateOpen(const std::string& gateId) const;
    
    /** @brief Försök öppna gate med specifik approach */
    bool tryOpenGate(const std::string& gateId, ApproachType approach);
    
    /** @brief Markera approach som completed */
    void completeApproach(const std::string& gateId, ApproachType approach);
    
    /** @brief Hämta tillgängliga approaches för ett gate */
    std::vector<GateApproach> getAvailableApproaches(const std::string& gateId) const;
    
    /** @brief Callback när gate öppnas */
    void setOnGateOpened(std::function<void(const std::string&, ApproachType)> callback) {
        m_onGateOpened = callback;
    }
    
    /** @brief Rensa alla gates */
    void clear();

private:
    GateSystem() = default;
    ~GateSystem() = default;
    GateSystem(const GateSystem&) = delete;
    GateSystem& operator=(const GateSystem&) = delete;
    
    bool checkApproachRequirements(const GateApproach& approach) const;
    
    std::unordered_map<std::string, Gate> m_gates;
    std::function<void(const std::string&, ApproachType)> m_onGateOpened;
};
