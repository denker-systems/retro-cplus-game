/**
 * @file PropertyEditorUtils.cpp
 * @brief Implementation av property editor helpers
 */
#include "PropertyEditorUtils.h"
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace PropertyEditorUtils {

bool InputText(const char* label, std::string& str, ImGuiInputTextFlags flags) {
#ifdef HAS_IMGUI
    char buffer[1024];
    strncpy_s(buffer, str.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    if (ImGui::InputText(label, buffer, sizeof(buffer), flags)) {
        str = buffer;
        return true;
    }
#endif
    return false;
}

bool InputTextMultiline(const char* label, std::string& str, const ImVec2& size, ImGuiInputTextFlags flags) {
#ifdef HAS_IMGUI
    char buffer[4096];
    strncpy_s(buffer, str.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    if (ImGui::InputTextMultiline(label, buffer, sizeof(buffer), size, flags)) {
        str = buffer;
        return true;
    }
#endif
    return false;
}

template<typename EnumType>
bool EnumCombo(const char* label, EnumType& value, const std::vector<std::pair<EnumType, const char*>>& items) {
#ifdef HAS_IMGUI
    // Hitta nuvarande index
    int currentIdx = 0;
    for (size_t i = 0; i < items.size(); i++) {
        if (items[i].first == value) {
            currentIdx = static_cast<int>(i);
            break;
        }
    }
    
    // Hitta nuvarande display name
    const char* currentName = items[currentIdx].second;
    
    bool changed = false;
    if (ImGui::BeginCombo(label, currentName)) {
        for (size_t i = 0; i < items.size(); i++) {
            bool isSelected = (static_cast<int>(i) == currentIdx);
            if (ImGui::Selectable(items[i].second, isSelected)) {
                value = items[i].first;
                changed = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return changed;
#else
    return false;
#endif
}

bool FileBrowser(const char* label, std::string& path, const char* filter) {
#ifdef HAS_IMGUI
    bool changed = false;
    
    // Visa nuvarande path
    ImGui::Text("%s", label);
    ImGui::SameLine();
    
    char buffer[512];
    strncpy_s(buffer, path.c_str(), sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    
    ImGui::PushItemWidth(-100);
    if (ImGui::InputText("##path", buffer, sizeof(buffer))) {
        path = buffer;
        changed = true;
    }
    ImGui::PopItemWidth();
    
    ImGui::SameLine();
    if (ImGui::Button("Browse...")) {
        // TODO: Implementera native file dialog när nativefiledialog-extended är installerat
        // För nu: bara visa meddelande
        ImGui::OpenPopup("FileBrowserNotImplemented");
    }
    
    if (ImGui::BeginPopupModal("FileBrowserNotImplemented", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("File browser not yet implemented.");
        ImGui::Text("Please type the path manually.");
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    
    return changed;
#else
    return false;
#endif
}

void HelpMarker(const char* text) {
#ifdef HAS_IMGUI
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(text);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
#endif
}

void SectionHeader(const char* text) {
#ifdef HAS_IMGUI
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Text("%s", text);
    ImGui::Separator();
    ImGui::Spacing();
#endif
}

bool IdCombo(const char* label, std::string& currentId, const std::vector<std::pair<std::string, std::string>>& items) {
#ifdef HAS_IMGUI
    // Hitta display name för current ID
    std::string displayName = currentId.empty() ? "(None)" : currentId;
    for (const auto& item : items) {
        if (item.first == currentId) {
            displayName = item.second;
            break;
        }
    }
    
    bool changed = false;
    if (ImGui::BeginCombo(label, displayName.c_str())) {
        // None option
        bool isSelected = currentId.empty();
        if (ImGui::Selectable("(None)", isSelected)) {
            currentId.clear();
            changed = true;
        }
        if (isSelected) {
            ImGui::SetItemDefaultFocus();
        }
        
        // Lista alla items
        for (const auto& item : items) {
            isSelected = (currentId == item.first);
            if (ImGui::Selectable(item.second.c_str(), isSelected)) {
                currentId = item.first;
                changed = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    return changed;
#else
    return false;
#endif
}

} // namespace PropertyEditorUtils
