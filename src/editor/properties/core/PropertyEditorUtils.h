/**
 * @file PropertyEditorUtils.h
 * @brief Helper-funktioner för property editors
 */
#pragma once

#include <string>
#include <vector>
#include <utility>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace PropertyEditorUtils {

/**
 * @brief InputText för std::string
 * @param label Label för input field
 * @param str String att editera
 * @param flags ImGui flags
 * @return true om värdet ändrades
 */
bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags = 0);

/**
 * @brief InputText för std::string med disabled state
 * @param label Label för input field
 * @param str String att editera
 * @param disabled Om input är disabled
 * @return true om värdet ändrades
 */
bool InputTextDisabled(const char* label, std::string& str, bool disabled = false);

/**
 * @brief InputTextMultiline för std::string
 * @param label Label för textarea
 * @param str String att editera
 * @param size Storlek på textarea
 * @param flags ImGui flags
 * @return true om värdet ändrades
 */
bool InputTextMultiline(const char* label, std::string& str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0);

/**
 * @brief Enum dropdown
 * @tparam EnumType Enum-typ
 * @param label Label för dropdown
 * @param value Nuvarande värde
 * @param items Lista med (enum_value, display_name) par
 * @return true om värdet ändrades
 */
template<typename EnumType>
bool EnumCombo(const char* label, EnumType& value, const std::vector<std::pair<EnumType, const char*>>& items);

/**
 * @brief File browser button
 * @param label Label för button
 * @param path Sökväg att editera
 * @param filter File filter (t.ex. "*.png")
 * @return true om sökväg ändrades
 */
bool FileBrowser(const char* label, std::string& path, const char* filter = nullptr);

/**
 * @brief Hjälptext med (?) ikon
 * @param text Hjälptext att visa
 */
void HelpMarker(const char* text);

/**
 * @brief Section header med separator
 * @param text Header text
 */
void SectionHeader(const char* text);

/**
 * @brief Dropdown för ID selection från lista
 * @param label Label för dropdown
 * @param currentId Nuvarande valt ID
 * @param items Lista med (id, display_name) par
 * @return true om värdet ändrades
 */
bool IdCombo(const char* label, std::string& currentId, const std::vector<std::pair<std::string, std::string>>& items);

} // namespace PropertyEditorUtils
