/**
 * @file RuntimeDialogSystem.h
 * @brief Dialog system för runtime - visar dialog UI och hanterar val
 */
#pragma once

#include <string>
#include <vector>
#include <functional>

struct DialogChoice {
    std::string text;
    int nextNodeId;
    std::string condition;
};

struct DialogNode {
    int id;
    std::string speaker;
    std::string text;
    std::vector<DialogChoice> choices;
    int nextNodeId = -1;  // For linear dialogs
    std::string action;   // Quest/item triggers
};

struct Dialog {
    std::string id;
    std::vector<DialogNode> nodes;
};

/**
 * @brief Manages dialog display and player choices
 */
class RuntimeDialogSystem {
public:
    RuntimeDialogSystem();
    ~RuntimeDialogSystem() = default;
    
    /**
     * @brief Load dialogs from DataLoader
     */
    void loadDialogs();
    
    /**
     * @brief Start a dialog by ID
     */
    bool startDialog(const std::string& dialogId);
    
    /**
     * @brief End current dialog
     */
    void endDialog();
    
    /**
     * @brief Select a choice (0-indexed)
     */
    void selectChoice(int choiceIndex);
    
    /**
     * @brief Advance to next node (for linear dialogs)
     */
    void advance();
    
    /**
     * @brief Check if dialog is active
     */
    bool isActive() const { return m_active; }
    
    /**
     * @brief Get current speaker name
     */
    const std::string& getCurrentSpeaker() const { return m_currentSpeaker; }
    
    /**
     * @brief Get current dialog text
     */
    const std::string& getCurrentText() const { return m_currentText; }
    
    /**
     * @brief Get current choices
     */
    const std::vector<DialogChoice>& getCurrentChoices() const { return m_currentChoices; }
    
    /**
     * @brief Set callback for dialog actions (quest triggers etc)
     */
    void setActionCallback(std::function<void(const std::string&)> callback) {
        m_actionCallback = callback;
    }

private:
    void setCurrentNode(int nodeId);
    Dialog* findDialog(const std::string& id);
    
    std::vector<Dialog> m_dialogs;
    Dialog* m_currentDialog = nullptr;
    int m_currentNodeId = -1;
    
    bool m_active = false;
    std::string m_currentSpeaker;
    std::string m_currentText;
    std::vector<DialogChoice> m_currentChoices;
    
    std::function<void(const std::string&)> m_actionCallback;
};
