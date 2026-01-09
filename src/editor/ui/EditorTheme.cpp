/**
 * @file EditorTheme.cpp
 * @brief ImGui theme manager implementation
 */
#include "EditorTheme.h"

namespace editor {

// Static font pointers
ImFont* EditorTheme::s_defaultFont = nullptr;
ImFont* EditorTheme::s_boldFont = nullptr;
ImFont* EditorTheme::s_monoFont = nullptr;
ImFont* EditorTheme::s_iconFont = nullptr;

void EditorTheme::Apply(Theme theme) {
    switch (theme) {
        case Theme::RetroEngine: ApplyRetroEngineTheme(); break;
        case Theme::RetroLight: ApplyRetroLightTheme(); break;
        case Theme::RetroDark: ApplyRetroDarkTheme(); break;
        case Theme::Classic: ApplyClassicTheme(); break;
    }
}

void EditorTheme::ApplyRetroEngineTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    // === ROUNDING (Modern, smooth) ===
    style.WindowRounding = 8.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    
    // === BORDERS (Subtle) ===
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    
    // === SPACING (More breathing room) ===
    style.WindowPadding = ImVec2(12, 12);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.IndentSpacing = 24.0f;
    style.ScrollbarSize = 16.0f;
    style.GrabMinSize = 14.0f;
    
    // === COLORS (Modern, clean) ===
    ImVec4* colors = style.Colors;
    
    // Text
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.55f, 0.36f, 0.96f, 0.35f);
    
    // Backgrounds (Darker, cleaner)
    colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.12f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.12f, 0.12f, 0.13f, 0.98f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
    
    // Borders (Subtle)
    colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.22f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    
    // Frames (Modern, subtle)
    colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.25f, 0.25f, 0.27f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.28f, 0.30f, 1.00f);
    
    // Title bars (Cleaner)
    colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.12f, 0.12f, 0.13f, 0.75f);
    
    // Scrollbars (Smoother)
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.14f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30f, 0.30f, 0.32f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.55f, 0.36f, 0.96f, 0.80f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    
    // Checkboxes & Sliders
    colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.55f, 0.36f, 0.96f, 0.78f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.66f, 0.55f, 0.98f, 1.00f);
    
    // Buttons
    colors[ImGuiCol_Button] = ImVec4(0.55f, 0.36f, 0.96f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.36f, 0.96f, 0.70f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    
    // Headers
    colors[ImGuiCol_Header] = ImVec4(0.55f, 0.36f, 0.96f, 0.50f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.55f, 0.36f, 0.96f, 0.70f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    
    // Separators
    colors[ImGuiCol_Separator] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.55f, 0.36f, 0.96f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    
    // Resize Grips
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.55f, 0.36f, 0.96f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.55f, 0.36f, 0.96f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.55f, 0.36f, 0.96f, 0.95f);
    
    // Tabs
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.55f, 0.36f, 0.96f, 0.80f);
    colors[ImGuiCol_TabSelected] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    colors[ImGuiCol_TabDimmed] = ImVec4(0.12f, 0.12f, 0.13f, 1.00f);
    colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.35f, 0.23f, 0.61f, 1.00f);
    
    // Plots
    colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.66f, 0.55f, 0.98f, 1.00f);
    
    // Tables
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.18f, 0.18f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
    
    // Drag & Drop
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.55f, 0.36f, 0.96f, 0.90f);
    
    // Navigation
    colors[ImGuiCol_NavHighlight] = ImVec4(0.55f, 0.36f, 0.96f, 1.00f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    
    // Modal
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.08f, 0.08f, 0.09f, 0.75f);
}

void EditorTheme::ApplyRetroLightTheme() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    style.WindowRounding = 4.0f;
    style.ChildRounding = 4.0f;
    style.FrameRounding = 2.0f;
    style.PopupRounding = 2.0f;
    style.ScrollbarRounding = 4.0f;
    style.GrabRounding = 2.0f;
    style.TabRounding = 2.0f;
    
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;
    
    style.WindowPadding = ImVec2(8, 8);
    style.FramePadding = ImVec2(6, 4);
    style.ItemSpacing = ImVec2(8, 4);
    style.ItemInnerSpacing = ImVec2(4, 4);
    style.IndentSpacing = 20.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 10.0f;
    
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.11f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.31f, 0.80f, 0.77f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.31f, 0.80f, 0.77f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.31f, 0.80f, 0.77f, 0.70f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.31f, 0.80f, 0.77f, 1.00f);
}

void EditorTheme::ApplyRetroDarkTheme() {
    // Extra dark variant of Retro Engine theme
    ApplyRetroEngineTheme();
    
    // Make it even darker
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.03f, 0.03f, 0.04f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.06f, 0.06f, 0.07f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.04f, 0.04f, 0.05f, 0.98f);
}


void EditorTheme::ApplyClassicTheme() {
    ImGui::StyleColorsClassic();
}

bool EditorTheme::LoadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    
    // Load default font (Roboto or fallback to default)
    // TODO: Add actual font files to assets/fonts/
    s_defaultFont = io.Fonts->AddFontDefault();
    
    // For now, use default font for all
    s_boldFont = s_defaultFont;
    s_monoFont = s_defaultFont;
    s_iconFont = s_defaultFont;
    
    return true;
}

ImFont* EditorTheme::GetDefaultFont() {
    return s_defaultFont;
}

ImFont* EditorTheme::GetBoldFont() {
    return s_boldFont;
}

ImFont* EditorTheme::GetMonoFont() {
    return s_monoFont;
}

ImFont* EditorTheme::GetIconFont() {
    return s_iconFont;
}

} // namespace editor
