/**
 * @file LayerManager.cpp
 * @brief Implementation of LayerManager
 */
#include "LayerManager.h"
#include <algorithm>

namespace engine {

// ═══════════════════════════════════════════════════════════════════════════
// LAYER
// ═══════════════════════════════════════════════════════════════════════════

Layer::Layer() : Node2D("Layer") {}

Layer::Layer(const std::string& name, LayerType type)
    : Node2D(name), m_layerType(type) {}

void Layer::render(SDL_Renderer* renderer) {
    if (!renderer) return;
    
    // Apply opacity if needed
    if (m_opacity < 1.0f) {
        // TODO: Implement opacity rendering
        // For now, just render normally
    }
    
    Node2D::render(renderer);
}

// ═══════════════════════════════════════════════════════════════════════════
// LAYER MANAGER
// ═══════════════════════════════════════════════════════════════════════════

LayerManager::LayerManager() {}

Layer* LayerManager::addLayer(const std::string& name, LayerType type, int zIndex) {
    auto layer = std::make_unique<Layer>(name, type);
    layer->setZIndex(zIndex);
    
    Layer* ptr = layer.get();
    m_layers.push_back(std::move(layer));
    
    // Sort by z-index
    std::stable_sort(m_layers.begin(), m_layers.end(),
        [](const std::unique_ptr<Layer>& a, const std::unique_ptr<Layer>& b) {
            return a->getZIndex() < b->getZIndex();
        });
    
    return ptr;
}

Layer* LayerManager::getLayer(const std::string& name) const {
    for (const auto& layer : m_layers) {
        if (layer && layer->getName() == name) {
            return layer.get();
        }
    }
    return nullptr;
}

std::vector<Layer*> LayerManager::getLayersByType(LayerType type) const {
    std::vector<Layer*> result;
    for (const auto& layer : m_layers) {
        if (layer && layer->getLayerType() == type) {
            result.push_back(layer.get());
        }
    }
    return result;
}

void LayerManager::removeLayer(const std::string& name) {
    m_layers.erase(
        std::remove_if(m_layers.begin(), m_layers.end(),
            [&name](const std::unique_ptr<Layer>& layer) {
                return layer && layer->getName() == name;
            }),
        m_layers.end()
    );
}

} // namespace engine
