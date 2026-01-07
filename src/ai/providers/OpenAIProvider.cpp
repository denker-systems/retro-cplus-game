/**
 * @file OpenAIProvider.cpp
 * @brief Implementation of OpenAI API provider
 */
#include "OpenAIProvider.h"
#include <stdexcept>

// Note: For actual HTTP requests, we'll need httplib or similar
// This is a placeholder implementation that shows the API structure

namespace ai {

OpenAIProvider::OpenAIProvider() = default;

bool OpenAIProvider::isAvailable() const {
    return !m_apiKey.empty();
}

void OpenAIProvider::setApiKey(const std::string& apiKey) {
    m_apiKey = apiKey;
}

std::vector<std::string> OpenAIProvider::getAvailableModels() const {
    return {
        "gpt-4o",
        "gpt-4o-mini", 
        "gpt-4-turbo",
        "gpt-4",
        "gpt-3.5-turbo"
    };
}

nlohmann::json OpenAIProvider::messageToJson(const Message& msg) const {
    nlohmann::json j;
    
    switch (msg.role) {
        case MessageRole::System:
            j["role"] = "system";
            break;
        case MessageRole::User:
            j["role"] = "user";
            break;
        case MessageRole::Assistant:
            j["role"] = "assistant";
            break;
        case MessageRole::Tool:
            j["role"] = "tool";
            j["tool_call_id"] = msg.toolCallId;
            break;
    }
    
    j["content"] = msg.content;
    return j;
}

nlohmann::json OpenAIProvider::toolToJson(const ToolDefinition& tool) const {
    return {
        {"type", "function"},
        {"function", {
            {"name", tool.name},
            {"description", tool.description},
            {"parameters", tool.parameters}
        }}
    };
}

LLMResponse OpenAIProvider::parseResponse(const nlohmann::json& json) const {
    LLMResponse response;
    
    try {
        if (json.contains("error")) {
            response.success = false;
            response.error = json["error"]["message"].get<std::string>();
            return response;
        }
        
        const auto& choice = json["choices"][0];
        const auto& message = choice["message"];
        
        // Get content
        if (message.contains("content") && !message["content"].is_null()) {
            response.content = message["content"].get<std::string>();
        }
        
        // Get tool calls
        if (message.contains("tool_calls")) {
            for (const auto& tc : message["tool_calls"]) {
                ToolCall call;
                call.id = tc["id"].get<std::string>();
                call.name = tc["function"]["name"].get<std::string>();
                call.arguments = nlohmann::json::parse(
                    tc["function"]["arguments"].get<std::string>()
                );
                response.toolCalls.push_back(std::move(call));
            }
        }
        
        // Get token usage
        if (json.contains("usage")) {
            response.tokensUsed = json["usage"]["total_tokens"].get<int>();
        }
        
        response.success = true;
    } catch (const std::exception& e) {
        response.success = false;
        response.error = std::string("Parse error: ") + e.what();
    }
    
    return response;
}

LLMResponse OpenAIProvider::chat(
    const std::vector<Message>& messages,
    const std::vector<ToolDefinition>& tools,
    const LLMConfig& config
) {
    if (!isAvailable()) {
        LLMResponse errResp;
        errResp.success = false;
        errResp.error = "API key not set";
        return errResp;
    }
    
    // Build request body
    nlohmann::json body;
    body["model"] = config.model;
    body["temperature"] = config.temperature;
    body["max_tokens"] = config.maxTokens;
    
    // Add messages
    nlohmann::json messagesJson = nlohmann::json::array();
    for (const auto& msg : messages) {
        messagesJson.push_back(messageToJson(msg));
    }
    body["messages"] = messagesJson;
    
    // Add tools
    if (!tools.empty()) {
        nlohmann::json toolsJson = nlohmann::json::array();
        for (const auto& tool : tools) {
            toolsJson.push_back(toolToJson(tool));
        }
        body["tools"] = toolsJson;
        body["tool_choice"] = "auto";
    }
    
    // Make request
    try {
        std::string responseStr = httpPost("/v1/chat/completions", body);
        nlohmann::json responseJson = nlohmann::json::parse(responseStr);
        return parseResponse(responseJson);
    } catch (const std::exception& e) {
        LLMResponse errResp;
        errResp.success = false;
        errResp.error = std::string("Request failed: ") + e.what();
        return errResp;
    }
}

LLMResponse OpenAIProvider::chatStream(
    const std::vector<Message>& messages,
    const std::vector<ToolDefinition>& tools,
    StreamCallback callback,
    const LLMConfig& config
) {
    // Streaming implementation would go here
    // For now, fall back to non-streaming
    (void)callback;
    return chat(messages, tools, config);
}

std::string OpenAIProvider::httpPost(const std::string& endpoint, const nlohmann::json& body) const {
    // TODO: Implement actual HTTP request using httplib or WinHTTP
    // This is a placeholder that returns an error
    // In production, this would make a real HTTP request
    
    (void)endpoint;
    (void)body;
    
    // For now, return a mock error to indicate implementation needed
    nlohmann::json error = {
        {"error", {
            {"message", "HTTP client not implemented. Add httplib or WinHTTP dependency."}
        }}
    };
    return error.dump();
}

} // namespace ai
