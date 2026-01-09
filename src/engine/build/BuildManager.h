/**
 * @file BuildManager.h
 * @brief Manages project compilation and building
 */
#pragma once

#include <string>
#include <functional>

namespace engine {

/**
 * @brief Build configuration
 */
struct BuildConfig {
    std::string projectPath;
    std::string projectName;
    std::string enginePath;
    std::string outputPath;
    std::string configuration = "Release";  // Debug or Release
    bool verbose = true;
};

/**
 * @brief Build result
 */
struct BuildResult {
    bool success = false;
    std::string message;
    std::string executablePath;
    float buildTime = 0.0f;
};

/**
 * @brief Manages project building and compilation
 */
class BuildManager {
public:
    using ProgressCallback = std::function<void(const std::string& message, float progress)>;
    
    /**
     * @brief Build a project
     * @param config Build configuration
     * @param progressCallback Optional callback for progress updates
     * @return Build result
     */
    static BuildResult buildProject(const BuildConfig& config, ProgressCallback progressCallback = nullptr);
    
private:
    static bool runCMakeGenerate(const BuildConfig& config, ProgressCallback callback);
    static bool runCMakeBuild(const BuildConfig& config, ProgressCallback callback);
    static bool copyAssets(const BuildConfig& config, ProgressCallback callback);
    static bool copyDependencies(const BuildConfig& config, ProgressCallback callback);
    
    static bool executeCommand(const std::string& command, const std::string& workingDir);
};

} // namespace engine
