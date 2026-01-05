/**
 * @file AssetBrowserPanel.h
 * @brief Asset browser för sprites, sounds, backgrounds etc.
 */
#pragma once

#include "../IEditorPanel.h"
#include <string>
#include <vector>
#include <functional>

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
};

/**
 * @brief Browser för game assets
 */
class AssetBrowserPanel : public IEditorPanel {
public:
    explicit AssetBrowserPanel(EditorContext& context);
    
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
    
    EditorContext& m_context;
    std::string m_id = "assetbrowser";
    std::string m_title = "Asset Browser";
    
    std::vector<AssetInfo> m_assets;
    AssetType m_filterType = AssetType::All;
    std::string m_searchFilter;
    bool m_gridView = true;
    int m_selectedAsset = -1;
    
    AssetSelectedCallback m_onAssetSelected;
};
