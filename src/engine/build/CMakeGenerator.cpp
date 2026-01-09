/**
 * @file CMakeGenerator.cpp
 * @brief CMake generation implementation
 */
#include "CMakeGenerator.h"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace engine {

bool CMakeGenerator::generateForProject(
    const std::string& projectPath,
    const std::string& projectName,
    const std::string& enginePath
) {
    std::cout << "[CMakeGenerator] Generating CMakeLists.txt for: " << projectName << std::endl;
    std::cout << "[CMakeGenerator] Project path: " << projectPath << std::endl;
    std::cout << "[CMakeGenerator] Engine path: " << enginePath << std::endl;
    
    try {
        // Generate CMakeLists.txt
        std::string cmakeContent = generateCMakeContent(projectName, enginePath);
        std::ofstream cmakeFile(fs::path(projectPath) / "CMakeLists.txt");
        if (!cmakeFile.is_open()) {
            std::cerr << "[CMakeGenerator] Failed to create CMakeLists.txt" << std::endl;
            return false;
        }
        cmakeFile << cmakeContent;
        cmakeFile.close();
        std::cout << "[CMakeGenerator] ✓ CMakeLists.txt generated" << std::endl;
        
        // Generate vcpkg.json for dependencies
        std::string vcpkgContent = R"({
  "name": ")" + projectName + R"(",
  "version": "1.0.0",
  "dependencies": [
    "sdl2",
    "sdl2-image",
    "sdl2-mixer",
    "sdl2-ttf",
    "nlohmann-json",
    "box2d",
    "glm",
    "glew",
    "assimp",
    "physx"
  ]
}
)";
        
        std::ofstream vcpkgFile(fs::path(projectPath) / "vcpkg.json");
        if (vcpkgFile.is_open()) {
            vcpkgFile << vcpkgContent;
            vcpkgFile.close();
            std::cout << "[CMakeGenerator] ✓ vcpkg.json generated" << std::endl;
        }
        
        // Generate main.cpp (always overwrite to ensure correct API usage)
        fs::path mainCppPath = fs::path(projectPath) / "src" / "main.cpp";
        fs::create_directories(mainCppPath.parent_path());
        
        std::string mainContent = generateMainCpp(projectName);
        std::ofstream mainFile(mainCppPath);
        if (!mainFile.is_open()) {
            std::cerr << "[CMakeGenerator] Failed to create main.cpp" << std::endl;
            return false;
        }
        mainFile << mainContent;
        mainFile.close();
        std::cout << "[CMakeGenerator] ✓ main.cpp generated (overwritten)" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[CMakeGenerator] Error: " << e.what() << std::endl;
        return false;
    }
}

std::string CMakeGenerator::generateCMakeContent(
    const std::string& projectName,
    const std::string& enginePath
) {
    std::string cmake = R"(cmake_minimum_required(VERSION 3.16)
project()";
    
    cmake += projectName;
    cmake += R"( VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use vcpkg for dependencies
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "C:/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# ============================================================================
# DEPENDENCIES
# ============================================================================
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(unofficial-omniverse-physx-sdk CONFIG REQUIRED)

# ============================================================================
# ENGINE PATH
# ============================================================================
set(ENGINE_PATH ")";
    
    // Replace backslashes with forward slashes for CMake
    std::string cmakeEnginePath = enginePath;
    std::replace(cmakeEnginePath.begin(), cmakeEnginePath.end(), '\\', '/');
    
    cmake += cmakeEnginePath;
    cmake += R"(")

# ============================================================================
# GAME EXECUTABLE - Runtime Player (3D from scratch)
# ============================================================================
# Uses new /runtime architecture instead of legacy /game states
set(RUNTIME_SOURCES
    src/main.cpp
    ${ENGINE_PATH}/src/runtime/RuntimeApp.cpp
    ${ENGINE_PATH}/src/runtime/RuntimeWorld.cpp
    ${ENGINE_PATH}/src/runtime/RuntimeRenderer.cpp
    ${ENGINE_PATH}/src/editor/viewport/3d/EditorCamera3D.cpp
)

add_executable()";
    cmake += projectName;
    cmake += R"( ${RUNTIME_SOURCES})

target_include_directories()";
    cmake += projectName;
    cmake += R"( PRIVATE
    ${ENGINE_PATH}/src
    ${ENGINE_PATH}/src/engine
)

target_link_libraries()";
    cmake += projectName;
    cmake += R"( PRIVATE
    ${ENGINE_PATH}/build/Release/RetroCore.lib
    SDL2::SDL2
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    $<IF:$<TARGET_EXISTS:SDL2_ttf::SDL2_ttf>,SDL2_ttf::SDL2_ttf,SDL2_ttf::SDL2_ttf-static>
    nlohmann_json::nlohmann_json
    box2d::box2d
    glm::glm
    GLEW::GLEW
    OpenGL::GL
    assimp::assimp
    unofficial::omniverse-physx-sdk::sdk
)

# ============================================================================
# POST BUILD - Copy assets
# ============================================================================
add_custom_command(TARGET )";
    cmake += projectName;
    cmake += R"( POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:)";
    cmake += projectName;
    cmake += R"(>/assets
)
)";
    
    return cmake;
}

std::string CMakeGenerator::generateMainCpp(const std::string& projectName) {
    std::string main = R"(
/**
 * @file main.cpp
 * @brief )";
    
    main += projectName;
    main += R"( entry point - Runtime Player
 */
#include "runtime/RuntimeApp.h"
#include "engine/utils/Logger.h"
#include <iostream>

int main(int argc, char* argv[]) {
    // Initialize logger
    Logger::instance().init("assets/logs/runtime.log");
    
    LOG_INFO("========================================");
    LOG_INFO("=== )";
    main += projectName;
    main += R"( STARTING ===");
    LOG_INFO("========================================");
    
    RuntimeApp app;
    
    if (!app.init()) {
        LOG_ERROR("Failed to initialize RuntimeApp");
        return -1;
    }
    
    app.run();
    app.shutdown();
    
    LOG_INFO("========================================");
    LOG_INFO("=== )";
    main += projectName;
    main += R"( SHUTDOWN ===");
    LOG_INFO("========================================");
    
    return 0;
}
)";
    
    return main;
}

} // namespace engine
