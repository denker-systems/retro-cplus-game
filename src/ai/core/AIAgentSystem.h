/**
 * @file AIAgentSystem.h
 * @brief Main orchestrator for AI agent functionality
 * 
 * @details
 * AIAgentSystem is the central coordinator that:
 * - Manages conversation history
 * - Handles LLM communication
 * - Executes tool calls
 * - Provides context to the LLM
 * 
 * @par Thread Safety
 * Main thread only. All AI operations are synchronous.
 */
#pragma once

#include "ILLMProvider.h"
#include "EditorToolRegistry.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <future>
#include <atomic>

namespace ai {

/**
 * @brief Agent state
 */
enum class AgentState {
    Idle,
    Processing,
    WaitingForConfirmation,
    Error
};

/**
 * @brief Context about current editor state
 */
struct EditorContext {
    std::string currentRoom;
    std::string currentLevel;
    std::string selectedActor;
    std::vector<std::string> recentActions;
    nlohmann::json additionalContext;
};

/**
 * @brief Configuration for the agent
 */
struct AgentConfig {
    std::string systemPrompt;
    int maxToolCalls = 10;      // Max tool calls per request
    bool requireConfirmation = true;  // Require confirmation for destructive actions
    bool streamResponses = false;
    LLMConfig llmConfig;
};

/**
 * @brief Callback for agent events
 */
using AgentCallback = std::function<void(const std::string& message, bool isError)>;

/**
 * @class AIAgentSystem
 * @brief Main AI agent orchestrator
 * 
 * @details
 * Usage:
 * @code
 * auto& agent = AIAgentSystem::instance();
 * agent.setProvider(std::make_unique<OpenAIProvider>());
 * agent.setCallback([](const std::string& msg, bool err) {
 *     // Handle agent output
 * });
 * agent.processUserMessage("Create a tavern room");
 * @endcode
 */
class AIAgentSystem {
public:
    /**
     * @brief Get singleton instance
     */
    static AIAgentSystem& instance();
    
    /**
     * @brief Initialize the agent system
     * @return true if successful
     */
    bool initialize();
    
    /**
     * @brief Shutdown and cleanup
     */
    void shutdown();
    
    /**
     * @brief Set the LLM provider
     * @param provider LLM provider (takes ownership)
     */
    void setProvider(std::unique_ptr<ILLMProvider> provider);
    
    /**
     * @brief Get current provider
     */
    ILLMProvider* getProvider() const { return m_provider.get(); }
    
    /**
     * @brief Set callback for agent events
     */
    void setCallback(AgentCallback callback);
    
    /**
     * @brief Set agent configuration
     */
    void setConfig(const AgentConfig& config);
    
    /**
     * @brief Get agent configuration
     */
    const AgentConfig& getConfig() const { return m_config; }
    
    /**
     * @brief Update editor context
     */
    void updateContext(const EditorContext& context);
    
    /**
     * @brief Process a user message
     * @param message User's natural language request
     * @return true if processing started
     */
    bool processUserMessage(const std::string& message);
    
    /**
     * @brief Confirm a pending action
     */
    void confirmAction();
    
    /**
     * @brief Cancel a pending action
     */
    void cancelAction();
    
    /**
     * @brief Get current state
     */
    AgentState getState() const { return m_state; }
    
    /**
     * @brief Get conversation history
     */
    const std::vector<Message>& getHistory() const { return m_history; }
    
    /**
     * @brief Clear conversation history
     */
    void clearHistory();
    
    /**
     * @brief Check if agent is ready
     */
    bool isReady() const;
    
    /**
     * @brief Get last error message
     */
    const std::string& getLastError() const { return m_lastError; }
    
    /**
     * @brief Update async processing (call every frame)
     */
    void update();
    
private:
    AIAgentSystem() = default;
    ~AIAgentSystem() = default;
    AIAgentSystem(const AIAgentSystem&) = delete;
    AIAgentSystem& operator=(const AIAgentSystem&) = delete;
    
    /**
     * @brief Build system prompt with context
     */
    std::string buildSystemPrompt() const;
    
    /**
     * @brief Execute tool calls from LLM response
     */
    void executeToolCalls(const std::vector<ToolCall>& toolCalls);
    
    /**
     * @brief Execute a single tool call
     */
    ToolResult executeToolCall(const ToolCall& toolCall);
    
    /**
     * @brief Send message to callback
     */
    void notify(const std::string& message, bool isError = false);
    
    // State
    bool m_initialized = false;
    AgentState m_state = AgentState::Idle;
    std::string m_lastError;
    
    // Components
    std::unique_ptr<ILLMProvider> m_provider;
    AgentCallback m_callback;
    AgentConfig m_config;
    EditorContext m_context;
    
    // Conversation
    std::vector<Message> m_history;
    
    // Pending confirmation
    std::vector<ToolCall> m_pendingToolCalls;
    
    // Async processing
    std::future<LLMResponse> m_asyncFuture;
    std::vector<Message> m_pendingMessages;
    std::vector<ToolDefinition> m_pendingTools;
    
    /**
     * @brief Handle completed async response
     */
    void handleAsyncResponse(const LLMResponse& response);
};

} // namespace ai
