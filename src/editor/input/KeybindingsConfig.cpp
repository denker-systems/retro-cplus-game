/**
 * @file KeybindingsConfig.cpp
 * @brief Keybindings configuration implementation
 */
#include "KeybindingsConfig.h"

#ifdef HAS_IMGUI

#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>

namespace editor {

KeybindingsConfig& KeybindingsConfig::instance() {
    static KeybindingsConfig s_instance;
    return s_instance;
}

bool KeybindingsConfig::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        m_bindings.clear();
        
        for (auto& [category, actions] : j.items()) {
            if (category == "version") continue;
            
            for (auto& [action, keyStr] : actions.items()) {
                if (keyStr.is_string()) {
                    m_bindings[category][action] = parseKeyString(keyStr.get<std::string>());
                }
            }
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool KeybindingsConfig::save(const std::string& path) const {
    nlohmann::json j;
    j["version"] = "1.0";
    
    for (const auto& [category, actions] : m_bindings) {
        for (const auto& [action, binding] : actions) {
            j[category][action] = keyToString(binding);
        }
    }
    
    std::ofstream file(path);
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(2);
    return true;
}

void KeybindingsConfig::resetToDefaults() {
    m_bindings.clear();
    
    // Camera
    m_bindings["camera"]["fly_forward"] = {SDL_SCANCODE_W, false, false, false};
    m_bindings["camera"]["fly_back"] = {SDL_SCANCODE_S, false, false, false};
    m_bindings["camera"]["fly_left"] = {SDL_SCANCODE_A, false, false, false};
    m_bindings["camera"]["fly_right"] = {SDL_SCANCODE_D, false, false, false};
    m_bindings["camera"]["fly_up"] = {SDL_SCANCODE_E, false, false, false};
    m_bindings["camera"]["fly_down"] = {SDL_SCANCODE_Q, false, false, false};
    m_bindings["camera"]["focus"] = {SDL_SCANCODE_F, false, false, false};
    
    // Gizmo
    m_bindings["gizmo"]["translate"] = {SDL_SCANCODE_W, false, false, false};
    m_bindings["gizmo"]["rotate"] = {SDL_SCANCODE_E, false, false, false};
    m_bindings["gizmo"]["scale"] = {SDL_SCANCODE_R, false, false, false};
    m_bindings["gizmo"]["toggle_space"] = {SDL_SCANCODE_SPACE, false, false, false};
    
    // Selection
    m_bindings["selection"]["delete"] = {SDL_SCANCODE_DELETE, false, false, false};
    m_bindings["selection"]["duplicate"] = {SDL_SCANCODE_D, true, false, false};
    m_bindings["selection"]["deselect"] = {SDL_SCANCODE_ESCAPE, false, false, false};
    m_bindings["selection"]["select_all"] = {SDL_SCANCODE_A, true, false, false};
    
    // File
    m_bindings["file"]["save"] = {SDL_SCANCODE_S, true, false, false};
    m_bindings["file"]["save_as"] = {SDL_SCANCODE_S, true, true, false};
    m_bindings["file"]["open"] = {SDL_SCANCODE_O, true, false, false};
    m_bindings["file"]["new"] = {SDL_SCANCODE_N, true, false, false};
    
    // Edit
    m_bindings["edit"]["undo"] = {SDL_SCANCODE_Z, true, false, false};
    m_bindings["edit"]["redo"] = {SDL_SCANCODE_Y, true, false, false};
    m_bindings["edit"]["cut"] = {SDL_SCANCODE_X, true, false, false};
    m_bindings["edit"]["copy"] = {SDL_SCANCODE_C, true, false, false};
    m_bindings["edit"]["paste"] = {SDL_SCANCODE_V, true, false, false};
    
    // View
    m_bindings["view"]["toggle_grid"] = {SDL_SCANCODE_G, false, false, false};
}

Keybinding KeybindingsConfig::getBinding(const std::string& category, const std::string& action) const {
    auto catIt = m_bindings.find(category);
    if (catIt == m_bindings.end()) return {};
    
    auto actIt = catIt->second.find(action);
    if (actIt == catIt->second.end()) return {};
    
    return actIt->second;
}

void KeybindingsConfig::setBinding(const std::string& category, const std::string& action, const Keybinding& binding) {
    m_bindings[category][action] = binding;
}

bool KeybindingsConfig::isActionTriggered(const std::string& category, const std::string& action,
                                          SDL_Scancode key, bool ctrl, bool shift, bool alt) const {
    auto binding = getBinding(category, action);
    return binding.matches(key, ctrl, shift, alt);
}

Keybinding KeybindingsConfig::parseKeyString(const std::string& keyStr) {
    Keybinding result;
    
    std::string str = keyStr;
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    
    // Check modifiers
    result.ctrl = str.find("CTRL+") != std::string::npos;
    result.shift = str.find("SHIFT+") != std::string::npos;
    result.alt = str.find("ALT+") != std::string::npos;
    
    // Extract key name
    std::string keyName = str;
    size_t lastPlus = keyName.rfind('+');
    if (lastPlus != std::string::npos) {
        keyName = keyName.substr(lastPlus + 1);
    }
    
    // Map key name to scancode
    static const std::unordered_map<std::string, SDL_Scancode> keyMap = {
        {"A", SDL_SCANCODE_A}, {"B", SDL_SCANCODE_B}, {"C", SDL_SCANCODE_C},
        {"D", SDL_SCANCODE_D}, {"E", SDL_SCANCODE_E}, {"F", SDL_SCANCODE_F},
        {"G", SDL_SCANCODE_G}, {"H", SDL_SCANCODE_H}, {"I", SDL_SCANCODE_I},
        {"J", SDL_SCANCODE_J}, {"K", SDL_SCANCODE_K}, {"L", SDL_SCANCODE_L},
        {"M", SDL_SCANCODE_M}, {"N", SDL_SCANCODE_N}, {"O", SDL_SCANCODE_O},
        {"P", SDL_SCANCODE_P}, {"Q", SDL_SCANCODE_Q}, {"R", SDL_SCANCODE_R},
        {"S", SDL_SCANCODE_S}, {"T", SDL_SCANCODE_T}, {"U", SDL_SCANCODE_U},
        {"V", SDL_SCANCODE_V}, {"W", SDL_SCANCODE_W}, {"X", SDL_SCANCODE_X},
        {"Y", SDL_SCANCODE_Y}, {"Z", SDL_SCANCODE_Z},
        {"0", SDL_SCANCODE_0}, {"1", SDL_SCANCODE_1}, {"2", SDL_SCANCODE_2},
        {"3", SDL_SCANCODE_3}, {"4", SDL_SCANCODE_4}, {"5", SDL_SCANCODE_5},
        {"6", SDL_SCANCODE_6}, {"7", SDL_SCANCODE_7}, {"8", SDL_SCANCODE_8},
        {"9", SDL_SCANCODE_9},
        {"SPACE", SDL_SCANCODE_SPACE}, {"ESCAPE", SDL_SCANCODE_ESCAPE},
        {"DELETE", SDL_SCANCODE_DELETE}, {"BACKSPACE", SDL_SCANCODE_BACKSPACE},
        {"TAB", SDL_SCANCODE_TAB}, {"RETURN", SDL_SCANCODE_RETURN},
        {"ENTER", SDL_SCANCODE_RETURN}, {"HOME", SDL_SCANCODE_HOME},
        {"END", SDL_SCANCODE_END}, {"PAGEUP", SDL_SCANCODE_PAGEUP},
        {"PAGEDOWN", SDL_SCANCODE_PAGEDOWN},
        {"UP", SDL_SCANCODE_UP}, {"DOWN", SDL_SCANCODE_DOWN},
        {"LEFT", SDL_SCANCODE_LEFT}, {"RIGHT", SDL_SCANCODE_RIGHT},
        {"F1", SDL_SCANCODE_F1}, {"F2", SDL_SCANCODE_F2}, {"F3", SDL_SCANCODE_F3},
        {"F4", SDL_SCANCODE_F4}, {"F5", SDL_SCANCODE_F5}, {"F6", SDL_SCANCODE_F6},
        {"F7", SDL_SCANCODE_F7}, {"F8", SDL_SCANCODE_F8}, {"F9", SDL_SCANCODE_F9},
        {"F10", SDL_SCANCODE_F10}, {"F11", SDL_SCANCODE_F11}, {"F12", SDL_SCANCODE_F12},
        {"PLUS", SDL_SCANCODE_EQUALS}, {"MINUS", SDL_SCANCODE_MINUS}
    };
    
    auto it = keyMap.find(keyName);
    if (it != keyMap.end()) {
        result.key = it->second;
    }
    
    return result;
}

std::string KeybindingsConfig::keyToString(const Keybinding& binding) {
    std::string result;
    
    if (binding.ctrl) result += "Ctrl+";
    if (binding.shift) result += "Shift+";
    if (binding.alt) result += "Alt+";
    
    // Map scancode to key name
    static const std::unordered_map<SDL_Scancode, std::string> scancodeMap = {
        {SDL_SCANCODE_A, "A"}, {SDL_SCANCODE_B, "B"}, {SDL_SCANCODE_C, "C"},
        {SDL_SCANCODE_D, "D"}, {SDL_SCANCODE_E, "E"}, {SDL_SCANCODE_F, "F"},
        {SDL_SCANCODE_G, "G"}, {SDL_SCANCODE_H, "H"}, {SDL_SCANCODE_I, "I"},
        {SDL_SCANCODE_J, "J"}, {SDL_SCANCODE_K, "K"}, {SDL_SCANCODE_L, "L"},
        {SDL_SCANCODE_M, "M"}, {SDL_SCANCODE_N, "N"}, {SDL_SCANCODE_O, "O"},
        {SDL_SCANCODE_P, "P"}, {SDL_SCANCODE_Q, "Q"}, {SDL_SCANCODE_R, "R"},
        {SDL_SCANCODE_S, "S"}, {SDL_SCANCODE_T, "T"}, {SDL_SCANCODE_U, "U"},
        {SDL_SCANCODE_V, "V"}, {SDL_SCANCODE_W, "W"}, {SDL_SCANCODE_X, "X"},
        {SDL_SCANCODE_Y, "Y"}, {SDL_SCANCODE_Z, "Z"},
        {SDL_SCANCODE_SPACE, "Space"}, {SDL_SCANCODE_ESCAPE, "Escape"},
        {SDL_SCANCODE_DELETE, "Delete"}, {SDL_SCANCODE_RETURN, "Return"},
        {SDL_SCANCODE_HOME, "Home"}, {SDL_SCANCODE_END, "End"}
    };
    
    auto it = scancodeMap.find(binding.key);
    if (it != scancodeMap.end()) {
        result += it->second;
    } else {
        result += "Unknown";
    }
    
    return result;
}

} // namespace editor

#endif // HAS_IMGUI
