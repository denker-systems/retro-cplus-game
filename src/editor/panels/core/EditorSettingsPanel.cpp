/**
 * @file EditorSettingsPanel.cpp
 * @brief Implementation of EditorSettingsPanel
 */
#include "EditorSettingsPanel.h"
#include "ai/core/AIAgentSystem.h"
#include "ai/core/EditorToolRegistry.h"
#include "ai/providers/OpenAIProvider.h"
#include "ai/providers/AnthropicProvider.h"
#include "engine/utils/Logger.h"

#include <imgui.h>
#include <nlohmann/json.hpp>
#include <fstream>

namespace editor {

EditorSettingsPanel::EditorSettingsPanel() {
    loadSettings();
}

void EditorSettingsPanel::render() {
    if (!m_isOpen) return;
    
    ImGui::OpenPopup("Editor Settings###EditorSettingsModal");
    
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    
    if (ImGui::BeginPopupModal("Editor Settings###EditorSettingsModal", &m_isOpen, 
                                ImGuiWindowFlags_NoResize)) {
        // Tab bar
        if (ImGui::BeginTabBar("SettingsTabs")) {
            if (ImGui::BeginTabItem("AI Assistant")) {
                m_currentTab = 0;
                renderAISettings();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Editor")) {
                m_currentTab = 1;
                renderEditorSettings();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Performance")) {
                m_currentTab = 2;
                renderPerformanceSettings();
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("About")) {
                m_currentTab = 3;
                renderAbout();
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }
        
        ImGui::Separator();
        
        // Bottom buttons
        if (ImGui::Button("Save", ImVec2(80, 0))) {
            saveSettings();
            m_isOpen = false;
            if (onSettingsChanged) onSettingsChanged();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(80, 0))) {
            loadSettings();  // Revert changes
            m_isOpen = false;
        }
        
        ImGui::EndPopup();
    }
}

void EditorSettingsPanel::renderAISettings() {
    ImGui::TextWrapped("Configure AI Assistant providers and API keys.");
    ImGui::Spacing();
    
    // Provider selection
    ImGui::Text("Provider:");
    const char* providers[] = {"OpenAI", "Anthropic", "Local (Ollama)"};
    if (ImGui::Combo("##provider", &m_selectedProvider, providers, IM_ARRAYSIZE(providers))) {
        m_aiSettingsChanged = true;
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Provider-specific settings
    if (m_selectedProvider == 0) {
        // OpenAI
        ImGui::Text("OpenAI API Key:");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##openai_key", m_openaiApiKey, sizeof(m_openaiApiKey), 
                            ImGuiInputTextFlags_Password)) {
            m_aiSettingsChanged = true;
        }
        
        ImGui::Spacing();
        
        ImGui::Text("Model:");
        const char* openaiModels[] = {
            "gpt-4.1",           // Smartest non-reasoning
            "gpt-4.1-mini",      // Faster version
            "gpt-4.1-nano",      // Fastest, cheapest
            "gpt-4o",            // Multimodal
            "gpt-4o-mini",       // Smaller multimodal
            "o4-mini",           // Reasoning model
            "o3",                // Reasoning model
            "o1-pro"             // More compute
        };
        if (ImGui::Combo("##openai_model", &m_selectedModel, openaiModels, IM_ARRAYSIZE(openaiModels))) {
            m_aiSettingsChanged = true;
        }
        
        ImGui::Spacing();
        
        // Help text
        ImGui::TextDisabled("Get your API key from: https://platform.openai.com/api-keys");
        
    } else if (m_selectedProvider == 1) {
        // Anthropic
        ImGui::Text("Anthropic API Key:");
        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##anthropic_key", m_anthropicApiKey, sizeof(m_anthropicApiKey), 
                            ImGuiInputTextFlags_Password)) {
            m_aiSettingsChanged = true;
        }
        
        ImGui::Spacing();
        
        ImGui::Text("Model:");
        const char* anthropicModels[] = {
            "claude-sonnet-4-5",           // Claude Sonnet 4.5 (recommended)
            "claude-sonnet-4-0",           // Claude Sonnet 4
            "claude-3-7-sonnet-latest",    // Claude Sonnet 3.7
            "claude-3-5-haiku-latest"      // Claude Haiku 3.5 (fast)
        };
        if (ImGui::Combo("##anthropic_model", &m_selectedModel, anthropicModels, IM_ARRAYSIZE(anthropicModels))) {
            m_aiSettingsChanged = true;
        }
        
        ImGui::Spacing();
        ImGui::TextDisabled("Get your API key from: https://console.anthropic.com/");
        
    } else if (m_selectedProvider == 2) {
        // Local/Ollama
        ImGui::TextWrapped("Local LLM via Ollama. Make sure Ollama is running on localhost:11434.");
        
        ImGui::Spacing();
        
        ImGui::Text("Model:");
        const char* localModels[] = {
            "llama4",            // Latest Llama
            "llama3.3",          // Stable Llama 3.3
            "qwen3",             // Alibaba Qwen 3
            "mistral-large-3",   // Powerful MoE
            "deepseek-r1",       // Reasoning focused
            "phi4",              // Microsoft efficient
            "gemma3",            // Google open model
            "codestral"          // Coding focused
        };
        if (ImGui::Combo("##local_model", &m_selectedModel, localModels, IM_ARRAYSIZE(localModels))) {
            m_aiSettingsChanged = true;
        }
        
        ImGui::Spacing();
        ImGui::TextDisabled("Install Ollama from: https://ollama.ai/");
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Status
    ImGui::Text("Status:");
    if (ai::AIAgentSystem::instance().isReady()) {
        ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.2f, 1.0f), "Connected");
    } else {
        ImGui::TextColored(ImVec4(0.8f, 0.5f, 0.2f, 1.0f), "Not configured");
        
        // Show why not ready
        if (m_selectedProvider == 2) {
            ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "Ollama provider not yet implemented!");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "Please use OpenAI or Anthropic for now.");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.4f, 1.0f), "Enter API key and click Save.");
        }
    }
    
    ImGui::Text("Registered tools: %zu", ai::EditorToolRegistry::instance().getToolCount());
}

