/**
 * @file FileWatcher.h
 * @brief Polling-based file watcher for hot reload functionality
 * 
 * Watches asset files and notifies when they change,
 * enabling live content updates without restarting the game.
 * 
 * Uses simple polling (checking file timestamps) which is:
 * - Most robust and portable solution
 * - No external dependencies
 * - Thread-safe (runs on main thread)
 */
#pragma once

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <chrono>

/**
 * @brief Callback type for file change notifications
 * @param directory Directory containing the file
 * @param filename Name of the changed file
 */
using FileChangeCallback = std::function<void(const std::string& directory, const std::string& filename)>;

/**
 * @brief Represents a single watched file
 */
struct WatchedFile {
    std::filesystem::path path;
    std::filesystem::file_time_type lastModified;
};

/**
 * @brief Polling-based file watcher for hot reload
 * 
 * Call update() in game loop to check for changes.
 * Uses file timestamps - simple, robust, no threading issues.
 */
class FileWatcher {
public:
    static FileWatcher& instance();
    
    /**
     * @brief Start watching a directory for changes
     * @param path Directory to watch
     * @param recursive Watch subdirectories (currently ignored, watches specific files)
     */
    void watch(const std::string& path, bool recursive = true);
    
    /**
     * @brief Watch a specific file
     * @param filepath Full path to file
     */
    void watchFile(const std::string& filepath);
    
    /**
     * @brief Stop watching all files/directories
     */
    void stopAll();
    
    /**
     * @brief Register callback for file changes
     * @param callback Function to call when file changes
     */
    void onFileChanged(FileChangeCallback callback);
    
    /**
     * @brief Check if hot reload is available
     */
    bool isAvailable() const { return true; }
    
    /**
     * @brief Check for file changes (call every frame or periodically)
     * 
     * Checks file timestamps and triggers callbacks if changed.
     * Lightweight - only stats files, doesn't read content.
     */
    void update();
    
    /**
     * @brief Set polling interval in seconds
     * @param seconds Time between polls (default 1.0)
     */
    void setPollingInterval(float seconds) { m_pollingInterval = seconds; }

private:
    FileWatcher() = default;
    ~FileWatcher() = default;
    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;
    
    std::vector<FileChangeCallback> m_callbacks;
    std::unordered_map<std::string, WatchedFile> m_watchedFiles;
    std::vector<std::string> m_watchedDirectories;
    
    float m_pollingInterval = 1.0f;  // Check every second
    float m_timeSinceLastPoll = 0.0f;
    std::chrono::steady_clock::time_point m_lastPollTime;
    bool m_initialized = false;
};
