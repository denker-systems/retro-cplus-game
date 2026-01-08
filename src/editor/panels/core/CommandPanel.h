/**
 * @file CommandPanel.h
 * @brief Command panel with CLI-style input for AI and power users
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>
#include <vector>
#include <deque>
#include <functional>

class EditorContext;

/**
 * @brief Command execution result
 */
struct CommandResult {
    bool success = true;
    std::string output;
    
    static CommandResult ok(const std::string& msg) { return {true, msg}; }
    static CommandResult error(const std::string& msg) { return {false, msg}; }
};

/**
 * @brief Command history entry
 */
struct CommandHistoryEntry {
    std::string command;
    std::string output;
    bool success;
    std::string timestamp;
};

/**
 * @brief Command panel with input field and history
 */
class CommandPanel : public IEditorPanel {
public:
    explicit CommandPanel(EditorContext& context);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
    /**
     * @brief Execute a command string
     * @param command The command to execute (e.g., "list_scenes" or "create_hotspot tavern door")
     * @return Result with success/error and output
     */
    CommandResult executeCommand(const std::string& command);
    
    /**
     * @brief Get available commands for autocomplete
     */
    std::vector<std::string> getAvailableCommands() const;
    
    /**
     * @brief Clear command history
     */
    void clearHistory();
    
    /**
     * @brief Add output to history (for external use)
     */
    void addOutput(const std::string& text, bool isError = false);
    
private:
    void processInput();
    void updateAutocomplete();
    std::vector<std::string> parseCommand(const std::string& input);
    
    EditorContext& m_context;
    std::string m_id = "command";
    std::string m_title = "Command";
    
    // Input
    char m_inputBuffer[512] = {0};
    bool m_focusInput = false;
    
    // History
    std::deque<CommandHistoryEntry> m_history;
    static constexpr size_t MAX_HISTORY = 100;
    std::vector<std::string> m_commandHistory;  // For up/down navigation
    int m_historyIndex = -1;
    
    // Autocomplete
    std::vector<std::string> m_suggestions;
    int m_selectedSuggestion = -1;
    bool m_showSuggestions = false;
    
    // Settings
    bool m_autoScroll = true;
};
