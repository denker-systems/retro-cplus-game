/**
 * @file AIChatPanel.cpp
 * @brief Implementation of AI chat panel
 */
#include "AIChatPanel.h"
#include "ai/core/AIAgentSystem.h"
#include "ai/core/EditorToolRegistry.h"
#include "ai/providers/OpenAIProvider.h"
#include "engine/utils/Logger.h"

#include <imgui.h>
#include <ctime>
#include <iomanip>
#include <sstream>

namespace ai {

AIChatPanel& AIChatPanel::instance() {
    static AIChatPanel instance;
    return instance;
}

void AIChatPanel::initialize() {
    if (m_initialized) return;
    
    // Set up agent callback
    AIAgentSystem::instance().setCallback(
        [this](const std::string& msg, bool isError) {
            onAgentMessage(msg, isError);
        }
    );
    
    // Add welcome message
    ChatMessage welcome;
    welcome.type = ChatMessage::Type::System;
    welcome.content = "Welcome to Retro Engine AI Assistant!\n"
                      "I can help you create rooms, hotspots, NPCs and more.\n"
                      "Start by asking a question or giving an instruction.";
    welcome.timestamp = getTimestamp();
    m_messages.push_back(welcome);
    
    m_initialized = true;
}

void AIChatPanel::shutdown() {
    if (!m_initialized) return;
    
    clearMessages();
    m_initialized = false;
}

void AIChatPanel::render() {
    if (!m_visible) return;
    
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin("AI Assistant", &m_visible, ImGuiWindowFlags_MenuBar)) {
        // Menu bar
        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("Options")) {
                if (ImGui::MenuItem("Settings...")) {
                    m_showSettings = true;
                }
                if (ImGui::MenuItem("Clear History")) {
                    clearMessages();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        
        // Status indicator
        auto state = AIAgentSystem::instance().getState();
        const char* stateStr = "Ready";
        ImVec4 stateColor = ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
        
        switch (state) {
            case AgentState::Processing:
                stateStr = "Processing...";
                stateColor = ImVec4(0.8f, 0.8f, 0.2f, 1.0f);
                break;
            case AgentState::WaitingForConfirmation:
                stateStr = "Waiting for confirmation";
                stateColor = ImVec4(0.2f, 0.6f, 0.8f, 1.0f);
                break;
            case AgentState::Error:
                stateStr = "Error";
                stateColor = ImVec4(0.8f, 0.2f, 0.2f, 1.0f);
                break;
            default:
                break;
        }
        
        ImGui::TextColored(stateColor, "● %s", stateStr);
        
        if (!AIAgentSystem::instance().isReady()) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), 
                "(API key required)");
        }
        
        ImGui::Separator();
        
        // Messages area
        renderMessages();
        
        ImGui::Separator();
        
        // Input area
        renderInput();
        
        // Confirmation dialog
        renderConfirmationDialog();
        
        // Settings popup
        renderSettings();
    }
    ImGui::End();
}

void AIChatPanel::renderMessages() {
    // Calculate available height (leave space for input)
    float footerHeight = ImGui::GetFrameHeightWithSpacing() + 8;
    
    ImGui::BeginChild("Messages", ImVec2(0, -footerHeight), true);
    
    for (const auto& msg : m_messages) {
        // Message styling based on type
        ImVec4 color;
        const char* prefix = "";
        
        switch (msg.type) {
            case ChatMessage::Type::User:
                color = ImVec4(0.4f, 0.7f, 1.0f, 1.0f);
                prefix = "Du: ";
                break;
            case ChatMessage::Type::Assistant:
                color = ImVec4(0.5f, 0.9f, 0.5f, 1.0f);
                prefix = "AI: ";
                break;
            case ChatMessage::Type::ToolCall:
                color = ImVec4(0.8f, 0.6f, 1.0f, 1.0f);
                prefix = "";
                break;
            case ChatMessage::Type::Error:
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                prefix = "Fel: ";
                break;
            case ChatMessage::Type::System:
                color = ImVec4(0.7f, 0.7f, 0.7f, 1.0f);
                prefix = "";
                break;
        }
        
        // Timestamp
        ImGui::TextDisabled("[%s]", msg.timestamp.c_str());
        ImGui::SameLine();
        
        // Message content
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        ImGui::TextWrapped("%s%s", prefix, msg.content.c_str());
        ImGui::PopStyleColor();
        
        ImGui::Spacing();
    }
    
    // Show streaming content if AI is processing
    if (AIAgentSystem::instance().isStreaming()) {
        std::string streamContent = AIAgentSystem::instance().getStreamingContent();
        if (!streamContent.empty()) {
            // Streaming message styling
            ImGui::TextDisabled("[...]");
            ImGui::SameLine();
            
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.9f, 0.5f, 1.0f));
            ImGui::TextWrapped("AI: %s▌", streamContent.c_str());  // Blinking cursor
            ImGui::PopStyleColor();
            
            ImGui::Spacing();
            
            // Auto-scroll during streaming
            ImGui::SetScrollHereY(1.0f);
        } else {
            // Show typing indicator
            ImGui::TextDisabled("[...]");
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.9f, 0.5f, 0.6f));
            ImGui::Text("AI: Thinking...");
            ImGui::PopStyleColor();
        }
    }
    
    // Auto-scroll
    if (m_scrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        m_scrollToBottom = false;
    }
    
    ImGui::EndChild();
}

