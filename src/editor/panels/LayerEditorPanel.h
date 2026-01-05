/**
 * @file LayerEditorPanel.h
 * @brief Layer management panel
 */
#pragma once

#include "editor/IEditorPanel.h"
#include "world/LayerManager.h"
#include <vector>

class EditorContext;

/**
 * @brief Panel for managing scene layers
 */
class LayerEditorPanel : public IEditorPanel {
public:
    explicit LayerEditorPanel(EditorContext& context);
    ~LayerEditorPanel() override = default;
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    void render() override;
    
    /** @brief Set layer manager to edit */
    void setLayerManager(engine::LayerManager* manager) { m_layerManager = manager; }
    
    /** @brief Enable parallax preview mode */
    void setParallaxPreview(bool enable) { m_parallaxPreview = enable; }
    
private:
    void renderLayerList();
    void renderLayerProperties();
    void renderAddLayerDialog();
    void renderParallaxPreview();
    
    EditorContext& m_context;
    std::string m_id = "LayerEditor";
    std::string m_title = "Layer Editor";
    
    engine::LayerManager* m_layerManager = nullptr;
    engine::Layer* m_selectedLayer = nullptr;
    
    bool m_showAddDialog = false;
    char m_newLayerName[64] = "";
    int m_newLayerType = 0;  // LayerType enum
    int m_newLayerZIndex = 0;
    
    bool m_parallaxPreview = false;
    float m_previewScroll = 0.0f;
};
