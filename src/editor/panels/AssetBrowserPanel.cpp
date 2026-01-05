/**
 * @file AssetBrowserPanel.cpp
 * @brief Asset browser implementation
 */
#include "AssetBrowserPanel.h"
#include "editor/EditorContext.h"
#include <filesystem>
#include <algorithm>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace fs = std::filesystem;

AssetBrowserPanel::AssetBrowserPanel(EditorContext& context)
    : m_context(context) {
    refreshAssets();
}

void AssetBrowserPanel::refreshAssets() {
    m_assets.clear();
    
    // Scan asset directories
    scanDirectory("assets/sprites", AssetType::Sprites);
    scanDirectory("assets/backgrounds", AssetType::Backgrounds);
    scanDirectory("assets/sounds", AssetType::Sounds);
    scanDirectory("assets/music", AssetType::Music);
    scanDirectory("assets/fonts", AssetType::Fonts);
    
    // Sort by name
    std::sort(m_assets.begin(), m_assets.end(), 
        [](const AssetInfo& a, const AssetInfo& b) { return a.name < b.name; });
}

void AssetBrowserPanel::scanDirectory(const std::string& path, AssetType type) {
    if (!fs::exists(path)) return;
    
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.is_regular_file()) {
            AssetInfo info;
            info.name = entry.path().filename().string();
            info.path = entry.path().string();
            info.type = type;
            info.fileSize = entry.file_size();
            m_assets.push_back(info);
        }
    }
}

void AssetBrowserPanel::render() {
#ifdef HAS_IMGUI
    if (!m_visible) return;
    
    if (ImGui::Begin(m_title.c_str(), &m_visible)) {
        // Toolbar
        if (ImGui::Button("Refresh")) {
            refreshAssets();
        }
        ImGui::SameLine();
        
        // View toggle
        if (ImGui::Button(m_gridView ? "List" : "Grid")) {
            m_gridView = !m_gridView;
        }
        ImGui::SameLine();
        
        // Filter dropdown
        const char* filterNames[] = { "All", "Sprites", "Backgrounds", "Sounds", "Music", "Fonts" };
        int currentFilter = static_cast<int>(m_filterType);
        ImGui::SetNextItemWidth(100);
        if (ImGui::Combo("##filter", &currentFilter, filterNames, 6)) {
            m_filterType = static_cast<AssetType>(currentFilter);
        }
        ImGui::SameLine();
        
        // Search
        char searchBuf[256] = {0};
        strncpy(searchBuf, m_searchFilter.c_str(), sizeof(searchBuf) - 1);
        ImGui::SetNextItemWidth(150);
        if (ImGui::InputText("##search", searchBuf, sizeof(searchBuf))) {
            m_searchFilter = searchBuf;
        }
        
        ImGui::Separator();
        
        // Asset count
        int visibleCount = 0;
        for (const auto& asset : m_assets) {
            if (m_filterType != AssetType::All && asset.type != m_filterType) continue;
            if (!m_searchFilter.empty() && 
                asset.name.find(m_searchFilter) == std::string::npos) continue;
            visibleCount++;
        }
        ImGui::Text("Assets: %d", visibleCount);
        
        ImGui::Separator();
        
        // Content
        if (m_gridView) {
            renderAssetGrid();
        } else {
            renderAssetList();
        }
    }
    ImGui::End();
#endif
}

void AssetBrowserPanel::renderAssetGrid() {
#ifdef HAS_IMGUI
    float cellSize = 80.0f;
    float padding = 8.0f;
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columns = std::max(1, (int)(panelWidth / (cellSize + padding)));
    
    if (ImGui::BeginChild("AssetGrid")) {
        int col = 0;
        int index = 0;
        
        for (const auto& asset : m_assets) {
            // Apply filters
            if (m_filterType != AssetType::All && asset.type != m_filterType) continue;
            if (!m_searchFilter.empty() && 
                asset.name.find(m_searchFilter) == std::string::npos) continue;
            
            ImGui::PushID(index);
            
            // Asset button
            bool selected = (m_selectedAsset == index);
            if (selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
            }
            
            if (ImGui::Button("##asset", ImVec2(cellSize, cellSize))) {
                m_selectedAsset = index;
                if (m_onAssetSelected) {
                    m_onAssetSelected(asset);
                }
            }
            
            if (selected) {
                ImGui::PopStyleColor();
            }
            
            // Icon based on type
            const char* icon = "?";
            switch (asset.type) {
                case AssetType::Sprites: icon = "[S]"; break;
                case AssetType::Backgrounds: icon = "[B]"; break;
                case AssetType::Sounds: icon = "[SFX]"; break;
                case AssetType::Music: icon = "[M]"; break;
                case AssetType::Fonts: icon = "[F]"; break;
                default: break;
            }
            
            // Draw icon and name
            ImVec2 pos = ImGui::GetItemRectMin();
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(pos.x + 5, pos.y + 5), 
                IM_COL32(200, 200, 200, 255), icon);
            
            // Truncate name
            std::string displayName = asset.name;
            if (displayName.length() > 10) {
                displayName = displayName.substr(0, 8) + "..";
            }
            ImGui::GetWindowDrawList()->AddText(
                ImVec2(pos.x + 5, pos.y + cellSize - 20), 
                IM_COL32(255, 255, 255, 255), displayName.c_str());
            
            // Tooltip
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s\n%s\nSize: %zu bytes", 
                    asset.name.c_str(), asset.path.c_str(), asset.fileSize);
            }
            
            ImGui::PopID();
            
            col++;
            if (col < columns) {
                ImGui::SameLine();
            } else {
                col = 0;
            }
            
            index++;
        }
    }
    ImGui::EndChild();
#endif
}

void AssetBrowserPanel::renderAssetList() {
#ifdef HAS_IMGUI
    if (ImGui::BeginChild("AssetList")) {
        if (ImGui::BeginTable("assets", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
            ImGui::TableSetupColumn("Name");
            ImGui::TableSetupColumn("Type");
            ImGui::TableSetupColumn("Size");
            ImGui::TableSetupColumn("Path");
            ImGui::TableHeadersRow();
            
            int index = 0;
            for (const auto& asset : m_assets) {
                // Apply filters
                if (m_filterType != AssetType::All && asset.type != m_filterType) continue;
                if (!m_searchFilter.empty() && 
                    asset.name.find(m_searchFilter) == std::string::npos) continue;
                
                ImGui::TableNextRow();
                
                ImGui::TableNextColumn();
                bool selected = (m_selectedAsset == index);
                if (ImGui::Selectable(asset.name.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns)) {
                    m_selectedAsset = index;
                    if (m_onAssetSelected) {
                        m_onAssetSelected(asset);
                    }
                }
                
                ImGui::TableNextColumn();
                const char* typeNames[] = { "All", "Sprite", "Background", "Sound", "Music", "Font" };
                ImGui::Text("%s", typeNames[static_cast<int>(asset.type)]);
                
                ImGui::TableNextColumn();
                if (asset.fileSize < 1024) {
                    ImGui::Text("%zu B", asset.fileSize);
                } else if (asset.fileSize < 1024 * 1024) {
                    ImGui::Text("%.1f KB", asset.fileSize / 1024.0f);
                } else {
                    ImGui::Text("%.1f MB", asset.fileSize / (1024.0f * 1024.0f));
                }
                
                ImGui::TableNextColumn();
                ImGui::Text("%s", asset.path.c_str());
                
                index++;
            }
            
            ImGui::EndTable();
        }
    }
    ImGui::EndChild();
#endif
}
