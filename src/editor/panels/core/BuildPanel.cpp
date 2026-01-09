/**
 * @file BuildPanel.cpp
 * @brief Build panel implementation
 */
#include "BuildPanel.h"
#include "editor/core/EditorContext.h"
#include "editor/ui/EditorTheme.h"
#include <filesystem>
#include <iostream>

#ifdef HAS_NFD
#include <nfd.h>
#endif

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace fs = std::filesystem;

BuildPanel::BuildPanel(EditorContext& context)
    : m_context(context) {
}

void BuildPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) {
        return;
    }
    
    // Center window on first use
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(600, 500), ImGuiCond_FirstUseEver);
    
    // Bring to front
    ImGui::SetNextWindowFocus();
    
    ImGuiWindowFlags flags = ImGuiWindowFlags_None;
    if (ImGui::Begin(m_title.c_str(), &m_visible, flags)) {
        renderBuildSettings();
        ImGui::Separator();
        
        renderBuildButton();
        ImGui::Separator();
        
        if (m_isBuilding) {
            renderBuildProgress();
            ImGui::Separator();
        }
        
        renderBuildLog();
    }
    ImGui::End();
#endif
}

void BuildPanel::renderBuildSettings() {
#ifdef HAS_IMGUI
    ImGui::Text("Build Settings");
    ImGui::Spacing();
    
    // Configuration selector
    const char* configs[] = { "Release", "Debug" };
    ImGui::Combo("Configuration", &m_selectedConfiguration, configs, 2);
    
    // Build output path
    ImGui::Text("Build Output:");
    ImGui::InputText("##buildoutput", m_buildOutputPath, sizeof(m_buildOutputPath));
    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        browseBuildOutput();
    }
    
    if (strlen(m_buildOutputPath) == 0) {
        ImGui::TextColored(ImVec4(0.6f, 0.6f, 0.6f, 1.0f), "Default: Documents/RetroEngine/Builds/ProjectName");
    }
    
    // Verbose output
    ImGui::Checkbox("Verbose Output", &m_verboseOutput);
    
    ImGui::Spacing();
#endif
}

void BuildPanel::browseBuildOutput() {
#ifdef HAS_NFD
    std::cout << "[BuildPanel] Opening folder picker..." << std::endl;
    
    NFD_Init();
    
    nfdchar_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolder(&outPath, nullptr);
    
    if (result == NFD_OKAY) {
        std::cout << "[BuildPanel] Selected folder: " << outPath << std::endl;
        strncpy_s(m_buildOutputPath, sizeof(m_buildOutputPath), outPath, _TRUNCATE);
        NFD_FreePath(outPath);
    } else if (result == NFD_CANCEL) {
        std::cout << "[BuildPanel] User cancelled folder selection" << std::endl;
    } else {
        std::cerr << "[BuildPanel] Error: " << NFD_GetError() << std::endl;
    }
    
    NFD_Quit();
#else
    std::cout << "[BuildPanel] Native file dialog not available" << std::endl;
    m_buildLog.push_back("ERROR: Native file dialog not compiled in");
#endif
}

void BuildPanel::renderBuildButton() {
#ifdef HAS_IMGUI
    if (m_isBuilding) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.5f));
        ImGui::Button("Building...", ImVec2(-1, 40));
        ImGui::PopStyleColor();
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, editor::EditorTheme::BrandColors::Primary);
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, editor::EditorTheme::BrandColors::PrimaryHover);
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, editor::EditorTheme::BrandColors::PrimaryActive);
        
        if (ImGui::Button("Build Project", ImVec2(-1, 40))) {
            startBuild();
        }
        
        ImGui::PopStyleColor(3);
    }
#endif
}

void BuildPanel::renderBuildProgress() {
#ifdef HAS_IMGUI
    ImGui::Text("Building: %s", m_currentBuildStep.c_str());
    ImGui::ProgressBar(m_buildProgress, ImVec2(-1, 0));
    ImGui::Spacing();
#endif
}

void BuildPanel::renderBuildLog() {
#ifdef HAS_IMGUI
    ImGui::Text("Build Log");
    ImGui::BeginChild("BuildLog", ImVec2(0, 200), true);
    
    for (const auto& line : m_buildLog) {
        if (line.find("ERROR") != std::string::npos || line.find("✗") != std::string::npos) {
            ImGui::TextColored(editor::EditorTheme::BrandColors::Error, "%s", line.c_str());
        } else if (line.find("WARNING") != std::string::npos) {
            ImGui::TextColored(editor::EditorTheme::BrandColors::Warning, "%s", line.c_str());
        } else if (line.find("✓") != std::string::npos) {
            ImGui::TextColored(editor::EditorTheme::BrandColors::Success, "%s", line.c_str());
        } else {
            ImGui::Text("%s", line.c_str());
        }
    }
    
    // Auto-scroll to bottom
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
        ImGui::SetScrollHereY(1.0f);
    }
    
    ImGui::EndChild();
