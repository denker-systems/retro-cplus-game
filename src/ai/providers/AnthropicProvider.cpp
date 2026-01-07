/**
 * @file AnthropicProvider.cpp
 * @brief Implementation of Anthropic Claude API provider
 */
#include "AnthropicProvider.h"
#include "engine/utils/Logger.h"

#include <httplib.h>
#include <stdexcept>

namespace ai {

AnthropicProvider::AnthropicProvider() {
    LOG_DEBUG("[Anthropic] Provider created");
}

bool AnthropicProvider::isAvailable() const {
    return !m_apiKey.empty();
}

void AnthropicProvider::setApiKey(const std::string& apiKey) {
    m_apiKey = apiKey;
    LOG_INFO("[Anthropic] API key set (length: " + std::to_string(apiKey.length()) + ")");
}

std::vector<std::string> AnthropicProvider::getAvailableModels() const {
    return {
        "claude-sonnet-4-5-20250514",   // Claude 4.5 Sonnet (recommended)
        "claude-sonnet-4-20250514",     // Claude 4 Sonnet
        "claude-3-5-haiku-20241022",    // Fast, cost-effective
        "claude-3-opus-20240229"        // Most capable Claude 3
    };
}

nlohmann::json AnthropicProvider::messageToJson(const Message& msg) const {
    nlohmann::json j;
    
    // Anthropic uses different role names
    switch (msg.role) {
        case MessageRole::System:
            // System messages are handled separately in Anthropic API
            j["role"] = "user";  // Will be extracted as system param
            break;
        case MessageRole::User:
            j["role"] = "user";
            break;
        case MessageRole::Assistant:
            j["role"] = "assistant";
            break;
        case MessageRole::Tool:
            j["role"] = "user";
            j["content"] = nlohmann::json::array();
            j["content"].push_back({
                {"type", "tool_result"},
                {"tool_use_id", msg.toolCallId},
                {"content", msg.content}
            });
            return j;
    }
    
    j["content"] = msg.content;
    return j;
}

nlohmann::json AnthropicProvider::toolToJson(const ToolDefinition& tool) const {
    // Anthropic tool format
    return {
        {"name", tool.name},
        {"description", tool.description},
        {"input_schema", tool.parameters}
    };
}

LLMResponse AnthropicProvider::parseResponse(const nlohmann::json& json) const {
    LLMResponse response;
    
    try {
        // Check for error
        if (json.contains("error")) {
            response.success = false;
            if (json["error"].contains("message")) {
                response.error = json["error"]["message"].get<std::string>();
            } else {
                response.error = json["error"].dump();
            }
            LOG_ERROR("[Anthropic] API error: " + response.error);
            return response;
        }
        
        // Parse content blocks
        if (json.contains("content")) {
            for (const auto& block : json["content"]) {
                std::string type = block.value("type", "");
                
                if (type == "text") {
                    response.content += block["text"].get<std::string>();
                } else if (type == "tool_use") {
                    ToolCall call;
                    call.id = block["id"].get<std::string>();
                    call.name = block["name"].get<std::string>();
                    call.arguments = block["input"];
                    response.toolCalls.push_back(std::move(call));
                }
            }
        }
        
        // Get token usage
        if (json.contains("usage")) {
            int inputTokens = json["usage"].value("input_tokens", 0);
            int outputTokens = json["usage"].value("output_tokens", 0);
            response.tokensUsed = inputTokens + outputTokens;
        }
        
        response.success = true;
        LOG_DEBUG("[Anthropic] Response parsed: " + std::to_string(response.content.length()) + 
                  " chars, " + std::to_string(response.toolCalls.size()) + " tool calls");
                  
    } catch (const std::exception& e) {
        response.success = false;
        response.error = std::string("Parse error: ") + e.what();
        LOG_ERROR("[Anthropic] " + response.error);
    }
    
    return response;
}

LLMResponse AnthropicProvider::chat(
    const std::vector<Message>& messages,
    const std::vector<ToolDefinition>& tools,
    const LLMConfig& config
) {
    LOG_INFO("[Anthropic] chat() called with " + std::to_string(messages.size()) + 
             " messages, " + std::to_string(tools.size()) + " tools");
    
    if (!isAvailable()) {
        LLMResponse errResp;
        errResp.success = false;
        errResp.error = "API key not set";
        LOG_ERROR("[Anthropic] " + errResp.error);
        return errResp;
    }
    
    // Build request body - Anthropic format
    nlohmann::json body;
    
    // Use model from config or default
    std::string model = config.model;
    if (model.empty() || model.find("gpt") != std::string::npos) {
        model = m_model;  // Use Anthropic default if OpenAI model specified
    }
    body["model"] = model;
    body["max_tokens"] = config.maxTokens;
    
    // Extract system message
    std::string systemPrompt;
    nlohmann::json messagesJson = nlohmann::json::array();
    
    for (const auto& msg : messages) {
        if (msg.role == MessageRole::System) {
            systemPrompt += msg.content + "\n";
        } else {
            messagesJson.push_back(messageToJson(msg));
        }
    }
    
    if (!systemPrompt.empty()) {
        body["system"] = systemPrompt;
    }
    body["messages"] = messagesJson;
    
    // Add tools
    if (!tools.empty()) {
        nlohmann::json toolsJson = nlohmann::json::array();
        for (const auto& tool : tools) {
            toolsJson.push_back(toolToJson(tool));
        }
        body["tools"] = toolsJson;
    }
    
    LOG_DEBUG("[Anthropic] Request body built, model: " + model);
    
    // Make request
    try {
        std::string responseStr = httpPost("/v1/messages", body);
        LOG_DEBUG("[Anthropic] Response received: " + std::to_string(responseStr.length()) + " bytes");
        
        nlohmann::json responseJson = nlohmann::json::parse(responseStr);
        return parseResponse(responseJson);
    } catch (const std::exception& e) {
        LLMResponse errResp;
        errResp.success = false;
        errResp.error = std::string("Request failed: ") + e.what();
        LOG_ERROR("[Anthropic] " + errResp.error);
        return errResp;
    }
}

LLMResponse AnthropicProvider::chatStream(
    const std::vector<Message>& messages,
    const std::vector<ToolDefinition>& tools,
    StreamCallback callback,
    const LLMConfig& config
) {
    // Streaming implementation would go here
    // For now, fall back to non-streaming
    (void)callback;
    LOG_DEBUG("[Anthropic] chatStream() falling back to non-streaming");
    return chat(messages, tools, config);
}

std::string AnthropicProvider::httpPost(const std::string& endpoint, const nlohmann::json& body) const {
    LOG_DEBUG("[Anthropic] httpPost to " + m_baseUrl + endpoint);
    
    std::string requestBody = body.dump();
    LOG_DEBUG("[Anthropic] Request: " + requestBody.substr(0, 200) + "...");
    
    // Create HTTPS client
    httplib::Client cli("https://api.anthropic.com");
    cli.set_connection_timeout(30);
    cli.set_read_timeout(60);
    
    // Set headers
    httplib::Headers headers = {
        {"Content-Type", "application/json"},
        {"x-api-key", m_apiKey},
        {"anthropic-version", "2023-06-01"}
    };
    
    // Make POST request
    auto res = cli.Post(endpoint.c_str(), headers, requestBody, "application/json");
    
    if (!res) {
        LOG_ERROR("[Anthropic] HTTP request failed: " + httplib::to_string(res.error()));
        nlohmann::json error = {
            {"error", {
                {"message", "HTTP request failed: " + httplib::to_string(res.error())}
            }}
        };
        return error.dump();
    }
    
    LOG_DEBUG("[Anthropic] Response status: " + std::to_string(res->status));
    
    if (res->status != 200) {
        LOG_ERROR("[Anthropic] API returned status " + std::to_string(res->status) + ": " + res->body);
    }
    
    return res->body;
}

} // namespace ai
