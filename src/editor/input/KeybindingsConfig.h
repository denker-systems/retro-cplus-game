/**
 * @file KeybindingsConfig.h
 * @brief Keybindings configuration loader
 */
#pragma once

#include <string>
#include <unordered_map>
#include <SDL.h>

#ifdef HAS_IMGUI

namespace editor {

/**
 * @brief Keybinding entry
 */
struct Keybinding {
    SDL_Scancode key = SDL_SCANCODE_UNKNOWN;
    bool ctrl = false;
    bool shift = false;
    bool alt = false;
    
    bool matches(SDL_Scancode scancode, bool ctrlDown, bool shiftDown, bool altDown) const {
        return key == scancode && ctrl == ctrlDown && shift == shiftDown && alt == altDown;
    }
};

/**
 * @brief Keybindings configuration
 */
class KeybindingsConfig {
public:
    static KeybindingsConfig& instance();
    
    /**
     * @brief Load keybindings from JSON file
     */
    bool load(const std::string& path);
    
    /**
     * @brief Save keybindings to JSON file
     */
    bool save(const std::string& path) const;
    
    /**
     * @brief Reset to default keybindings
     */
    void resetToDefaults();
    
    /**
     * @brief Get keybinding for action
     */
    Keybinding getBinding(const std::string& category, const std::string& action) const;
    
    /**
     * @brief Set keybinding for action
     */
    void setBinding(const std::string& category, const std::string& action, const Keybinding& binding);
    
    /**
     * @brief Check if action is triggered
     */
    bool isActionTriggered(const std::string& category, const std::string& action,
                           SDL_Scancode key, bool ctrl, bool shift, bool alt) const;

private:
    KeybindingsConfig() { resetToDefaults(); }
    
    static Keybinding parseKeyString(const std::string& keyStr);
    static std::string keyToString(const Keybinding& binding);
    
    std::unordered_map<std::string, std::unordered_map<std::string, Keybinding>> m_bindings;
};

} // namespace editor

#endif // HAS_IMGUI
