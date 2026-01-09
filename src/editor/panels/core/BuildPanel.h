/**
 * @file BuildPanel.h
 * @brief Build panel for compiling projects
 */
#pragma once

#include "editor/core/IEditorPanel.h"
#include "engine/build/BuildManager.h"
#include <string>
#include <vector>

class EditorContext;

/**
 * @brief Panel for building and packaging projects
 */
class BuildPanel : public IEditorPanel {
public:
    explicit BuildPanel(EditorContext& context);
    ~BuildPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
private:
    void renderBuildSettings();
    void renderBuildButton();
    void renderBuildProgress();
    void renderBuildLog();
    
    void startBuild();
    void onBuildProgress(const std::string& message, float progress);
    void onBuildComplete(const engine::BuildResult& result);
    
    EditorContext& m_context;
    
    std::string m_id = "build_panel";
    std::string m_title = "Build";
    
    // Build state
    bool m_isBuilding = false;
    float m_buildProgress = 0.0f;
    std::string m_currentBuildStep;
    std::vector<std::string> m_buildLog;
    
    // Build settings
    int m_selectedConfiguration = 0;  // 0=Release, 1=Debug
    bool m_verboseOutput = true;
    char m_buildOutputPath[512] = "";  // Custom build output path
    
    void browseBuildOutput();
};
