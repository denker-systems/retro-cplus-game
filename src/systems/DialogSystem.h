/**
 * @file DialogSystem.h
 * @brief Dialog system för NPC-konversationer
 */
#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

/**
 * @brief Ett val i en dialog
 */
struct DialogChoice {
    std::string text;           // Valtexten som visas
    int nextNodeId = -1;        // Nästa nod att gå till (-1 = avsluta)
    std::string condition;      // Villkor för att visa valet (optional)
};

/**
 * @brief En nod i dialogträdet
 */
struct DialogNode {
    int id = 0;
    std::string speaker;        // Vem som pratar
    std::string text;           // Dialogtexten
    std::vector<DialogChoice> choices;  // Valmöjligheter
    int nextNodeId = -1;        // Auto-fortsätt till nästa nod (om inga val)
};

/**
 * @brief Ett komplett dialogträd
 */
struct DialogTree {
    std::string id;             // Unik identifierare
    std::string npcName;        // NPC som äger dialogen
    std::vector<DialogNode> nodes;
    int startNodeId = 0;        // Startnod
};

/**
 * @brief Hanterar dialoger och konversationer
 */
class DialogSystem {
public:
    static DialogSystem& instance();
    
    /** @brief Lägg till ett dialogträd */
    void addDialog(const DialogTree& tree);
    
    /** @brief Starta en dialog */
    bool startDialog(const std::string& dialogId);
    
    /** @brief Avsluta nuvarande dialog */
    void endDialog();
    
    /** @brief Välj ett alternativ (0-baserat index) */
    void selectChoice(int index);
    
    /** @brief Fortsätt till nästa nod (för dialoger utan val) */
    void advance();
    
    // Getters
    bool isActive() const { return m_active; }
    const DialogNode* getCurrentNode() const;
    const std::string& getCurrentSpeaker() const;
    const std::string& getCurrentText() const;
    const std::vector<DialogChoice>& getCurrentChoices() const;
    
    /** @brief Callback när dialog startar/slutar */
    void setOnDialogEnd(std::function<void()> callback) { m_onDialogEnd = callback; }

private:
    DialogSystem() = default;
    
    std::unordered_map<std::string, DialogTree> m_dialogs;
    DialogTree* m_currentDialog = nullptr;
    int m_currentNodeId = 0;
    bool m_active = false;
    
    std::function<void()> m_onDialogEnd;
    
    const DialogNode* findNode(int id) const;
};
