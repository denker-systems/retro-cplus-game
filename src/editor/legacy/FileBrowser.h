/**
 * @file FileBrowser.h
 * @brief Native file browser utility class
 * 
 * Wraps nativefiledialog-extended for consistent file selection
 * across the editor. Follows OOP patterns with static utility methods.
 */
#pragma once

#include <string>
#include <vector>
#include <optional>

namespace editor {

/**
 * @brief File filter for dialog
 */
struct FileFilter {
    std::string name;        // Display name (e.g., "Images")
    std::string extensions;  // Comma-separated (e.g., "png,jpg,gif")
};

/**
 * @brief Result from file browser operation
 */
struct FileBrowserResult {
    bool success = false;
    std::string path;
    std::string error;
};

/**
 * @brief Native file browser utility
 * 
 * Provides static methods for opening file dialogs.
 * Uses nativefiledialog-extended under the hood.
 * 
 * Example usage:
 * @code
 *   auto result = FileBrowser::openFile({{"Images", "png,jpg"}});
 *   if (result.success) {
 *       loadTexture(result.path);
 *   }
 * @endcode
 */
class FileBrowser {
public:
    // ═══════════════════════════════════════════════════════════════════
    // SINGLE FILE SELECTION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Open single file dialog
     * @param filters File type filters (empty = all files)
     * @param defaultPath Starting directory (empty = current)
     * @return FileBrowserResult with path or error
     */
    static FileBrowserResult openFile(
        const std::vector<FileFilter>& filters = {},
        const std::string& defaultPath = ""
    );
    
    /**
     * @brief Open image file dialog (convenience)
     * @return FileBrowserResult with path or error
     */
    static FileBrowserResult openImage(const std::string& defaultPath = "");
    
    /**
     * @brief Open audio file dialog (convenience)
     * @return FileBrowserResult with path or error
     */
    static FileBrowserResult openAudio(const std::string& defaultPath = "");
    
    /**
     * @brief Open JSON file dialog (convenience)
     * @return FileBrowserResult with path or error
     */
    static FileBrowserResult openJson(const std::string& defaultPath = "");
    
    // ═══════════════════════════════════════════════════════════════════
    // MULTIPLE FILE SELECTION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Open multiple files dialog
     * @param filters File type filters
     * @param defaultPath Starting directory
     * @return Vector of selected paths (empty if cancelled)
     */
    static std::vector<std::string> openFiles(
        const std::vector<FileFilter>& filters = {},
        const std::string& defaultPath = ""
    );
    
    // ═══════════════════════════════════════════════════════════════════
    // SAVE DIALOGS
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Open save file dialog
     * @param filters File type filters
     * @param defaultPath Starting directory
     * @param defaultName Default filename
     * @return FileBrowserResult with path or error
     */
    static FileBrowserResult saveFile(
        const std::vector<FileFilter>& filters = {},
        const std::string& defaultPath = "",
        const std::string& defaultName = ""
    );
    
    // ═══════════════════════════════════════════════════════════════════
    // FOLDER SELECTION
    // ═══════════════════════════════════════════════════════════════════
    
    /**
     * @brief Open folder picker dialog
     * @param defaultPath Starting directory
     * @return FileBrowserResult with folder path or error
     */
    static FileBrowserResult pickFolder(const std::string& defaultPath = "");
    
    // ═══════════════════════════════════════════════════════════════════
    // COMMON FILTERS
    // ═══════════════════════════════════════════════════════════════════
    
    static const std::vector<FileFilter> IMAGE_FILTERS;
    static const std::vector<FileFilter> AUDIO_FILTERS;
    static const std::vector<FileFilter> JSON_FILTERS;
    static const std::vector<FileFilter> ALL_FILTERS;
};

} // namespace editor
