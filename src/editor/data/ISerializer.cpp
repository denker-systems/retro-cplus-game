/**
 * @file ISerializer.cpp
 * @brief Implementation of ISerializer base class helper methods
 */
#include "ISerializer.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include "engine/utils/Logger.h"

namespace editor {

bool ISerializer::createBackup() const {
    std::string path = getFilePath();
    std::string backupPath = path + ".bak";
    
    try {
        if (std::filesystem::exists(path)) {
            std::filesystem::copy_file(
                path, backupPath,
                std::filesystem::copy_options::overwrite_existing
            );
            return true;
        }
    } catch (const std::exception& e) {
        LOG_WARNING("Failed to create backup: " + std::string(e.what()));
    }
    return false;
}

bool ISerializer::writeJsonToFile(const std::string& path, const std::string& jsonContent) const {
    // Create backup first
    if (std::filesystem::exists(path)) {
        std::string backupPath = path + ".bak";
        try {
            std::filesystem::copy_file(
                path, backupPath,
                std::filesystem::copy_options::overwrite_existing
            );
        } catch (const std::exception& e) {
            LOG_WARNING("Backup failed: " + std::string(e.what()));
        }
    }
    
    // Write new content
    std::ofstream file(path);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open file for writing: " + path);
        return false;
    }
    
    file << jsonContent;
    file.close();
    
    if (file.fail()) {
        LOG_ERROR("Failed to write to file: " + path);
        return false;
    }
    
    return true;
}

std::string ISerializer::readJsonFromFile(const std::string& path) const {
    std::ifstream file(path);
    if (!file.is_open()) {
        LOG_WARNING("File not found: " + path);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace editor
