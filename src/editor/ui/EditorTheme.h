/**
 * @file EditorTheme.h
 * @brief ImGui theme manager for Retro Engine editor
 */
#pragma once

#include <imgui.h>
#include <string>

namespace editor {

/**
 * @class EditorTheme
 * @brief Manages ImGui styling and themes for the editor
 */
class EditorTheme {
public:
    enum class Theme {
        RetroEngine,    // Retro Engine branded theme (default)
        RetroLight,     // Retro Engine light variant
        RetroDark,      // Retro Engine dark variant
        Classic         // ImGui classic fallback
    };
    
    /**
     * @brief Apply a theme to ImGui
     * @param theme Theme to apply
     */
    static void Apply(Theme theme = Theme::RetroEngine);
    
    /**
     * @brief Apply Retro Engine branded theme
     */
    static void ApplyRetroEngineTheme();
    
    /**
     * @brief Apply Retro Engine light variant
     */
    static void ApplyRetroLightTheme();
    
    /**
     * @brief Apply Retro Engine dark variant
     */
    static void ApplyRetroDarkTheme();
    
    
    /**
     * @brief Apply Classic ImGui theme
     */
    static void ApplyClassicTheme();
    
    /**
     * @brief Load custom fonts for the editor
     * @return True if fonts loaded successfully
     */
    static bool LoadFonts();
    
    /**
     * @brief Get default UI font
     */
    static ImFont* GetDefaultFont();
    
    /**
     * @brief Get bold UI font
     */
    static ImFont* GetBoldFont();
    
    /**
     * @brief Get monospace font (for code/console)
     */
    static ImFont* GetMonoFont();
    
    /**
     * @brief Get icon font (FontAwesome)
     */
    static ImFont* GetIconFont();
    
    // Retro Engine Brand Colors
    struct BrandColors {
        static constexpr ImVec4 Primary = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);      // #8B5CF6 - Purple
        static constexpr ImVec4 PrimaryHover = ImVec4(0.66f, 0.55f, 0.98f, 1.00f); // #A78BFA - Light Purple
        static constexpr ImVec4 PrimaryActive = ImVec4(0.45f, 0.26f, 0.86f, 1.00f);// #7342DB - Dark Purple
        static constexpr ImVec4 Secondary = ImVec4(0.31f, 0.80f, 0.77f, 1.00f);    // #4ECDC4 - Cyan
        static constexpr ImVec4 Accent = ImVec4(1.00f, 0.42f, 0.42f, 1.00f);       // #FF6B6B - Coral
        static constexpr ImVec4 Success = ImVec4(0.06f, 0.73f, 0.51f, 1.00f);      // #10B981 - Green
        static constexpr ImVec4 Warning = ImVec4(0.96f, 0.62f, 0.04f, 1.00f);      // #F59E0B - Orange
        static constexpr ImVec4 Error = ImVec4(0.94f, 0.27f, 0.27f, 1.00f);        // #EF4444 - Red
        static constexpr ImVec4 Info = ImVec4(0.22f, 0.56f, 0.94f, 1.00f);         // #378EF0 - Blue
    };
    
private:
    static ImFont* s_defaultFont;
    static ImFont* s_boldFont;
    static ImFont* s_monoFont;
    static ImFont* s_iconFont;
};

} // namespace editor
