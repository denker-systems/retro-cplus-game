/**
 * @file AIAgentSystem.cpp
 * @brief Implementation of AIAgentSystem
 */
#include "AIAgentSystem.h"
#include "EditorToolRegistry.h"
#include "editor/core/CommandManager.h"
#include "engine/utils/Logger.h"
#include <sstream>

namespace ai {

// Default system prompt
static const char* DEFAULT_SYSTEM_PROMPT = R"(
You are an AI assistant for the Retro Engine Editor, a 2D adventure game engine inspired by LucasArts classics.

You help the user to:
- Create and modify scenes (rooms)
- Place and configure hotspots and interactive objects
- Create NPCs and dialog trees
- Design quests and objectives
- Generate level layouts
- Manage actors and components

Available tools allow you to:
- List, create, modify, and delete scenes
- List, create, modify, and delete actors
- Add components to actors

Rules:
1. Always use tools to perform changes - never just describe what could be done
2. Ask for clarification if you're unsure about the user's intention
3. Confirm destructive operations (delete) before executing them
4. All changes can be undone with Ctrl+Z
5. Respond in English
6. Be concise and helpful

Current context:
- Scene: {current_room}
- Level: {current_level}
- Selected actor: {selected_actor}
)";

AIAgentSystem& AIAgentSystem::instance() {
    static AIAgentSystem instance;
    return instance;
}

bool AIAgentSystem::initialize() {
    if (m_initialized) {
        LOG_DEBUG("[AI] AIAgentSystem already initialized");
        return true;
    }
    
    LOG_DEBUG("[AI] Setting default agent config");
    // Set default config
    m_config.systemPrompt = DEFAULT_SYSTEM_PROMPT;
    m_config.maxToolCalls = 10;
    m_config.requireConfirmation = true;
    m_config.streamResponses = false;
    
    m_initialized = true;
    LOG_DEBUG("[AI] AIAgentSystem state: initialized=true, provider=" + 
              std::string(m_provider ? "set" : "null"));
    return true;
}

void AIAgentSystem::shutdown() {
    if (!m_initialized) {
        LOG_DEBUG("[AI] AIAgentSystem not initialized, skip shutdown");
        return;
    }
    
    LOG_DEBUG("[AI] Shutting down AIAgentSystem...");
    clearHistory();
    m_provider.reset();
    m_callback = nullptr;
    m_initialized = false;
    LOG_DEBUG("[AI] AIAgentSystem shutdown complete");
}

void AIAgentSystem::setProvider(std::unique_ptr<ILLMProvider> provider) {
    if (provider) {
        LOG_INFO("[AI] LLM Provider set: " + std::string(provider->getName()));
    } else {
        LOG_WARNING("[AI] LLM Provider cleared");
    }
    m_provider = std::move(provider);
}

void AIAgentSystem::setCallback(AgentCallback callback) {
    m_callback = std::move(callback);
}

void AIAgentSystem::setConfig(const AgentConfig& config) {
    m_config = config;
}

void AIAgentSystem::updateContext(const EditorContext& context) {
    m_context = context;
}

bool AIAgentSystem::processUserMessage(const std::string& message) {
    if (!isReady()) {
        m_lastError = "Agent not ready. Check provider configuration.";
        notify(m_lastError, true);
        return false;
    }
    
    if (m_state == AgentState::Processing) {
        m_lastError = "Already processing a request.";
        notify(m_lastError, true);
        return false;
    }
    
    m_state = AgentState::Processing;
    LOG_INFO("[AI] Starting async processing for message: " + message.substr(0, 50));
    
    // Add user message to history
    m_history.push_back(Message::user(message));
    
    // Build messages for API call
    m_pendingMessages.clear();
    m_pendingMessages.push_back(Message::system(buildSystemPrompt()));
    
    // Add conversation history
    for (const auto& msg : m_history) {
        m_pendingMessages.push_back(msg);
    }
    
    // Get tool definitions
    m_pendingTools = EditorToolRegistry::instance().getToolDefinitions();
    
    // Start async API call
    auto* provider = m_provider.get();
    auto messages = m_pendingMessages;
    auto tools = m_pendingTools;
    auto config = m_config.llmConfig;
    
    m_asyncFuture = std::async(std::launch::async, [provider, messages, tools, config]() {
        return provider->chat(messages, tools, config);
    });
    
    return true;
}

void AIAgentSystem::update() {
    // Check if async operation is complete
    if (m_state != AgentState::Processing) {
        return;
    }
    
    if (!m_asyncFuture.valid()) {
        return;
    }
    
    // Check if future is ready (non-blocking)
    auto status = m_asyncFuture.wait_for(std::chrono::milliseconds(0));
    if (status != std::future_status::ready) {
        return;  // Still processing
    }
    
    // Get the result
    try {
        LLMResponse response = m_asyncFuture.get();
        handleAsyncResponse(response);
    } catch (const std::exception& e) {
        m_state = AgentState::Error;
        m_lastError = std::string("Async error: ") + e.what();
        notify("Error: " + m_lastError, true);
        LOG_ERROR("[AI] Async exception: " + m_lastError);
    }
}

