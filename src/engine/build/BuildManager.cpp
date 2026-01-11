/**
 * @file BuildManager.cpp
 * @brief Build manager implementation
 */
#include "BuildManager.h"
#include "CMakeGenerator.h"
#include <filesystem>
#include <iostream>
#include <chrono>
#include <cstdlib>

namespace fs = std::filesystem;

namespace engine {

BuildResult BuildManager::buildProject(const BuildConfig& config, ProgressCallback progressCallback) {
    std::cout << "[BuildManager] ========================================" << std::endl;
    std::cout << "[BuildManager] Starting build for: " << config.projectName << std::endl;
    std::cout << "[BuildManager] ========================================" << std::endl;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    BuildResult result;
    
    // Step 1: Generate CMakeLists.txt
    if (progressCallback) progressCallback("Generating CMake files...", 0.1f);
    
    if (!CMakeGenerator::generateForProject(config.projectPath, config.projectName, config.enginePath)) {
        result.success = false;
        result.message = "Failed to generate CMakeLists.txt";
        return result;
    }
    
    // Step 2: Run CMake configure
    if (progressCallback) progressCallback("Configuring project...", 0.3f);
    
    if (!runCMakeGenerate(config, progressCallback)) {
        result.success = false;
        result.message = "CMake configuration failed";
        return result;
    }
    
    // Step 3: Run CMake build
    if (progressCallback) progressCallback("Compiling project...", 0.5f);
    
    if (!runCMakeBuild(config, progressCallback)) {
        result.success = false;
        result.message = "Compilation failed";
        return result;
    }
    
    // Step 4: Copy assets
    if (progressCallback) progressCallback("Copying assets...", 0.8f);
    
    if (!copyAssets(config, progressCallback)) {
        result.success = false;
        result.message = "Failed to copy assets";
        return result;
    }
    
    // Step 5: Copy DLL dependencies
    if (progressCallback) progressCallback("Copying dependencies...", 0.9f);
    
    copyDependencies(config, progressCallback);
    
    // Complete
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
    
    result.success = true;
    result.message = "Build completed successfully";
    result.executablePath = config.outputPath + "/" + config.projectName + ".exe";
    result.buildTime = duration.count();
    
    if (progressCallback) progressCallback("Build complete!", 1.0f);
    
    std::cout << "[BuildManager] ========================================" << std::endl;
    std::cout << "[BuildManager] ✓ Build completed in " << result.buildTime << "s" << std::endl;
    std::cout << "[BuildManager] Executable: " << result.executablePath << std::endl;
    std::cout << "[BuildManager] ========================================" << std::endl;
    
    return result;
}

bool BuildManager::runCMakeGenerate(const BuildConfig& config, ProgressCallback callback) {
    std::cout << "[BuildManager] Running CMake configure..." << std::endl;
    
    std::string buildDir = config.projectPath + "/build";
    fs::create_directories(buildDir);
    
    // CMake command with vcpkg manifest mode
    std::string command = "cmake .. "
        "-DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake "
        "-DVCPKG_MANIFEST_MODE=ON "
        "-DVCPKG_MANIFEST_DIR=..";
    
    std::cout << "[BuildManager] Build directory: " << buildDir << std::endl;
    std::cout << "[BuildManager] CMake command: " << command << std::endl;
    
    return executeCommand(command, buildDir);
}

bool BuildManager::runCMakeBuild(const BuildConfig& config, ProgressCallback callback) {
    std::cout << "[BuildManager] Running CMake build..." << std::endl;
    
    std::string buildDir = config.projectPath + "/build";
    
    // CMake build command
    std::string command = "cmake --build . --config " + config.configuration;
    
    return executeCommand(command, buildDir);
}

bool BuildManager::copyAssets(const BuildConfig& config, ProgressCallback callback) {
    std::cout << "[BuildManager] Copying build output to: " << config.outputPath << std::endl;
    
    try {
        // Create output directory
        fs::create_directories(config.outputPath);
        
        // Copy executable from project/build/Release/ to output path
        fs::path srcExe = fs::path(config.projectPath) / "build" / config.configuration / (config.projectName + ".exe");
        fs::path dstExe = fs::path(config.outputPath) / (config.projectName + ".exe");
        
        if (fs::exists(srcExe)) {
            fs::copy_file(srcExe, dstExe, fs::copy_options::overwrite_existing);
            std::cout << "[BuildManager] ✓ Executable copied to: " << dstExe << std::endl;
        } else {
            std::cerr << "[BuildManager] WARNING: Executable not found at: " << srcExe << std::endl;
        }
        
        // Copy assets from editor's WORKING DIRECTORY (build/Release/assets)
        // Editor saves data relative to its working directory, not engine root
        fs::path editorWorkingDir = fs::path(config.enginePath) / "build" / "Release" / "assets";
        fs::path engineAssets = fs::path(config.enginePath) / "assets";
        fs::path dstAssets = fs::path(config.outputPath) / "assets";
        
        // Remove old assets first to ensure clean copy
        if (fs::exists(dstAssets)) {
            fs::remove_all(dstAssets);
        }
        
        // Priority 1: Copy from editor's working directory (has latest saved data)
        if (fs::exists(editorWorkingDir)) {
            fs::copy(editorWorkingDir, dstAssets, 
                    fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            std::cout << "[BuildManager] ✓ Assets copied from editor working dir: " << editorWorkingDir << std::endl;
        } else if (fs::exists(engineAssets)) {
            // Fallback to engine assets
            fs::copy(engineAssets, dstAssets, 
                    fs::copy_options::recursive | fs::copy_options::overwrite_existing);
            std::cout << "[BuildManager] ✓ Assets copied from engine: " << engineAssets << std::endl;
        } else {
            // Fallback to project assets
            fs::path srcAssets = fs::path(config.projectPath) / "assets";
            if (fs::exists(srcAssets)) {
                fs::copy(srcAssets, dstAssets, 
                        fs::copy_options::recursive | fs::copy_options::overwrite_existing);
                std::cout << "[BuildManager] ✓ Assets copied from project" << std::endl;
            } else {
                std::cout << "[BuildManager] WARNING: No assets folder found" << std::endl;
            }
        }
        
        // Copy DLLs from project/build/Release/ to output path
        fs::path srcBuildDir = fs::path(config.projectPath) / "build" / config.configuration;
        for (const auto& entry : fs::directory_iterator(srcBuildDir)) {
            if (entry.path().extension() == ".dll") {
                fs::path dstDll = fs::path(config.outputPath) / entry.path().filename();
                fs::copy_file(entry.path(), dstDll, fs::copy_options::overwrite_existing);
                std::cout << "[BuildManager] ✓ Copied DLL: " << entry.path().filename() << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[BuildManager] Error copying files: " << e.what() << std::endl;
        return false;
    }
}

bool BuildManager::copyDependencies(const BuildConfig& config, ProgressCallback callback) {
    std::cout << "[BuildManager] Copying DLL dependencies..." << std::endl;
    
    // vcpkg handles this automatically via applocal.ps1
    // No manual copying needed
    
    return true;
}

bool BuildManager::executeCommand(const std::string& command, const std::string& workingDir) {
    std::cout << "[BuildManager] Executing: " << command << std::endl;
    std::cout << "[BuildManager] Working dir: " << workingDir << std::endl;
    
    // Change to working directory and execute
    std::string fullCommand = "cd \"" + workingDir + "\" && " + command;
    
    int exitCode = system(fullCommand.c_str());
    
    if (exitCode == 0) {
        std::cout << "[BuildManager] ✓ Command succeeded" << std::endl;
        return true;
    } else {
        std::cerr << "[BuildManager] ✗ Command failed with exit code: " << exitCode << std::endl;
        return false;
    }
}

} // namespace engine