void EditorSettingsPanel::renderEditorSettings() {
    ImGui::TextWrapped("General editor preferences.");
    ImGui::Spacing();
    
    // Auto-save
    ImGui::Checkbox("Auto-save enabled", &m_autoSave);
    if (m_autoSave) {
        ImGui::SliderInt("Auto-save interval (seconds)", &m_autoSaveInterval, 60, 600);
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Grid
    ImGui::Checkbox("Show grid", &m_showGrid);
    if (m_showGrid) {
        ImGui::SliderFloat("Grid size", &m_gridSize, 8.0f, 128.0f);
    }
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Theme (placeholder)
    ImGui::Text("Theme:");
    const char* themes[] = {"Dark (Default)", "Light", "Classic"};
    static int currentTheme = 0;
    ImGui::Combo("##theme", &currentTheme, themes, IM_ARRAYSIZE(themes));
    ImGui::TextDisabled("Theme switching coming soon.");
}

void EditorSettingsPanel::renderPerformanceSettings() {
    ImGui::TextWrapped("Performance and rendering settings.");
    ImGui::Spacing();
    
    // FPS
    ImGui::SliderInt("Target FPS", &m_targetFPS, 30, 144);
    ImGui::Checkbox("V-Sync", &m_vsync);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    // Memory info (placeholder)
    ImGui::Text("Memory Usage:");
    ImGui::TextDisabled("Memory statistics coming soon.");
}

void EditorSettingsPanel::renderAbout() {
    ImGui::TextWrapped("Retro Engine Editor");
    ImGui::Spacing();
    
    ImGui::Text("Version: 1.0.0-dev");
    ImGui::Text("Build: %s %s", __DATE__, __TIME__);
    
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    
    ImGui::TextWrapped("A modern 2D adventure game engine inspired by LucasArts classics.");
    
    ImGui::Spacing();
    
    ImGui::Text("Components:");
    ImGui::BulletText("SDL2 - Graphics & Input");
    ImGui::BulletText("Dear ImGui - Editor UI");
    ImGui::BulletText("Box2D - Physics");
    ImGui::BulletText("nlohmann/json - Data");
}

void EditorSettingsPanel::loadSettings() {
    LOG_DEBUG("[Settings] Loading editor settings");
    loadFromFile();
    
    // If we have saved API key and provider, configure the provider
    if (m_selectedProvider == 0 && strlen(m_openaiApiKey) > 0) {
        auto provider = std::make_unique<ai::OpenAIProvider>();
        provider->setApiKey(m_openaiApiKey);
        ai::AIAgentSystem::instance().setProvider(std::move(provider));
        LOG_INFO("[Settings] Restored OpenAI provider from config");
    } else if (m_selectedProvider == 1 && strlen(m_anthropicApiKey) > 0) {
        auto provider = std::make_unique<ai::AnthropicProvider>();
        provider->setApiKey(m_anthropicApiKey);
        
        // Set model based on saved selection
        const char* anthropicModels[] = {
            "claude-sonnet-4-5",           // Claude Sonnet 4.5 (recommended)
            "claude-sonnet-4-0",           // Claude Sonnet 4
            "claude-3-7-sonnet-latest",    // Claude Sonnet 3.7
            "claude-3-5-haiku-latest"      // Claude Haiku 3.5 (fast)
        };
        if (m_selectedModel < 4) {
            provider->setModel(anthropicModels[m_selectedModel]);
            LOG_INFO("[Settings] Restored model: " + std::string(anthropicModels[m_selectedModel]));
        }
        
        ai::AIAgentSystem::instance().setProvider(std::move(provider));
        LOG_INFO("[Settings] Restored Anthropic provider from config");
    }
}

void EditorSettingsPanel::loadFromFile() {
    std::ifstream file(CONFIG_FILE);
    if (!file.is_open()) {
        LOG_DEBUG("[Settings] No config file found, using defaults");
        return;
    }
    
    try {
        nlohmann::json config;
        file >> config;
        
        // AI Settings
        if (config.contains("ai")) {
            auto& ai = config["ai"];
            m_selectedProvider = ai.value("provider", 1);
            m_selectedModel = ai.value("model", 0);
            
            std::string openaiKey = ai.value("openai_key", "");
            std::string anthropicKey = ai.value("anthropic_key", "");
            
            strncpy(m_openaiApiKey, openaiKey.c_str(), sizeof(m_openaiApiKey) - 1);
            strncpy(m_anthropicApiKey, anthropicKey.c_str(), sizeof(m_anthropicApiKey) - 1);
        }
        
        // Editor Settings
        if (config.contains("editor")) {
            auto& editor = config["editor"];
            m_autoSave = editor.value("auto_save", true);
            m_autoSaveInterval = editor.value("auto_save_interval", 300);
            m_showGrid = editor.value("show_grid", true);
            m_gridSize = editor.value("grid_size", 32.0f);
        }
        
        // Performance Settings
        if (config.contains("performance")) {
            auto& perf = config["performance"];
            m_targetFPS = perf.value("target_fps", 60);
            m_vsync = perf.value("vsync", true);
        }
        
        LOG_INFO("[Settings] Loaded settings from " + std::string(CONFIG_FILE));
        
    } catch (const std::exception& e) {
        LOG_ERROR("[Settings] Failed to parse config: " + std::string(e.what()));
    }
}

void EditorSettingsPanel::saveToFile() {
    nlohmann::json config;
    
    // AI Settings
    config["ai"] = {
        {"provider", m_selectedProvider},
        {"model", m_selectedModel},
        {"openai_key", std::string(m_openaiApiKey)},
        {"anthropic_key", std::string(m_anthropicApiKey)}
    };
    
    // Editor Settings
    config["editor"] = {
        {"auto_save", m_autoSave},
        {"auto_save_interval", m_autoSaveInterval},
        {"show_grid", m_showGrid},
        {"grid_size", m_gridSize}
    };
    
    // Performance Settings
    config["performance"] = {
        {"target_fps", m_targetFPS},
        {"vsync", m_vsync}
    };
    
    std::ofstream file(CONFIG_FILE);
    if (!file.is_open()) {
        LOG_ERROR("[Settings] Failed to open config file for writing: " + std::string(CONFIG_FILE));
        return;
    }
    
    file << config.dump(2);
    LOG_INFO("[Settings] Saved settings to " + std::string(CONFIG_FILE));
}

void EditorSettingsPanel::saveSettings() {
    LOG_INFO("[Settings] ========== Saving Editor Settings ==========");
    LOG_INFO("[Settings] Selected provider: " + std::to_string(m_selectedProvider));
    LOG_INFO("[Settings] Selected model: " + std::to_string(m_selectedModel));
    LOG_INFO("[Settings] AI settings changed: " + std::string(m_aiSettingsChanged ? "yes" : "no"));
    
    // Always apply AI settings when Save is clicked (ensures model is updated)
    {
        if (m_selectedProvider == 0) {
            // OpenAI
            if (strlen(m_openaiApiKey) > 0) {
                LOG_INFO("[Settings] Creating OpenAI provider...");
                LOG_DEBUG("[Settings] API key length: " + std::to_string(strlen(m_openaiApiKey)));
                
                auto provider = std::make_unique<ai::OpenAIProvider>();
                provider->setApiKey(m_openaiApiKey);
                
                LOG_INFO("[Settings] Setting provider on AIAgentSystem...");
                ai::AIAgentSystem::instance().setProvider(std::move(provider));
                
                LOG_INFO("[Settings] OpenAI provider configured successfully");
                LOG_INFO("[Settings] Agent ready: " + std::string(ai::AIAgentSystem::instance().isReady() ? "yes" : "no"));
            } else {
                LOG_WARNING("[Settings] OpenAI selected but no API key provided");
            }
        } else if (m_selectedProvider == 1) {
            // Anthropic
            if (strlen(m_anthropicApiKey) > 0) {
                LOG_INFO("[Settings] Creating Anthropic provider...");
                LOG_DEBUG("[Settings] API key length: " + std::to_string(strlen(m_anthropicApiKey)));
                
                auto provider = std::make_unique<ai::AnthropicProvider>();
                provider->setApiKey(m_anthropicApiKey);
                
                // Set model based on selection
                const char* anthropicModels[] = {
                    "claude-sonnet-4-5",           // Claude Sonnet 4.5 (recommended)
                    "claude-sonnet-4-0",           // Claude Sonnet 4
                    "claude-3-7-sonnet-latest",    // Claude Sonnet 3.7
                    "claude-3-5-haiku-latest"      // Claude Haiku 3.5 (fast)
                };
                if (m_selectedModel < 4) {
                    provider->setModel(anthropicModels[m_selectedModel]);
                    LOG_INFO("[Settings] Selected model: " + std::string(anthropicModels[m_selectedModel]));
                }
                
                LOG_INFO("[Settings] Setting provider on AIAgentSystem...");
                ai::AIAgentSystem::instance().setProvider(std::move(provider));
                
                LOG_INFO("[Settings] Anthropic provider configured successfully");
                LOG_INFO("[Settings] Agent ready: " + std::string(ai::AIAgentSystem::instance().isReady() ? "yes" : "no"));
            } else {
                LOG_WARNING("[Settings] Anthropic selected but no API key provided");
            }
        } else if (m_selectedProvider == 2) {
            // Local/Ollama
            LOG_WARNING("[Settings] Ollama provider not yet implemented");
        }
        
        m_aiSettingsChanged = false;
    }
    
    // Save to file
    saveToFile();
    
    LOG_INFO("[Settings] ========== Settings Saved ==========");
}

} // namespace editor
