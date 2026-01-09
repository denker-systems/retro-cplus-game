# Research: ImGui Styling & Tema-anpassning

## Datum
2026-01-09

## Frågeställning
Hur kan vi anpassa hela vårt ImGui-tema för att göra Retro Engine-editorn snyggare och mer professionell?

---

## Sammanfattning

ImGui erbjuder omfattande styling-möjligheter genom:
1. **60+ färgkomponenter** (ImGuiCol_)
2. **30+ style variables** (ImGuiStyleVar_)
3. **Font-system** (TTF/OTF support)
4. **3 inbyggda teman** (Dark, Light, Classic)

---

## Källor

### Dokumentation
- [ImGui Official](https://github.com/ocornut/imgui) - Context7: `/ocornut/imgui`
- [ImGui Issue #707](https://github.com/ocornut/imgui/issues/707) - Community themes
- [ImGui FONTS.md](https://github.com/ocornut/imgui/blob/master/docs/FONTS.md) - Font guide

### Kod-exempel
- [GraphicsProgramming/dear-imgui-styles](https://github.com/GraphicsProgramming/dear-imgui-styles)
- [Adobe Spectrum Theme](https://github.com/adobe/imgui)
- [Cinder-ImGui Dark Theme](https://github.com/simongeilfus/Cinder-ImGui)

---

## Styling-system

### 1. Färger (60+ komponenter)

**Huvudkategorier:**
- Text, Backgrounds, Borders, Frames
- Title bars, Scrollbars, Buttons
- Headers, Tabs, Tables, Sliders
- Checkboxes, Plots, Navigation

**Användning:**
```cpp
// Global
ImGuiStyle& style = ImGui::GetStyle();
style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 0.9f);

// Temporärt
ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
ImGui::Button("Röd");
ImGui::PopStyleColor();
```

### 2. Style Variables (30+ variabler)

**Huvudkategorier:**
- Rounding (WindowRounding, FrameRounding, etc.)
- Borders (WindowBorderSize, FrameBorderSize, etc.)
- Spacing (ItemSpacing, ItemInnerSpacing, etc.)
- Padding (WindowPadding, FramePadding, etc.)

**Användning:**
```cpp
// Global
style.WindowRounding = 5.0f;
style.FramePadding = ImVec2(4, 2);

// Temporärt
ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
ImGui::Button("Rundad");
ImGui::PopStyleVar();
```

### 3. Fonts

**Ladda fonts:**
```cpp
ImGuiIO& io = ImGui::GetIO();
ImFont* font1 = io.Fonts->AddFontFromFileTTF("Roboto-Regular.ttf");
ImFont* font2 = io.Fonts->AddFontFromFileTTF("Roboto-Bold.ttf");
```

**Använda fonts:**
```cpp
ImGui::PushFont(font2);
ImGui::Text("Bold text");
ImGui::PopFont();
```

---

## Professionella Teman

### 1. Dark Theme (Cinder-ImGui)
- Background: `#212229`
- Accent: `#EB2E4D` (röd)
- Mjuka avrundningar (4px windows, 2px frames)

### 2. Adobe Spectrum
- Light theme
- Background: `#F5F5F5`
- Accent: `#378EF0` (blå)
- Professionell design system

### 3. Unreal Engine Style
- Background: `#1E1E1E`
- Accent: `#007ACC` (blå)
- Inga avrundningar (0px)
- Minimalistisk

---

## Rekommendation för Retro Engine

### Valt Tema: Hybrid Dark Theme

**Färgpalett:**
| Element | Hex | Användning |
|---------|-----|------------|
| Primary BG | `#1A1A1D` | Huvudfönster |
| Secondary BG | `#252529` | Panels |
| Frame BG | `#2D2D32` | Inputs |
| Accent | `#4ECDC4` | Headers, aktiva |
| Hover | `#FF6B6B` | Hover states |
| Text | `#FFFFFF` | Normal text |
| Text Disabled | `#B8B8B8` | Disabled |
| Border | `#3A3A3F` | Ramar |

**Design Principer:**
1. Minimalistisk
2. Funktionell hierarki
3. Professionell (likt Unreal/Unity)
4. Hög kontrast för läsbarhet
5. Konsekvent accent-färg

---

## Implementation Plan

### Steg 1: Skapa EditorTheme.h/cpp
```
src/editor/ui/EditorTheme.h
src/editor/ui/EditorTheme.cpp
```

### Steg 2: Theme Manager
```cpp
class EditorTheme {
public:
    enum class Theme { Dark, Light, Unreal, Classic };
    static void Apply(Theme theme = Theme::Dark);
    static void LoadFonts();
};
```

### Steg 3: Ladda Fonts
Rekommenderade fonts:
- **Roboto** - Default UI font
- **Roboto Mono** - Code/console
- **FontAwesome** - Icons

### Steg 4: Applicera i ImGuiManager
```cpp
// ImGuiManager::init()
EditorTheme::LoadFonts();
EditorTheme::Apply(EditorTheme::Theme::Dark);
```

---

## Nästa Steg

1. Skapa `EditorTheme.h/cpp`
2. Implementera `ApplyDarkTheme()`
3. Ladda Roboto fonts
4. Integrera i `ImGuiManager`
5. Testa och justera färger

---

## Referenser

- ImGui Styling Docs: https://github.com/ocornut/imgui/blob/master/imgui.h
- Font Loading: https://github.com/ocornut/imgui/blob/master/docs/FONTS.md
- Community Themes: https://github.com/ocornut/imgui/issues/707
