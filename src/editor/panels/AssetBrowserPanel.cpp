/**
 * @file AssetBrowserPanel.cpp
 * @brief Asset browser implementation
 */
#include "AssetBrowserPanel.h"
#include "editor/EditorContext.h"
#include <filesystem>
#include <algorithm>
#include <SDL.h>
#include <SDL_image.h>

#ifdef HAS_IMGUI
#include <imgui.h>
#endif

namespace fs = std::filesystem;

AssetBrowserPanel::AssetBrowserPanel(EditorContext& context)
    : m_context(context) {
    refreshAssets();
}

AssetBrowserPanel::~AssetBrowserPanel() {
    clearThumbnails();
}

void AssetBrowserPanel::clearThumbnails() {
    for (auto& [path, texture] : m_thumbnailCache) {
        if (texture) {
            SDL_DestroyTexture(texture);
        }
    }
    m_thumbnailCache.clear();
}

void AssetBrowserPanel::loadThumbnail(AssetInfo& asset) {
    if (!m_renderer) return;
    if (asset.thumbnail) return; // Already loaded
    
    // Check cache first
    auto it = m_thumbnailCache.find(asset.path);
    if (it != m_thumbnailCache.end()) {
        asset.thumbnail = it->second;
        if (asset.thumbnail) {
            SDL_QueryTexture(asset.thumbnail, nullptr, nullptr, &asset.thumbWidth, &asset.thumbHeight);
        }
        return;
    }
    
    // Only load images
    std::string ext = fs::path(asset.path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".bmp") {
        return;
    }
    
    // Load texture
    SDL_Texture* texture = IMG_LoadTexture(m_renderer, asset.path.c_str());
    if (texture) {
        SDL_QueryTexture(texture, nullptr, nullptr, &asset.thumbWidth, &asset.thumbHeight);
        asset.thumbnail = texture;
        m_thumbnailCache[asset.path] = texture;
    }
}

void AssetBrowserPanel::refreshAssets() {
    // Don't clear thumbnails - keep cache
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
        
        for (auto& asset : m_assets) {
            // Apply filters
            if (m_filterType != AssetType::All && asset.type != m_filterType) continue;
            if (!m_searchFilter.empty() && 
                asset.name.find(m_searchFilter) == std::string::npos) continue;
            
            // Load thumbnail if needed
            loadThumbnail(asset);
            
            ImGui::PushID(index);
            
            // Asset button
            bool selected = (m_selectedAsset == index);
            if (selected) {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.5f, 0.8f, 1.0f));
            }
            
            ImVec2 buttonPos = ImGui::GetCursorScreenPos();
            
            if (ImGui::Button("##asset", ImVec2(cellSize, cellSize))) {
                m_selectedAsset = index;
                if (m_onAssetSelected) {
                    m_onAssetSelected(asset);
                }
            }
            
            // Drag-and-drop source for sprites and backgrounds
            if ((asset.type == AssetType::Sprites || asset.type == AssetType::Backgrounds) &&
                ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
                // Store the asset path as payload
                ImGui::SetDragDropPayload("ASSET_PATH", asset.path.c_str(), asset.path.size() + 1);
                
                // Show preview while dragging
                if (asset.thumbnail) {
                    ImGui::Image((ImTextureID)(intptr_t)asset.thumbnail, ImVec2(48, 48));
                }
                ImGui::Text("%s", asset.name.c_str());
                ImGui::EndDragDropSource();
            }
            
            if (selected) {
                ImGui::PopStyleColor();
            }
            
            // Draw thumbnail or icon
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            
            if (asset.thumbnail) {
                // Calculate scaled size maintaining aspect ratio
                float scale = std::min((cellSize - 10) / asset.thumbWidth, (cellSize - 25) / asset.thumbHeight);
                float drawW = asset.thumbWidth * scale;
                float drawH = asset.thumbHeight * scale;
                float offsetX = (cellSize - drawW) / 2;
                float offsetY = (cellSize - 25 - drawH) / 2;
                
                ImVec2 uvMin(0, 0);
                ImVec2 uvMax(1, 1);
                drawList->AddImage(
                    (ImTextureID)(intptr_t)asset.thumbnail,
                    ImVec2(buttonPos.x + offsetX, buttonPos.y + offsetY),
                    ImVec2(buttonPos.x + offsetX + drawW, buttonPos.y + offsetY + drawH),
                    uvMin, uvMax);
            } else {
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
                drawList->AddText(
                    ImVec2(buttonPos.x + 5, buttonPos.y + 5), 
                    IM_COL32(200, 200, 200, 255), icon);
            }
            
            // Truncate name
            std::string displayName = asset.name;
            if (displayName.length() > 10) {
                displayName = displayName.substr(0, 8) + "..";
            }
            drawList->AddText(
                ImVec2(buttonPos.x + 5, buttonPos.y + cellSize - 18), 
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
