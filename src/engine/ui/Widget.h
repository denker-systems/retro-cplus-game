/**
 * @file Widget.h
 * @brief Bas-klass för UI widgets
 */
#pragma once

#include <SDL.h>
#include <string>
#include <functional>
#include <memory>
#include <vector>

/**
 * @brief Abstrakt bas-klass för alla UI widgets
 */
class Widget {
public:
    Widget(int x, int y, int width, int height);
    virtual ~Widget() = default;
    
    virtual void update(float deltaTime);
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void handleEvent(const SDL_Event& event);
    
    // Position och storlek
    void setPosition(int x, int y) { m_x = x; m_y = y; }
    void setSize(int width, int height) { m_width = width; m_height = height; }
    SDL_Rect getBounds() const { return {m_x, m_y, m_width, m_height}; }
    
    int getX() const { return m_x; }
    int getY() const { return m_y; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    // Visibility och enabled
    void setVisible(bool visible) { m_visible = visible; }
    bool isVisible() const { return m_visible; }
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }
    
    // Focus
    void setFocused(bool focused) { m_focused = focused; }
    bool isFocused() const { return m_focused; }
    
    // ID
    void setId(const std::string& id) { m_id = id; }
    const std::string& getId() const { return m_id; }
    
    // Hit test
    bool containsPoint(int x, int y) const;

protected:
    int m_x, m_y;
    int m_width, m_height;
    bool m_visible = true;
    bool m_enabled = true;
    bool m_focused = false;
    bool m_hovered = false;
    std::string m_id;
};

/**
 * @brief Label widget för text
 */
class Label : public Widget {
public:
    Label(int x, int y, const std::string& text);
    
    void render(SDL_Renderer* renderer) override;
    
    void setText(const std::string& text) { m_text = text; }
    const std::string& getText() const { return m_text; }
    
    void setColor(SDL_Color color) { m_color = color; }
    void setFontId(const std::string& fontId) { m_fontId = fontId; }
    
    void setCentered(bool centered) { m_centered = centered; }

private:
    std::string m_text;
    std::string m_fontId = "default";
    SDL_Color m_color = {255, 255, 255, 255};
    bool m_centered = false;
};

/**
 * @brief Button widget
 */
class Button : public Widget {
public:
    Button(int x, int y, int width, int height, const std::string& text);
    
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;
    
    void setText(const std::string& text) { m_text = text; }
    const std::string& getText() const { return m_text; }
    
    void setOnClick(std::function<void()> callback) { m_onClick = callback; }
    
    // Färger
    void setNormalColor(SDL_Color color) { m_normalColor = color; }
    void setHoverColor(SDL_Color color) { m_hoverColor = color; }
    void setPressedColor(SDL_Color color) { m_pressedColor = color; }

private:
    std::string m_text;
    std::function<void()> m_onClick;
    bool m_pressed = false;
    
    SDL_Color m_normalColor = {60, 50, 80, 255};
    SDL_Color m_hoverColor = {80, 70, 100, 255};
    SDL_Color m_pressedColor = {40, 35, 60, 255};
    SDL_Color m_textColor = {255, 255, 255, 255};
};

/**
 * @brief Panel widget - container för andra widgets
 */
class Panel : public Widget {
public:
    Panel(int x, int y, int width, int height);
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;
    
    void addWidget(std::unique_ptr<Widget> widget);
    Widget* getWidget(const std::string& id);
    void clearWidgets();
    
    void setBackgroundColor(SDL_Color color) { m_bgColor = color; }
    void setBorderColor(SDL_Color color) { m_borderColor = color; }
    void setDrawBackground(bool draw) { m_drawBackground = draw; }
    void setDrawBorder(bool draw) { m_drawBorder = draw; }

private:
    std::vector<std::unique_ptr<Widget>> m_widgets;
    SDL_Color m_bgColor = {30, 25, 40, 200};
    SDL_Color m_borderColor = {100, 90, 120, 255};
    bool m_drawBackground = true;
    bool m_drawBorder = true;
};

/**
 * @brief ProgressBar widget
 */
class ProgressBar : public Widget {
public:
    ProgressBar(int x, int y, int width, int height);
    
    void render(SDL_Renderer* renderer) override;
    
    void setValue(float value);  // 0.0 - 1.0
    float getValue() const { return m_value; }
    
    void setFillColor(SDL_Color color) { m_fillColor = color; }
    void setBackgroundColor(SDL_Color color) { m_bgColor = color; }

private:
    float m_value = 0.0f;
    SDL_Color m_fillColor = {100, 180, 100, 255};
    SDL_Color m_bgColor = {40, 35, 50, 255};
};
