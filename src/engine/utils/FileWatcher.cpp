/**
 * @file FileWatcher.cpp
 * @brief Polling-based file watcher implementation
 * 
 * Simple, robust hot reload using file timestamps.
 * No external dependencies, no threading issues.
 */
#include "FileWatcher.h"
#include "Logger.h"
#include <thread>

namespace fs = std::filesystem;

FileWatcher& FileWatcher::instance() {
    static FileWatcher instance;
    return instance;
}

void FileWatcher::watch(const std::string& path, bool recursive) {
    (void)recursive;  // Currently watches specific files in directory
    
    if (!fs::exists(path)) {
        LOG_ERROR("FileWatcher: Directory not found: " + path);
        return;
    }
    
    m_watchedDirectories.push_back(path);
    
    // Watch all JSON files in the directory
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            watchFile(entry.path().string());
        }
    }
    
    if (!m_initialized) {
        m_lastPollTime = std::chrono::steady_clock::now();
        m_initialized = true;
    }
    
    LOG_INFO("FileWatcher: Watching directory " + path + " (" + 
             std::to_string(m_watchedFiles.size()) + " files)");
}

void FileWatcher::watchFile(const std::string& filepath) {
    if (!fs::exists(filepath)) {
        LOG_WARNING("FileWatcher: File not found: " + filepath);
        return;
    }
    
    WatchedFile wf;
    wf.path = filepath;
    wf.lastModified = fs::last_write_time(filepath);
    
    m_watchedFiles[filepath] = wf;
    LOG_DEBUG("FileWatcher: Watching file " + filepath);
}

void FileWatcher::stopAll() {
    m_watchedFiles.clear();
    m_watchedDirectories.clear();
    m_callbacks.clear();
    m_initialized = false;
    LOG_INFO("FileWatcher: Stopped all watches");
}

void FileWatcher::onFileChanged(FileChangeCallback callback) {
    m_callbacks.push_back(callback);
}

void FileWatcher::update() {
    if (!m_initialized || m_watchedFiles.empty()) {
        return;
    }
    
    // Check polling interval
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - m_lastPollTime).count();
    
    if (elapsed < m_pollingInterval) {
        return;
    }
    
    m_lastPollTime = now;
    
    // Debug: Log that we're polling
    static int pollCount = 0;
    if (++pollCount % 10 == 0) {  // Log every 10th poll
        LOG_DEBUG("FileWatcher: Polling... (" + std::to_string(m_watchedFiles.size()) + " files)");
    }
    
    // Check each watched file for changes
    for (auto& [filepath, watchedFile] : m_watchedFiles) {
        try {
            if (!fs::exists(watchedFile.path)) {
                continue;
            }
            
            auto currentTime = fs::last_write_time(watchedFile.path);
            
            if (currentTime != watchedFile.lastModified) {
                // File changed!
                watchedFile.lastModified = currentTime;
                
                std::string directory = watchedFile.path.parent_path().string();
                std::string filename = watchedFile.path.filename().string();
                
                LOG_INFO("FileWatcher: File changed - " + filename);
                
                // Wait a moment for file to be fully written
                // (editors often write in multiple steps)
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                
                // Notify all callbacks
                for (auto& callback : m_callbacks) {
                    try {
                        LOG_DEBUG("FileWatcher: Calling callback for " + filename);
                        callback(directory, filename);
                        LOG_DEBUG("FileWatcher: Callback completed for " + filename);
                    } catch (const std::exception& e) {
                        LOG_ERROR("FileWatcher callback error: " + std::string(e.what()));
                    } catch (...) {
                        LOG_ERROR("FileWatcher callback unknown error");
                    }
                }
            }
        } catch (const fs::filesystem_error& e) {
            LOG_WARNING("FileWatcher: Error checking file: " + std::string(e.what()));
        }
    }
}
