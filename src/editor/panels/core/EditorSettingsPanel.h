/**
 * @file EditorSettingsPanel.h
 * @brief Global settings panel for the editor
 * 
 * @details
 * Central panel for all editor settings including:
 * - AI Assistant configuration (providers, API keys)
 * - Editor preferences (theme, layout)
 * - Performance settings
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include <string>
#include <functional>

namespace editor {

/**
 * @class EditorSettingsPanel
 * @brief Modal panel for editor-wide settings
 * 
 * @details
 * Provides a tabbed interface for configuring various editor settings.
 * Opens as a modal window from Edit menu.
 */
class EditorSettingsPanel : public IEditorPanel {
public:
    EditorSettingsPanel();
    ~EditorSettingsPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    void render() override;
    
    void open() { m_isOpen = true; }
    void close() { m_isOpen = false; }
    bool isOpen() const { return m_isOpen; }
    
    std::function<void()> onSettingsChanged;
    
private:
    void renderAISettings();
    void renderEditorSettings();
    void renderPerformanceSettings();
    void renderAbout();
    
    void loadSettings();
    void saveSettings();
    void saveToFile();
    void loadFromFile();
    
    static constexpr const char* CONFIG_FILE = "assets/data/editor_config.json";
    
    std::string m_id = "editor_settings";
    std::string m_title = "Editor Settings";
    bool m_isOpen = false;
    
    // Current tab
    int m_currentTab = 0;
    
    // AI Settings
    char m_openaiApiKey[256] = {0};
    char m_anthropicApiKey[256] = {0};
    int m_selectedProvider = 1;  // 0=OpenAI, 1=Anthropic (default), 2=Local
    int m_selectedModel = 0;
    bool m_aiSettingsChanged = false;
    
    // Editor Settings
    bool m_autoSave = true;
    int m_autoSaveInterval = 300;  // seconds
    bool m_showGrid = true;
    float m_gridSize = 32.0f;
    
    // Performance
    int m_targetFPS = 60;
    bool m_vsync = true;
};

} // namespace editor