void AIAgentSystem::handleAsyncResponse(const LLMResponse& response) {
    LOG_DEBUG("[AI] Handling async response");
    
    if (!response.success) {
        m_state = AgentState::Error;
        m_lastError = response.error;
        notify("Error: " + response.error, true);
        return;
    }
    
    // Handle response
    if (!response.toolCalls.empty()) {
        // Check if confirmation needed
        bool needsConfirmation = false;
        if (m_config.requireConfirmation) {
            for (const auto& call : response.toolCalls) {
                auto* tool = EditorToolRegistry::instance().getTool(call.name);
                if (tool && tool->requiresConfirmation()) {
                    needsConfirmation = true;
                    break;
                }
            }
        }
        
        if (needsConfirmation) {
            m_pendingToolCalls = response.toolCalls;
            m_state = AgentState::WaitingForConfirmation;
            
            // Notify about pending actions
            std::stringstream ss;
            ss << "I want to perform the following:\n";
            for (const auto& call : response.toolCalls) {
                ss << "- " << call.name << "\n";
            }
            ss << "\nConfirm with 'OK' or cancel.";
            notify(ss.str());
        } else {
            // Execute immediately
            executeToolCalls(response.toolCalls);
        }
    }
    
    // Add assistant response to history
    if (!response.content.empty()) {
        m_history.push_back(Message::assistant(response.content));
        notify(response.content);
    }
    
    if (m_state == AgentState::Processing) {
        m_state = AgentState::Idle;
    }
    
    LOG_INFO("[AI] Async processing complete");
}

void AIAgentSystem::confirmAction() {
    if (m_state != AgentState::WaitingForConfirmation) {
        return;
    }
    
    executeToolCalls(m_pendingToolCalls);
    m_pendingToolCalls.clear();
    m_state = AgentState::Idle;
}

void AIAgentSystem::cancelAction() {
    if (m_state != AgentState::WaitingForConfirmation) {
        return;
    }
    
    m_pendingToolCalls.clear();
    m_state = AgentState::Idle;
    notify("Åtgärd avbruten.");
}

void AIAgentSystem::clearHistory() {
    m_history.clear();
}

bool AIAgentSystem::isReady() const {
    return m_initialized && m_provider && m_provider->isAvailable();
}

std::string AIAgentSystem::buildSystemPrompt() const {
    std::string prompt = m_config.systemPrompt;
    
    // Replace placeholders
    auto replace = [&prompt](const std::string& key, const std::string& value) {
        size_t pos = 0;
        while ((pos = prompt.find(key, pos)) != std::string::npos) {
            prompt.replace(pos, key.length(), value);
            pos += value.length();
        }
    };
    
    replace("{current_room}", m_context.currentRoom.empty() ? "(inget)" : m_context.currentRoom);
    replace("{current_level}", m_context.currentLevel.empty() ? "(inget)" : m_context.currentLevel);
    replace("{selected_actor}", m_context.selectedActor.empty() ? "(inget)" : m_context.selectedActor);
    
    return prompt;
}

void AIAgentSystem::executeToolCalls(const std::vector<ToolCall>& toolCalls) {
    for (const auto& call : toolCalls) {
        ToolResult result = executeToolCall(call);
        
        if (result.success) {
            notify("✓ " + call.name + ": " + result.message);
            
            // Execute command for undo/redo
            if (result.command) {
                CommandManager::instance().executeCommand(std::move(result.command));
            }
        } else {
            notify("✗ " + call.name + ": " + result.message, true);
        }
        
        // Add tool result to history
        nlohmann::json resultJson = {
            {"success", result.success},
            {"message", result.message}
        };
        if (!result.data.empty()) {
            resultJson["data"] = result.data;
        }
        m_history.push_back(Message::toolResult(call.id, resultJson.dump()));
    }
}

ToolResult AIAgentSystem::executeToolCall(const ToolCall& toolCall) {
    LOG_DEBUG("[AI] Executing tool: " + toolCall.name);
    auto* tool = EditorToolRegistry::instance().getTool(toolCall.name);
    
    if (!tool) {
        LOG_ERROR("[AI] Tool not found: " + toolCall.name);
        return ToolResult::error("Tool not found: " + toolCall.name);
    }
    
    try {
        auto result = tool->execute(toolCall.arguments);
        LOG_DEBUG("[AI] Tool " + toolCall.name + " result: " + 
                  (result.success ? "success" : "failed"));
        return result;
    } catch (const std::exception& e) {
        LOG_ERROR("[AI] Tool " + toolCall.name + " threw exception: " + e.what());
        return ToolResult::error(std::string("Exception: ") + e.what());
    }
}

void AIAgentSystem::notify(const std::string& message, bool isError) {
    if (m_callback) {
        m_callback(message, isError);
    }
}

} // namespace ai
