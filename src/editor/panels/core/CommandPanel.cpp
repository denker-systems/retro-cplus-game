/**
 * @file CommandPanel.cpp
 * @brief Command panel implementation
 */
#include "CommandPanel.h"
#include "editor/core/EditorContext.h"
#include "ai/core/EditorToolRegistry.h"
#include "ai/core/IEditorTool.h"
#include "engine/utils/Logger.h"
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

CommandPanel::CommandPanel(EditorContext& context)
    : m_context(context) {
    addOutput("Command panel ready. Type 'help' for available commands.", false);
}

void CommandPanel::clearHistory() {
    m_history.clear();
    m_commandHistory.clear();
    m_historyIndex = -1;
}

void CommandPanel::addOutput(const std::string& text, bool isError) {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    
    CommandHistoryEntry entry;
    entry.command = "";
    entry.output = text;
    entry.success = !isError;
    entry.timestamp = oss.str();
    
    m_history.push_back(entry);
    
    while (m_history.size() > MAX_HISTORY) {
        m_history.pop_front();
    }
}

std::vector<std::string> CommandPanel::getAvailableCommands() const {
    std::vector<std::string> commands;
    commands.push_back("help");
    commands.push_back("clear");
    
    // Add all registered tools
    auto& registry = ai::EditorToolRegistry::instance();
    for (const auto& tool : registry.getAllTools()) {
        commands.push_back(tool->getName());
    }
    
    return commands;
}

