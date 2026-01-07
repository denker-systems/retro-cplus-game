/**
 * @file AnthropicProvider.h
 * @brief Anthropic Claude API provider implementation
 * 
 * @details
 * Implements ILLMProvider for Anthropic's Claude API:
 * - Claude 4.5 Sonnet, Claude 4 Sonnet
 * - Claude 3.5 Haiku, Claude 3 Opus
 * - Tool calling support
 */
#pragma once

#include "ai/core/ILLMProvider.h"
#include <string>

namespace ai {

/**
 * @class AnthropicProvider
 * @brief Anthropic Claude API implementation
 */
class AnthropicProvider : public ILLMProvider {
public:
    AnthropicProvider();
    ~AnthropicProvider() override = default;
    
    // ILLMProvider interface
    const char* getName() const override { return "Anthropic"; }
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
     * @brief Set custom API base URL
     */
    void setBaseUrl(const std::string& url) { m_baseUrl = url; }
    
    /**
     * @brief Set model to use
     */
    void setModel(const std::string& model) { m_model = model; }
    
private:
    /**
     * @brief Convert Message to JSON for Anthropic API
     */
    nlohmann::json messageToJson(const Message& msg) const;
    
    /**
     * @brief Convert ToolDefinition to JSON for Anthropic API
     */
    nlohmann::json toolToJson(const ToolDefinition& tool) const;
    
    /**
     * @brief Parse Anthropic API response
     */
    LLMResponse parseResponse(const nlohmann::json& json) const;
    
    /**
     * @brief Make HTTP POST request
     */
    std::string httpPost(const std::string& endpoint, const nlohmann::json& body) const;
    
    std::string m_apiKey;
    std::string m_baseUrl = "https://api.anthropic.com";
    std::string m_model = "claude-sonnet-4-5-20250514";
};

} // namespace ai
