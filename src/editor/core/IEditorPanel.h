/**
 * @file IEditorPanel.h
 * @brief Interface för ImGui-baserade editor-paneler
 */
#pragma once

#include <string>

class EditorContext;

/**
 * @brief Base interface för ImGui editor-paneler
 * 
 * Varje panel är ett dockbart ImGui-fönster:
 * - HierarchyPanel: Träd med rum, dialoger, etc.
 * - ViewportPanel: Rum-preview och editing
 * - PropertiesPanel: Inspector för valt objekt
 * - AssetBrowserPanel: Assets (sprites, sounds, etc.)
 * - ConsolePanel: Logg och status
 */
class IEditorPanel {
public:
    virtual ~IEditorPanel() = default;
    
    /** @brief Hämta panel-ID (används för docking) */
    virtual const std::string& getId() const = 0;
    
    /** @brief Hämta visningsnamn för fönster */
    virtual const std::string& getTitle() const = 0;
    
    /** @brief Rendera panelens ImGui-innehåll */
    virtual void render() = 0;
    
    /** @brief Uppdatera panel-logik */
    virtual void update(float deltaTime) {}
    
    /** @brief Är panelen synlig? */
    bool isVisible() const { return m_visible; }
    
    /** @brief Sätt synlighet */
    void setVisible(bool visible) { m_visible = visible; }
    
    /** @brief Toggle synlighet */
    void toggleVisible() { m_visible = !m_visible; }
    
protected:
    bool m_visible = true;
};

