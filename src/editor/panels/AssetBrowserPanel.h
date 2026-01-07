/**
 * @file AssetBrowserPanel.h
 * @brief Asset browser för sprites, sounds, backgrounds etc.
 */
#pragma once

#include "editor/IEditorPanel.h"
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>

struct SDL_Texture;
struct SDL_Renderer;
class EditorContext;

/**
 * @brief Asset typer
 */
enum class AssetType {
    All,
    Sprites,
    Backgrounds,
    Sounds,
    Music,
    Fonts
};

/**
 * @brief Asset info
 */
struct AssetInfo {
    std::string name;
    std::string path;
    AssetType type;
    size_t fileSize = 0;
    SDL_Texture* thumbnail = nullptr;
    int thumbWidth = 0;
    int thumbHeight = 0;
};

/**
 * @brief Browser för game assets
 */
class AssetBrowserPanel : public IEditorPanel {
public:
    explicit AssetBrowserPanel(EditorContext& context);
    ~AssetBrowserPanel();
    
    void setRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
    // Callbacks
    using AssetSelectedCallback = std::function<void(const AssetInfo&)>;
    void setOnAssetSelected(AssetSelectedCallback cb) { m_onAssetSelected = cb; }
    
    // Refresh asset list
    void refreshAssets();
    
private:
    void scanDirectory(const std::string& path, AssetType type);
    void renderAssetGrid();
    void renderAssetList();
    void loadThumbnail(AssetInfo& asset);
    void clearThumbnails();
    
    EditorContext& m_context;
    std::string m_id = "assetbrowser";
    std::string m_title = "Asset Browser";
    
    std::vector<AssetInfo> m_assets;
    AssetType m_filterType = AssetType::All;
    std::string m_searchFilter;
    bool m_gridView = true;
    int m_selectedAsset = -1;
    
    AssetSelectedCallback m_onAssetSelected;
    
    SDL_Renderer* m_renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> m_thumbnailCache;
};
