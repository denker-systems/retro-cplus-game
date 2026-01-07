/**
 * @file PlaceActorsPanel.h
 * @brief Panel for placing actor templates via drag-and-drop (UE5-style)
 */
#pragma once

#include "editor/IEditorPanel.h"
#include <string>
#include <vector>

struct SDL_Renderer;
struct SDL_Texture;
class EditorContext;

/**
 * @brief Actor category for organization
 */
enum class ActorCategory {
    Basic,
    Characters,
    Props,
    Triggers,
    Lights,
    Audio
};

/**
 * @brief Template for placeable actors
 */
struct ActorTemplate {
    std::string name;
    std::string icon;           // Icon character or path
    ActorCategory category;
    std::string description;
    std::string defaultSprite;  // Optional default sprite path
    bool hasSprite = false;
};

/**
 * @brief Panel for placing actors in the scene
 */
class PlaceActorsPanel : public IEditorPanel {
public:
    explicit PlaceActorsPanel(EditorContext& context);
    ~PlaceActorsPanel() = default;
    
    void setRenderer(SDL_Renderer* renderer) { m_renderer = renderer; }
    
    const std::string& getId() const override { return m_id; }
    const std::string& getTitle() const override { return m_title; }
    
    void render() override;
    
private:
    void initializeTemplates();
    void renderCategory(ActorCategory category, const char* name);
    void renderActorButton(const ActorTemplate& templ);
    
    EditorContext& m_context;
    std::string m_id = "placeactors";
    std::string m_title = "Place Actors";
    
    std::vector<ActorTemplate> m_templates;
    ActorCategory m_selectedCategory = ActorCategory::Basic;
    std::string m_searchFilter;
    
    SDL_Renderer* m_renderer = nullptr;
};