#endif
}

void BuildPanel::startBuild() {
    std::cout << "[BuildPanel] Starting build..." << std::endl;
    
    m_isBuilding = true;
    m_buildProgress = 0.0f;
    m_buildLog.clear();
    m_buildLog.push_back("[BuildPanel] Build started...");
    
    // Try to get project path from EditorContext first
    std::string projectPath = m_context.currentProjectPath;
    std::string projectName = m_context.currentProjectName;
    bool foundProject = !projectPath.empty();
    
    if (foundProject) {
        std::cout << "[BuildPanel] ✓ Using project from EditorContext: " << projectName << " at " << projectPath << std::endl;
    } else {
        std::cout << "[BuildPanel] No project in EditorContext, searching for .retro file..." << std::endl;
        
        // Fallback: Detect project path - look for .retro file in current dir or parent dirs
        fs::path searchPath = fs::current_path();
    
        // Search up to 3 levels up for .retro file
        for (int i = 0; i < 3 && !foundProject; i++) {
            std::cout << "[BuildPanel] Searching for .retro in: " << searchPath << std::endl;
            
            try {
                for (const auto& entry : fs::directory_iterator(searchPath)) {
                    if (entry.path().extension() == ".retro") {
                        projectPath = searchPath.string();
                        projectName = entry.path().stem().string();
                        foundProject = true;
                        std::cout << "[BuildPanel] ✓ Found project: " << projectName << " at " << projectPath << std::endl;
                        break;
                    }
                }
            } catch (const std::exception& e) {
                std::cout << "[BuildPanel] Error searching: " << e.what() << std::endl;
            }
            
            if (!foundProject && searchPath.has_parent_path()) {
                searchPath = searchPath.parent_path();
            } else {
                break;
            }
        }
    }
    
    if (!foundProject) {
        m_buildLog.push_back("✗ ERROR: No .retro project file found!");
        m_buildLog.push_back("Make sure you opened a project from the launcher.");
        m_isBuilding = false;
        return;
    }
    
    // Engine path is where RetroCore.lib is located
    std::string enginePath = fs::current_path().parent_path().parent_path().string();
    
    m_buildLog.push_back("[BuildPanel] Project: " + projectName);
    m_buildLog.push_back("[BuildPanel] Path: " + projectPath);
    
    // Configure build
    engine::BuildConfig config;
    config.projectPath = projectPath;
    config.projectName = projectName;
    config.enginePath = enginePath;
    
    // Use custom output path if specified, otherwise default to RetroEngine/Builds/ProjectName
    if (strlen(m_buildOutputPath) > 0) {
        config.outputPath = m_buildOutputPath;
        std::cout << "[BuildPanel] Using custom output path: " << config.outputPath << std::endl;
    } else {
        // Default: C:\Users\Calle\Documents\RetroEngine\Builds\ProjectName
        const char* userProfile = std::getenv("USERPROFILE");
        if (userProfile) {
            config.outputPath = std::string(userProfile) + "\\Documents\\RetroEngine\\Builds\\" + projectName;
        } else {
            config.outputPath = projectPath + "/build/Release";
        }
        
        // Create builds directory if it doesn't exist
        try {
            fs::create_directories(config.outputPath);
            std::cout << "[BuildPanel] Using default output path: " << config.outputPath << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "[BuildPanel] Failed to create output directory: " << e.what() << std::endl;
            config.outputPath = projectPath + "/build/Release";
        }
    }
    config.configuration = (m_selectedConfiguration == 0) ? "Release" : "Debug";
    config.verbose = m_verboseOutput;
    
    // Start build (async in real implementation)
    m_buildLog.push_back("[BuildPanel] Generating CMake files...");
    
    engine::BuildResult result = engine::BuildManager::buildProject(config,
        [this](const std::string& msg, float progress) {
            onBuildProgress(msg, progress);
        }
    );
    
    onBuildComplete(result);
}

void BuildPanel::onBuildProgress(const std::string& message, float progress) {
    m_currentBuildStep = message;
    m_buildProgress = progress;
    m_buildLog.push_back(message);
    
    std::cout << "[BuildPanel] Progress: " << message << " (" << (progress * 100) << "%)" << std::endl;
}

void BuildPanel::onBuildComplete(const engine::BuildResult& result) {
    m_isBuilding = false;
    
    if (result.success) {
        m_buildLog.push_back("✓ Build completed successfully!");
        m_buildLog.push_back("Executable: " + result.executablePath);
        m_buildLog.push_back("Build time: " + std::to_string(result.buildTime) + "s");
    } else {
        m_buildLog.push_back("✗ Build failed: " + result.message);
    }
    
    std::cout << "[BuildPanel] Build complete: " << (result.success ? "SUCCESS" : "FAILED") << std::endl;
}
