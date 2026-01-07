/**
 * @file FileBrowser.cpp
 * @brief Implementation of FileBrowser utility class
 */
#include "FileBrowser.h"
#include <nfd.h>

namespace editor {

// ═══════════════════════════════════════════════════════════════════════════
// COMMON FILTERS
// ═══════════════════════════════════════════════════════════════════════════

const std::vector<FileFilter> FileBrowser::IMAGE_FILTERS = {
    {"Images", "png,jpg,jpeg,gif,bmp,webp"}
};

const std::vector<FileFilter> FileBrowser::AUDIO_FILTERS = {
    {"Audio", "wav,ogg,mp3,flac"}
};

const std::vector<FileFilter> FileBrowser::JSON_FILTERS = {
    {"JSON", "json"}
};

const std::vector<FileFilter> FileBrowser::ALL_FILTERS = {};

// ═══════════════════════════════════════════════════════════════════════════
// HELPER: Convert FileFilter to nfd format
// ═══════════════════════════════════════════════════════════════════════════

static std::vector<nfdfilteritem_t> toNfdFilters(const std::vector<FileFilter>& filters) {
    std::vector<nfdfilteritem_t> nfdFilters;
    nfdFilters.reserve(filters.size());
    
    for (const auto& filter : filters) {
        nfdfilteritem_t item;
        item.name = filter.name.c_str();
        item.spec = filter.extensions.c_str();
        nfdFilters.push_back(item);
    }
    
    return nfdFilters;
}

// ═══════════════════════════════════════════════════════════════════════════
// SINGLE FILE SELECTION
// ═══════════════════════════════════════════════════════════════════════════

FileBrowserResult FileBrowser::openFile(
    const std::vector<FileFilter>& filters,
    const std::string& defaultPath
) {
    FileBrowserResult result;
    
    nfdchar_t* outPath = nullptr;
    auto nfdFilters = toNfdFilters(filters);
    
    const char* defPath = defaultPath.empty() ? nullptr : defaultPath.c_str();
    
    nfdresult_t nfdResult = NFD_OpenDialog(
        &outPath,
        nfdFilters.empty() ? nullptr : nfdFilters.data(),
        static_cast<nfdfiltersize_t>(nfdFilters.size()),
        defPath
    );
    
    if (nfdResult == NFD_OKAY) {
        result.success = true;
        result.path = outPath;
        NFD_FreePath(outPath);
    } else if (nfdResult == NFD_CANCEL) {
        result.success = false;
        result.error = "User cancelled";
    } else {
        result.success = false;
        result.error = NFD_GetError();
    }
    
    return result;
}

FileBrowserResult FileBrowser::openImage(const std::string& defaultPath) {
    return openFile(IMAGE_FILTERS, defaultPath);
}

FileBrowserResult FileBrowser::openAudio(const std::string& defaultPath) {
    return openFile(AUDIO_FILTERS, defaultPath);
}

FileBrowserResult FileBrowser::openJson(const std::string& defaultPath) {
    return openFile(JSON_FILTERS, defaultPath);
}

// ═══════════════════════════════════════════════════════════════════════════
// MULTIPLE FILE SELECTION
// ═══════════════════════════════════════════════════════════════════════════

std::vector<std::string> FileBrowser::openFiles(
    const std::vector<FileFilter>& filters,
    const std::string& defaultPath
) {
    std::vector<std::string> results;
    
    const nfdpathset_t* outPaths = nullptr;
    auto nfdFilters = toNfdFilters(filters);
    
    const char* defPath = defaultPath.empty() ? nullptr : defaultPath.c_str();
    
    nfdresult_t nfdResult = NFD_OpenDialogMultiple(
        &outPaths,
        nfdFilters.empty() ? nullptr : nfdFilters.data(),
        static_cast<nfdfiltersize_t>(nfdFilters.size()),
        defPath
    );
    
    if (nfdResult == NFD_OKAY && outPaths) {
        nfdpathsetsize_t count;
        NFD_PathSet_GetCount(outPaths, &count);
        
        for (nfdpathsetsize_t i = 0; i < count; ++i) {
            nfdchar_t* path;
            NFD_PathSet_GetPath(outPaths, i, &path);
            results.push_back(path);
            NFD_PathSet_FreePath(path);
        }
        
        NFD_PathSet_Free(outPaths);
    }
    
    return results;
}

// ═══════════════════════════════════════════════════════════════════════════
// SAVE DIALOGS
// ═══════════════════════════════════════════════════════════════════════════

FileBrowserResult FileBrowser::saveFile(
    const std::vector<FileFilter>& filters,
    const std::string& defaultPath,
    const std::string& defaultName
) {
    FileBrowserResult result;
    
    nfdchar_t* outPath = nullptr;
    auto nfdFilters = toNfdFilters(filters);
    
    const char* defPath = defaultPath.empty() ? nullptr : defaultPath.c_str();
    const char* defName = defaultName.empty() ? nullptr : defaultName.c_str();
    
    nfdresult_t nfdResult = NFD_SaveDialog(
        &outPath,
        nfdFilters.empty() ? nullptr : nfdFilters.data(),
        static_cast<nfdfiltersize_t>(nfdFilters.size()),
        defPath,
        defName
    );
    
    if (nfdResult == NFD_OKAY) {
        result.success = true;
        result.path = outPath;
        NFD_FreePath(outPath);
    } else if (nfdResult == NFD_CANCEL) {
        result.success = false;
        result.error = "User cancelled";
    } else {
        result.success = false;
        result.error = NFD_GetError();
    }
    
    return result;
}

// ═══════════════════════════════════════════════════════════════════════════
// FOLDER SELECTION
// ═══════════════════════════════════════════════════════════════════════════

FileBrowserResult FileBrowser::pickFolder(const std::string& defaultPath) {
    FileBrowserResult result;
    
    nfdchar_t* outPath = nullptr;
    const char* defPath = defaultPath.empty() ? nullptr : defaultPath.c_str();
    
    nfdresult_t nfdResult = NFD_PickFolder(&outPath, defPath);
    
    if (nfdResult == NFD_OKAY) {
        result.success = true;
        result.path = outPath;
        NFD_FreePath(outPath);
    } else if (nfdResult == NFD_CANCEL) {
        result.success = false;
        result.error = "User cancelled";
    } else {
        result.success = false;
        result.error = NFD_GetError();
    }
    
    return result;
}

} // namespace editor
