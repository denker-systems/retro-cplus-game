/**
 * @file AIChatPanel.h
 * @brief ImGui panel for AI chat interface
 * 
 * @details
 * Provides a chat-like interface for interacting with the AI agent.
 * Features:
 * - Message history display
 * - Input field for user messages
 * - Tool call visualization
 * - Confirmation dialogs for destructive actions
 */
#pragma once

#include "ai/core/AIAgentSystem.h"
#include <string>
#include <vector>
#include <deque>

namespace ai {

/**
 * @brief Chat message for display
 */
struct ChatMessage {
    enum class Type {
        User,
        Assistant,
        ToolCall,
        Error,
        System
    };
    
    Type type;
    std::string content;
    std::string timestamp;
    bool isToolResult = false;
};

/**
 * @class AIChatPanel
 * @brief ImGui panel for AI chat interface
 */
class AIChatPanel {
public:
    /**
     * @brief Get singleton instance
     */
    static AIChatPanel& instance();
    
    /**
     * @brief Initialize the panel
     */
    void initialize();
    
    /**
     * @brief Shutdown and cleanup
     */
    void shutdown();
    
    /**
     * @brief Render the panel (call in ImGui render loop)
     */
    void render();
    
    /**
     * @brief Check if panel is visible
     */
    bool isVisible() const { return m_visible; }
    
    /**
     * @brief Set panel visibility
     */
    void setVisible(bool visible) { m_visible = visible; }
    
    /**
     * @brief Toggle panel visibility
     */
    void toggleVisible() { m_visible = !m_visible; }
    
    /**
     * @brief Add a message to display
     */
    void addMessage(const ChatMessage& message);
    
    /**
     * @brief Clear all messages
     */
    void clearMessages();
    
private:
    AIChatPanel() = default;
    ~AIChatPanel() = default;
    AIChatPanel(const AIChatPanel&) = delete;
    AIChatPanel& operator=(const AIChatPanel&) = delete;
    
    /**
     * @brief Render message list
     */
    void renderMessages();
    
    /**
     * @brief Render input area
     */
    void renderInput();
    
    /**
     * @brief Render settings popup
     */
    void renderSettings();
    
    /**
     * @brief Render confirmation dialog
     */
    void renderConfirmationDialog();
    
    /**
     * @brief Handle agent callback
     */
    void onAgentMessage(const std::string& message, bool isError);
    
    /**
     * @brief Get current timestamp string
     */
    std::string getTimestamp() const;
    
    /**
     * @brief Send user message
     */
    void sendMessage();
    
    // State
    bool m_initialized = false;
    bool m_visible = true;  // Visible by default
    bool m_showSettings = false;
    bool m_scrollToBottom = false;
    
    // Input
    char m_inputBuffer[1024] = {0};
    
    // Messages
    std::deque<ChatMessage> m_messages;
    static constexpr size_t MAX_MESSAGES = 100;
    
    // Settings
    std::string m_apiKey;
    std::string m_selectedModel = "gpt-4o";
};

} // namespace ai
