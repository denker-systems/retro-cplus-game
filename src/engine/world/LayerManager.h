/**
 * @file LayerManager.h
 * @brief Manages rendering layers in a scene
 */
#pragma once

#include "core/Node2D.h"
#include <vector>
#include <memory>
#include <string>

namespace engine {

/**
 * @brief Layer types for organization
 */
enum class LayerType {
    Background,     // Parallax backgrounds
    Main,          // Main game objects
    Foreground,    // Foreground overlays
    UI             // Screen-space UI (no camera transform)
};

/**
 * @brief Single rendering layer
 */
class Layer : public Node2D {
public:
    Layer();
    explicit Layer(const std::string& name, LayerType type = LayerType::Main);
    virtual ~Layer() = default;
    
    LayerType getLayerType() const { return m_layerType; }
    void setLayerType(LayerType type) { m_layerType = type; }
    
    /** @brief Opacity for fade effects */
    float getOpacity() const { return m_opacity; }
    void setOpacity(float opacity) { m_opacity = opacity; }
    
    void render(SDL_Renderer* renderer) override;
    
protected:
    LayerType m_layerType = LayerType::Main;
    float m_opacity = 1.0f;
};

/**
 * @brief Manages multiple layers in a scene
 */
class LayerManager {
public:
    LayerManager();
    ~LayerManager() = default;
    
    /** @brief Add a layer */
    Layer* addLayer(const std::string& name, LayerType type, int zIndex = 0);
    
    /** @brief Get layer by name */
    Layer* getLayer(const std::string& name) const;
    
    /** @brief Get all layers of a type */
    std::vector<Layer*> getLayersByType(LayerType type) const;
    
    /** @brief Remove layer */
    void removeLayer(const std::string& name);
    
    /** @brief Get all layers */
    const std::vector<std::unique_ptr<Layer>>& getLayers() const { return m_layers; }
    
protected:
    std::vector<std::unique_ptr<Layer>> m_layers;
};

} // namespace engine