std::vector<std::string> CommandPanel::parseCommand(const std::string& input) {
    std::vector<std::string> tokens;
    std::string current;
    bool inQuotes = false;
    
    for (char c : input) {
        if (c == '"') {
            inQuotes = !inQuotes;
        } else if (c == ' ' && !inQuotes) {
            if (!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        tokens.push_back(current);
    }
    
    return tokens;
}

CommandResult CommandPanel::executeCommand(const std::string& command) {
    auto tokens = parseCommand(command);
    
    if (tokens.empty()) {
        return CommandResult::error("No command specified");
    }
    
    std::string cmd = tokens[0];
    
    // Built-in commands
    if (cmd == "help") {
        std::string helpText = "Available commands:\n";
        helpText += "  help          - Show this help\n";
        helpText += "  clear         - Clear command history\n";
        helpText += "\nAI Tools:\n";
        
        auto& registry = ai::EditorToolRegistry::instance();
        for (const auto& tool : registry.getAllTools()) {
            helpText += "  " + std::string(tool->getName());
            helpText += " - " + std::string(tool->getDescription()) + "\n";
        }
        
        return CommandResult::ok(helpText);
    }
    
    if (cmd == "clear") {
        clearHistory();
        return CommandResult::ok("History cleared");
    }
    
    // Try to execute as AI tool
    auto& registry = ai::EditorToolRegistry::instance();
    ai::IEditorTool* tool = registry.getTool(cmd);
    
    if (!tool) {
        return CommandResult::error("Unknown command: " + cmd);
    }
    
    // Build parameters JSON from remaining tokens
    nlohmann::json params = nlohmann::json::object();
    
    // Simple key=value parsing for remaining tokens
    for (size_t i = 1; i < tokens.size(); i++) {
        const std::string& token = tokens[i];
        size_t eqPos = token.find('=');
        if (eqPos != std::string::npos) {
            std::string key = token.substr(0, eqPos);
            std::string value = token.substr(eqPos + 1);
            
            // Try to parse as number
            try {
                if (value.find('.') != std::string::npos) {
                    params[key] = std::stod(value);
                } else {
                    params[key] = std::stoi(value);
                }
            } catch (...) {
                // Not a number, use as string
                params[key] = value;
            }
        }
    }
    
    LOG_INFO("[Command] Executing: " + cmd + " with params: " + params.dump());
    
    // Execute the tool
    auto result = tool->execute(params);
    
    if (result.success) {
        std::string output = result.message;
        if (!result.data.empty()) {
            output += "\n" + result.data.dump(2);
        }
        return CommandResult::ok(output);
    } else {
        return CommandResult::error(result.message);
    }
}

void CommandPanel::updateAutocomplete() {
    m_suggestions.clear();
    m_selectedSuggestion = -1;
    
    std::string input(m_inputBuffer);
    if (input.empty()) {
        m_showSuggestions = false;
        return;
    }
    
    // Get matching commands
    auto commands = getAvailableCommands();
    for (const auto& cmd : commands) {
        if (cmd.find(input) == 0) {
            m_suggestions.push_back(cmd);
        }
    }
    
    m_showSuggestions = !m_suggestions.empty();
}

void CommandPanel::processInput() {
    std::string command(m_inputBuffer);
    
    if (command.empty()) return;
    
    // Add to command history
    m_commandHistory.push_back(command);
    m_historyIndex = -1;
    
    // Get timestamp
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M:%S");
    
    // Execute command
    auto result = executeCommand(command);
    
    // Add to history
    CommandHistoryEntry entry;
    entry.command = command;
    entry.output = result.output;
    entry.success = result.success;
    entry.timestamp = oss.str();
    
    m_history.push_back(entry);
    
    while (m_history.size() > MAX_HISTORY) {
        m_history.pop_front();
    }
    
    // Clear input
    m_inputBuffer[0] = '\0';
    m_showSuggestions = false;
}

void CommandPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Toolbar
        if (ImGui::Button("Clear")) {
            clearHistory();
        }
        ImGui::SameLine();
        ImGui::Checkbox("Auto-scroll", &m_autoScroll);
        ImGui::SameLine();
        ImGui::TextDisabled("(?) Type 'help' for commands");
        
        ImGui::Separator();
        
        // History area
        float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
        if (ImGui::BeginChild("CommandHistory", ImVec2(0, -footerHeight), false, ImGuiWindowFlags_HorizontalScrollbar)) {
            for (const auto& entry : m_history) {
                if (!entry.command.empty()) {
                    // Show command
                    ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "[%s]", entry.timestamp.c_str());
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "> %s", entry.command.c_str());
                }
                
                // Show output
                if (!entry.output.empty()) {
                    ImVec4 color = entry.success ? ImVec4(0.8f, 0.8f, 0.8f, 1.0f) : ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    
                    // Multi-line output
                    std::istringstream stream(entry.output);
                    std::string line;
                    while (std::getline(stream, line)) {
                        ImGui::TextColored(color, "  %s", line.c_str());
                    }
                }
            }
            
            if (m_autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
                ImGui::SetScrollHereY(1.0f);
            }
        }
        ImGui::EndChild();
        
        ImGui::Separator();
        
        // Input field
        ImGui::PushItemWidth(-1);
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory | ImGuiInputTextFlags_CallbackCompletion;
        
        bool enterPressed = ImGui::InputText("##CommandInput", m_inputBuffer, sizeof(m_inputBuffer), flags,
            [](ImGuiInputTextCallbackData* data) -> int {
                CommandPanel* panel = static_cast<CommandPanel*>(data->UserData);
                
                if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
                    // Up/Down history navigation
                    if (data->EventKey == ImGuiKey_UpArrow) {
                        if (!panel->m_commandHistory.empty()) {
                            if (panel->m_historyIndex < 0) {
                                panel->m_historyIndex = (int)panel->m_commandHistory.size() - 1;
                            } else if (panel->m_historyIndex > 0) {
                                panel->m_historyIndex--;
                            }
                            data->DeleteChars(0, data->BufTextLen);
                            data->InsertChars(0, panel->m_commandHistory[panel->m_historyIndex].c_str());
                        }
                    } else if (data->EventKey == ImGuiKey_DownArrow) {
                        if (panel->m_historyIndex >= 0) {
                            panel->m_historyIndex++;
                            if (panel->m_historyIndex >= (int)panel->m_commandHistory.size()) {
                                panel->m_historyIndex = -1;
                                data->DeleteChars(0, data->BufTextLen);
                            } else {
                                data->DeleteChars(0, data->BufTextLen);
                                data->InsertChars(0, panel->m_commandHistory[panel->m_historyIndex].c_str());
                            }
                        }
                    }
                } else if (data->EventFlag == ImGuiInputTextFlags_CallbackCompletion) {
                    // Tab completion
                    panel->updateAutocomplete();
                    if (!panel->m_suggestions.empty()) {
                        data->DeleteChars(0, data->BufTextLen);
                        data->InsertChars(0, panel->m_suggestions[0].c_str());
                    }
                }
                
                return 0;
            }, this);
        
        ImGui::PopItemWidth();
        
        // Set focus on first render
        if (m_focusInput) {
            ImGui::SetKeyboardFocusHere(-1);
            m_focusInput = false;
        }
        
        if (enterPressed) {
            processInput();
            ImGui::SetKeyboardFocusHere(-1);
        }
        
        // Show autocomplete suggestions
        if (m_showSuggestions && !m_suggestions.empty()) {
            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
            ImGui::BeginTooltip();
            for (size_t i = 0; i < m_suggestions.size() && i < 10; i++) {
                ImGui::Text("%s", m_suggestions[i].c_str());
            }
            ImGui::EndTooltip();
        }
    }
    ImGui::End();
#endif
}
