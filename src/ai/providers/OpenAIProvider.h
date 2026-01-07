/**
 * @file OpenAIProvider.h
 * @brief OpenAI API provider implementation
 * 
 * @details
 * Implements ILLMProvider for OpenAI's API:
 * - GPT-4, GPT-4o, GPT-3.5-turbo
 * - Tool calling support
 * - Streaming support
 */
#pragma once

#include "ai/core/ILLMProvider.h"
#include <string>

namespace ai {

/**
 * @class OpenAIProvider
 * @brief OpenAI API implementation
 */
class OpenAIProvider : public ILLMProvider {
public:
    OpenAIProvider();
    ~OpenAIProvider() override = default;
    
    // ILLMProvider interface
    const char* getName() const override { return "OpenAI"; }
    bool isAvailable() const override;
    void setApiKey(const std::string& apiKey) override;
    
    LLMResponse chat(
        const std::vector<Message>& messages,
        const std::vector<ToolDefinition>& tools,
        const LLMConfig& config = {}
    ) override;
    
    LLMResponse chatStream(
        const std::vector<Message>& messages,
        const std::vector<ToolDefinition>& tools,
        StreamCallback callback,
        const LLMConfig& config = {}
    ) override;
    
    std::vector<std::string> getAvailableModels() const override;
    
    /**
     * @brief Set custom API base URL (for proxies/alternatives)
     */
    void setBaseUrl(const std::string& url) { m_baseUrl = url; }
    
private:
    /**
     * @brief Convert Message to JSON for API
     */
    nlohmann::json messageToJson(const Message& msg) const;
    
    /**
     * @brief Convert ToolDefinition to JSON for API
     */
    nlohmann::json toolToJson(const ToolDefinition& tool) const;
    
    /**
     * @brief Parse API response
     */
    LLMResponse parseResponse(const nlohmann::json& json) const;
    
    /**
     * @brief Make HTTP POST request
     */
    std::string httpPost(const std::string& endpoint, const nlohmann::json& body) const;
    
    std::string m_apiKey;
    std::string m_baseUrl = "https://api.openai.com";
};

} // namespace ai
