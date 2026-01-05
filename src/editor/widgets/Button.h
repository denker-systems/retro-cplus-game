/**
 * @file Button.h
 * @brief Clickable editor button widget
 */
#pragma once

#include "Widget.h"
#include <functional>

namespace Editor {

/**
 * @brief Button widget with click callback
 */
class Button : public Widget {
public:
    Button(int x, int y, int width, int height, const std::string& text);
    
    void render(SDL_Renderer* renderer) override;
    
    void setText(const std::string& text) { m_text = text; }
    const std::string& getText() const { return m_text; }
    
    void setOnClick(std::function<void()> callback) { m_onClick = callback; }
    
    // Styling
    void setColors(SDL_Color normal, SDL_Color hover, SDL_Color pressed);
    void setTextColor(SDL_Color color) { m_textColor = color; }

protected:
    void onClick() override;

private:
    std::string m_text;
    std::function<void()> m_onClick;
    
    SDL_Color m_normalColor = {60, 60, 80, 255};
    SDL_Color m_hoverColor = {80, 80, 100, 255};
    SDL_Color m_pressedColor = {40, 40, 60, 255};
    SDL_Color m_textColor = {255, 255, 255, 255};
    SDL_Color m_borderColor = {100, 150, 200, 255};
};

} // namespace Editor
