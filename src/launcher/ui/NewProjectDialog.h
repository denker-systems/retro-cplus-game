/**
 * @file NewProjectDialog.h
 * @brief New project creation dialog
 */
#pragma once

#include <string>
#include <vector>
#include <functional>

namespace launcher {

/**
 * @brief Dialog for creating new projects
 */
class NewProjectDialog {
public:
    using CreateCallback = std::function<void(const std::string& name, const std::string& path, const std::string& templateName)>;
    
    NewProjectDialog();
    
    /**
     * @brief Show the dialog
     */
    void show();
    
    /**
     * @brief Hide the dialog
     */
    void hide();
    
    /**
     * @brief Check if dialog is visible
     */
    bool isVisible() const { return m_visible; }
    
    /**
     * @brief Render dialog
     */
    void render();
    
    /**
     * @brief Set callback for project creation
     */
    void setOnCreate(CreateCallback callback) { m_onCreate = callback; }
    
    /**
     * @brief Set default project path
     */
    void setDefaultPath(const std::string& path);
    
private:
    bool m_visible = false;
    char m_projectName[256] = "";
    char m_projectPath[512] = "";
    int m_selectedTemplate = 0;
    std::vector<std::string> m_templates;
    CreateCallback m_onCreate;
};

} // namespace launcher
