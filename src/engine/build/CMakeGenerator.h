/**
 * @file CMakeGenerator.h
 * @brief Generates CMakeLists.txt for Retro Engine projects
 */
#pragma once

#include <string>
#include <vector>

namespace engine {

/**
 * @brief Generates CMakeLists.txt for building standalone game executables
 */
class CMakeGenerator {
public:
    /**
     * @brief Generate CMakeLists.txt for a project
     * @param projectPath Path to project directory
     * @param projectName Name of the project
     * @param enginePath Path to Retro Engine installation
     * @return true if generation succeeded
     */
    static bool generateForProject(
        const std::string& projectPath,
        const std::string& projectName,
        const std::string& enginePath
    );
    
private:
    static std::string generateCMakeContent(
        const std::string& projectName,
        const std::string& enginePath
    );
    
    static std::string generateMainCpp(const std::string& projectName);
};

} // namespace engine