void AIChatPanel::renderInput() {
    auto state = AIAgentSystem::instance().getState();
    // Allow input even without provider - local tools still work
    bool canSend = state == AgentState::Idle;
    
    // Confirmation buttons if waiting
    if (state == AgentState::WaitingForConfirmation) {
        if (ImGui::Button("Confirm", ImVec2(100, 0))) {
            AIAgentSystem::instance().confirmAction();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(100, 0))) {
            AIAgentSystem::instance().cancelAction();
        }
        return;
    }
    
    // Input field
    ImGui::PushItemWidth(-80);
    
    ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue;
    if (!canSend) {
        flags |= ImGuiInputTextFlags_ReadOnly;
    }
    
    bool submitted = ImGui::InputText("##input", m_inputBuffer, 
                                       sizeof(m_inputBuffer), flags);
    
    ImGui::PopItemWidth();
    
    ImGui::SameLine();
    
    // Send button
    if (!canSend) {
        ImGui::BeginDisabled();
    }
    
    if (ImGui::Button("Send", ImVec2(70, 0)) || submitted) {
        sendMessage();
    }
    
    if (!canSend) {
        ImGui::EndDisabled();
    }
}

void AIChatPanel::renderSettings() {
    if (!m_showSettings) return;
    
    ImGui::OpenPopup("AI Inställningar");
    
    if (ImGui::BeginPopupModal("AI Inställningar", &m_showSettings, 
                                ImGuiWindowFlags_AlwaysAutoResize)) {
        // API Key
        ImGui::Text("OpenAI API-nyckel:");
        static char apiKeyBuffer[256] = {0};
        ImGui::InputText("##apikey", apiKeyBuffer, sizeof(apiKeyBuffer), 
                        ImGuiInputTextFlags_Password);
        
        if (ImGui::Button("Spara")) {
            m_apiKey = apiKeyBuffer;
            
            // Create and configure provider
            auto provider = std::make_unique<OpenAIProvider>();
            provider->setApiKey(m_apiKey);
            AIAgentSystem::instance().setProvider(std::move(provider));
            
            m_showSettings = false;
            
            // Add confirmation message
            ChatMessage msg;
            msg.type = ChatMessage::Type::System;
            msg.content = "API key saved. AI assistant is now ready.";
            msg.timestamp = getTimestamp();
            addMessage(msg);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel")) {
            m_showSettings = false;
        }
        
        ImGui::Separator();
        
        // Model selection
        ImGui::Text("Model:");
        const char* models[] = {"gpt-4o", "gpt-4o-mini", "gpt-4-turbo", "gpt-3.5-turbo"};
        static int currentModel = 0;
        ImGui::Combo("##model", &currentModel, models, IM_ARRAYSIZE(models));
        m_selectedModel = models[currentModel];
        
        // Tool count
        ImGui::Text("Registered tools: %zu", 
                   EditorToolRegistry::instance().getToolCount());
        
        ImGui::EndPopup();
    }
}

void AIChatPanel::renderConfirmationDialog() {
    // Handled in renderInput() for simplicity
}

void AIChatPanel::onAgentMessage(const std::string& message, bool isError) {
    ChatMessage msg;
    
    // Check if it's a tool call result
    if (message.find("✓") == 0 || message.find("✗") == 0) {
        msg.type = ChatMessage::Type::ToolCall;
    } else if (isError) {
        msg.type = ChatMessage::Type::Error;
    } else {
        msg.type = ChatMessage::Type::Assistant;
    }
    
    msg.content = message;
    msg.timestamp = getTimestamp();
    
    addMessage(msg);
}

std::string AIChatPanel::getTimestamp() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M");
    return oss.str();
}

void AIChatPanel::sendMessage() {
    std::string message = m_inputBuffer;
    
    if (message.empty()) {
        LOG_DEBUG("[AI Chat] Empty message, ignoring");
        return;
    }
    
    LOG_INFO("[AI Chat] User message: " + message);
    
    // Add user message to display
    ChatMessage userMsg;
    userMsg.type = ChatMessage::Type::User;
    userMsg.content = message;
    userMsg.timestamp = getTimestamp();
    addMessage(userMsg);
    
    // Clear input
    m_inputBuffer[0] = '\0';
    
    // Check agent state before sending
    auto& agent = AIAgentSystem::instance();
    LOG_DEBUG("[AI Chat] Agent state - initialized: " + std::string(agent.isReady() ? "yes" : "no"));
    LOG_DEBUG("[AI Chat] Agent state - state: " + std::to_string(static_cast<int>(agent.getState())));
    
    // Send to agent
    LOG_INFO("[AI Chat] Sending to AIAgentSystem...");
    bool result = agent.processUserMessage(message);
    LOG_INFO("[AI Chat] processUserMessage returned: " + std::string(result ? "success" : "failed"));
}

void AIChatPanel::addMessage(const ChatMessage& message) {
    m_messages.push_back(message);
    
    // Trim old messages
    while (m_messages.size() > MAX_MESSAGES) {
        m_messages.pop_front();
    }
    
    m_scrollToBottom = true;
}

void AIChatPanel::clearMessages() {
    m_messages.clear();
}

} // namespace ai
