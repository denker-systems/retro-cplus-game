/**
 * @file ILLMProvider.h
 * @brief Abstract interface for LLM API providers
 * 
 * @details
 * Provides abstraction layer for different LLM backends:
 * - OpenAI (GPT-4, GPT-3.5)
 * - Anthropic (Claude)
 * - Local (Ollama, llama.cpp) - future
 */
#pragma once

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

namespace ai {

/**
 * @brief Message role in conversation
 */
enum class MessageRole {
    System,
    User,
    Assistant,
    Tool
};

/**
 * @brief Single message in conversation
 */
struct Message {
    MessageRole role;
    std::string content;
    std::string toolCallId;  // For tool responses
    
    static Message system(const std::string& content) {
        return {MessageRole::System, content, ""};
    }
    
    static Message user(const std::string& content) {
        return {MessageRole::User, content, ""};
    }
    
    static Message assistant(const std::string& content) {
        return {MessageRole::Assistant, content, ""};
    }
    
    static Message toolResult(const std::string& callId, const std::string& content) {
        return {MessageRole::Tool, content, callId};
    }
};

/**
 * @brief Tool call request from LLM
 */
struct ToolCall {
    std::string id;
    std::string name;
    nlohmann::json arguments;
};

/**
 * @brief Response from LLM
 */
struct LLMResponse {
    std::string content;
    std::vector<ToolCall> toolCalls;
    bool success = true;
    std::string error;
    int tokensUsed = 0;
};

/**
 * @brief Configuration for LLM request
 */
struct LLMConfig {
    std::string model = "gpt-4o";
    float temperature = 0.7f;
    int maxTokens = 4096;
    bool stream = false;
};

/**
 * @brief Tool definition for LLM
 */
struct ToolDefinition {
    std::string name;
    std::string description;
    nlohmann::json parameters;  // JSON Schema
};

/**
 * @brief Callback for streaming responses
 */
using StreamCallback = std::function<void(const std::string& chunk)>;

/**
 * @class ILLMProvider
 * @brief Abstract interface for LLM providers
 * 
 * @details
 * Implementations should handle:
 * - API authentication
 * - Request formatting
 * - Response parsing
 * - Error handling
 * - Rate limiting
 */
class ILLMProvider {
public:
    virtual ~ILLMProvider() = default;
    
    /**
     * @brief Get provider name
     */
    virtual const char* getName() const = 0;
    
    /**
     * @brief Check if provider is configured and ready
     */
    virtual bool isAvailable() const = 0;
    
    /**
     * @brief Set API key
     */
    virtual void setApiKey(const std::string& apiKey) = 0;
    
    /**
     * @brief Send chat completion request
     * @param messages Conversation history
     * @param tools Available tools
     * @param config Request configuration
     * @return LLM response
     */
    virtual LLMResponse chat(
        const std::vector<Message>& messages,
        const std::vector<ToolDefinition>& tools,
        const LLMConfig& config = {}
    ) = 0;
    
    /**
     * @brief Send streaming chat completion request
     * @param messages Conversation history
     * @param tools Available tools
     * @param callback Callback for each chunk
     * @param config Request configuration
     * @return Final response
     */
    virtual LLMResponse chatStream(
        const std::vector<Message>& messages,
        const std::vector<ToolDefinition>& tools,
        StreamCallback callback,
        const LLMConfig& config = {}
    ) = 0;
    
    /**
     * @brief Get available models for this provider
     */
    virtual std::vector<std::string> getAvailableModels() const = 0;
};

} // namespace ai
