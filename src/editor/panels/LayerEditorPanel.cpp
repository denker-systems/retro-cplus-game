/**
 * @file LayerEditorPanel.cpp
 * @brief Implementation of LayerEditorPanel
 */
#include "LayerEditorPanel.h"
#include "editor/EditorContext.h"
#include <imgui.h>
#include <cstring>
#include <cmath>

LayerEditorPanel::LayerEditorPanel(EditorContext& context)
    : m_context(context) {}

void LayerEditorPanel::render() {
    if (!m_visible) return;
    
    if (!ImGui::Begin(m_title.c_str(), &m_visible)) {
        ImGui::End();
        return;
    }
    
    if (!m_layerManager) {
        ImGui::TextDisabled("No layer manager loaded");
        ImGui::End();
        return;
    }
    
    // Toolbar
    if (ImGui::Button("Add Layer")) {
        m_showAddDialog = true;
        std::memset(m_newLayerName, 0, sizeof(m_newLayerName));
        m_newLayerType = 0;
        m_newLayerZIndex = 0;
    }
    
    ImGui::Separator();
    
    // Two columns: layer list and properties
    ImGui::Columns(2);
    
    renderLayerList();
    
    ImGui::NextColumn();
    
    renderLayerProperties();
    
    ImGui::Columns(1);
    
    // Add layer dialog
    if (m_showAddDialog) {
        renderAddLayerDialog();
    }
    
    ImGui::End();
}

void LayerEditorPanel::renderLayerList() {
    ImGui::Text("Layers");
    ImGui::Separator();
    
    const auto& layers = m_layerManager->getLayers();
    
    for (const auto& layer : layers) {
        if (!layer) continue;
        
        bool isSelected = (layer.get() == m_selectedLayer);
        
        // Layer type icon
        const char* icon = "L";
        switch (layer->getLayerType()) {
            case engine::LayerType::Background: icon = "B"; break;
            case engine::LayerType::Main: icon = "M"; break;
            case engine::LayerType::Foreground: icon = "F"; break;
            case engine::LayerType::UI: icon = "U"; break;
        }
        
        std::string label = std::string(icon) + " " + layer->getName() + 
                           " (z:" + std::to_string(layer->getZIndex()) + ")";
        
        if (ImGui::Selectable(label.c_str(), isSelected)) {
            m_selectedLayer = layer.get();
        }
    }
}

void LayerEditorPanel::renderLayerProperties() {
    ImGui::Text("Properties");
    ImGui::Separator();
    
    if (!m_selectedLayer) {
        ImGui::TextDisabled("No layer selected");
        return;
    }
    
    // Name
    ImGui::Text("Name: %s", m_selectedLayer->getName().c_str());
    
    // Type
    const char* typeNames[] = {"Background", "Main", "Foreground", "UI"};
    int typeIndex = static_cast<int>(m_selectedLayer->getLayerType());
    ImGui::Text("Type: %s", typeNames[typeIndex]);
    
    // Z-Index
    int zIndex = m_selectedLayer->getZIndex();
    if (ImGui::DragInt("Z-Index", &zIndex)) {
        m_selectedLayer->setZIndex(zIndex);
    }
    
    // Opacity
    float opacity = m_selectedLayer->getOpacity();
    if (ImGui::SliderFloat("Opacity", &opacity, 0.0f, 1.0f)) {
        m_selectedLayer->setOpacity(opacity);
    }
    
    // Active/Visible
    bool active = m_selectedLayer->isActive();
    if (ImGui::Checkbox("Active", &active)) {
        m_selectedLayer->setActive(active);
    }
    
    bool visible = m_selectedLayer->isVisible();
    if (ImGui::Checkbox("Visible", &visible)) {
        m_selectedLayer->setVisible(visible);
    }
    
    ImGui::Separator();
    
    // Delete button
    if (ImGui::Button("Delete Layer")) {
        m_layerManager->removeLayer(m_selectedLayer->getName());
        m_selectedLayer = nullptr;
    }
    
    ImGui::Separator();
    
    // Parallax preview
    if (m_selectedLayer->getLayerType() == engine::LayerType::Background) {
        ImGui::Checkbox("Parallax Preview", &m_parallaxPreview);
        
        if (m_parallaxPreview) {
            renderParallaxPreview();
        }
    }
}

void LayerEditorPanel::renderAddLayerDialog() {
    ImGui::OpenPopup("Add Layer");
    
    if (ImGui::BeginPopupModal("Add Layer", &m_showAddDialog)) {
        ImGui::InputText("Name", m_newLayerName, sizeof(m_newLayerName));
        
        const char* typeNames[] = {"Background", "Main", "Foreground", "UI"};
        ImGui::Combo("Type", &m_newLayerType, typeNames, 4);
        
        ImGui::DragInt("Z-Index", &m_newLayerZIndex);
        
        ImGui::Separator();
        
        if (ImGui::Button("Create")) {
            if (std::strlen(m_newLayerName) > 0) {
                engine::LayerType type = static_cast<engine::LayerType>(m_newLayerType);
                m_layerManager->addLayer(m_newLayerName, type, m_newLayerZIndex);
                m_showAddDialog = false;
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Cancel")) {
            m_showAddDialog = false;
        }
        
        ImGui::EndPopup();
    }
}

void LayerEditorPanel::renderParallaxPreview() {
    ImGui::Text("Parallax Preview");
    
    // Scroll slider
    ImGui::SliderFloat("Scroll Position", &m_previewScroll, 0.0f, 1000.0f);
    
    // Visual preview
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize(400, 100);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    
    // Background
    drawList->AddRectFilled(canvasPos, 
                           ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
                           IM_COL32(30, 30, 30, 255));
    
    // Simulate parallax layers
    const auto& layers = m_layerManager->getLayers();
    for (const auto& layer : layers) {
        if (layer->getLayerType() == engine::LayerType::Background) {
            // Calculate parallax offset (simplified)
            float parallaxFactor = 0.5f;  // Could be layer-specific
            float offset = m_previewScroll * parallaxFactor;
            
            // Draw layer representation
            float x = canvasPos.x + std::fmod(offset, canvasSize.x);
            drawList->AddRectFilled(
                ImVec2(x, canvasPos.y + 20),
                ImVec2(x + 50, canvasPos.y + 80),
                IM_COL32(100, 150, 200, 200)
            );
            
            // Draw label
            drawList->AddText(ImVec2(x + 5, canvasPos.y + 40), 
                            IM_COL32(255, 255, 255, 255), 
                            layer->getName().c_str());
        }
    }
    
    ImGui::Dummy(canvasSize);
}
