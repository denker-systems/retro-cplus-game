/**
 * @file TextField.h
 * @brief Text input widget
 */
#pragma once

#include "Widget.h"
#include <functional>

/**
 * @brief Text input field widget
 */
class TextField : public Widget {
public:
    TextField(int x, int y, int width, int height);
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    bool handleEvent(const SDL_Event& event) override;
    
    // Text
    void setText(const std::string& text);
    const std::string& getText() const { return m_text; }
    
    // Placeholder
    void setPlaceholder(const std::string& text) { m_placeholder = text; }
    
    // Label
    void setLabel(const std::string& label) { m_label = label; }
    
    // Callbacks
    void setOnChange(std::function<void(const std::string&)> callback) { m_onChange = callback; }
    void setOnSubmit(std::function<void(const std::string&)> callback) { m_onSubmit = callback; }
    
    // Validation
    void setMaxLength(size_t maxLength) { m_maxLength = maxLength; }
    void setNumericOnly(bool numeric) { m_numericOnly = numeric; }

protected:
    void onFocusGained() override;
    void onFocusLost() override;

private:
    std::string m_text;
    std::string m_placeholder;
    std::string m_label;
    size_t m_cursorPos = 0;
    float m_cursorBlink = 0.0f;
    bool m_showCursor = true;
    
    size_t m_maxLength = 256;
    bool m_numericOnly = false;
    
    std::function<void(const std::string&)> m_onChange;
    std::function<void(const std::string&)> m_onSubmit;
    
    SDL_Color m_backgroundColor = {40, 40, 60, 255};
    SDL_Color m_focusedColor = {60, 60, 80, 255};
    SDL_Color m_textColor = {255, 255, 255, 255};
    SDL_Color m_placeholderColor = {150, 150, 150, 255};
    SDL_Color m_borderColor = {100, 150, 200, 255};
    SDL_Color m_labelColor = {100, 255, 255, 255};
};

