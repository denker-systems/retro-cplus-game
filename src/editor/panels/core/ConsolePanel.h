/**
 * @file ConsolePanel.h
 * @brief Console/log panel för status och debug info
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>
#include <vector>
#include <deque>

class EditorContext;

/**
 * @brief Log entry
 */
struct LogEntry {
    enum class Level { Info, Warning, Error };
    Level level;
    std::string message;
    std::string timestamp;
};

/**
 * @brief Console panel för logg och status
 */
class ConsolePanel : public IEditorPanel {
public:
    explicit ConsolePanel(EditorContext& context);
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
    // Logging
    void log(const std::string& message);
    void logWarning(const std::string& message);
    void logError(const std::string& message);
    void clear();
    
private:
    void addEntry(LogEntry::Level level, const std::string& message);
    
    EditorContext& m_context;
    std::string m_id = "console";
    std::string m_title = "Console";
    
    std::deque<LogEntry> m_entries;
    static constexpr size_t MAX_ENTRIES = 1000;
    bool m_autoScroll = true;
    bool m_showInfo = true;
    bool m_showWarnings = true;
    bool m_showErrors = true;
};
