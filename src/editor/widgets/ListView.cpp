/**
 * @file ListView.cpp
 * @brief Editor ListView widget implementation
 */
#include "ListView.h"
#include "engine/graphics/FontManager.h"
#include "engine/audio/AudioManager.h"
#include <algorithm>

namespace Editor {

ListView::ListView(int x, int y, int width, int height)
    : Widget(x, y, width, height) {
}

void ListView::update(float deltaTime) {
    (void)deltaTime;
}

void ListView::render(SDL_Renderer* renderer) {
    if (!m_state.visible) return;
    
    // Background
    SDL_SetRenderDrawColor(renderer, m_backgroundColor.r, m_backgroundColor.g, 
                          m_backgroundColor.b, m_backgroundColor.a);
    SDL_RenderFillRect(renderer, &m_bounds);
    
    // Border
    SDL_SetRenderDrawColor(renderer, m_borderColor.r, m_borderColor.g,
                          m_borderColor.b, m_borderColor.a);
    SDL_RenderDrawRect(renderer, &m_bounds);
    
    // Set clip rect
    SDL_RenderSetClipRect(renderer, &m_bounds);
    
    // Render items
    int visibleItems = m_bounds.h / m_itemHeight;
    int startIndex = m_scrollOffset / m_itemHeight;
    int yOffset = -(m_scrollOffset % m_itemHeight);
    
    for (int i = 0; i <= visibleItems && (startIndex + i) < static_cast<int>(m_items.size()); i++) {
        int index = startIndex + i;
        const auto& item = m_items[index];
        
        int itemY = m_bounds.y + yOffset + i * m_itemHeight;
        SDL_Rect itemRect = {m_bounds.x, itemY, m_bounds.w, m_itemHeight};
        
        // Selection/hover background
        if (index == m_selectedIndex) {
            SDL_SetRenderDrawColor(renderer, m_selectedColor.r, m_selectedColor.g,
                                  m_selectedColor.b, m_selectedColor.a);
            SDL_RenderFillRect(renderer, &itemRect);
        } else if (m_state.hovered && getItemAtY(m_bounds.y + i * m_itemHeight + m_itemHeight/2) == index) {
            SDL_SetRenderDrawColor(renderer, m_hoverColor.r, m_hoverColor.g,
                                  m_hoverColor.b, m_hoverColor.a);
            SDL_RenderFillRect(renderer, &itemRect);
        }
        
        // Item text
        SDL_Color textColor = item.enabled ? item.color : SDL_Color{100, 100, 100, 255};
        FontManager::instance().renderText(renderer, "default", item.text, 
                                          m_bounds.x + 5, itemY + 3, textColor);
        
        // Subtext
        if (!item.subtext.empty()) {
            SDL_Color subtextColor = {150, 150, 150, 255};
            FontManager::instance().renderText(renderer, "default", item.subtext,
                                              m_bounds.x + 5, itemY + 12, subtextColor);
        }
    }
    
    // Clear clip rect
    SDL_RenderSetClipRect(renderer, nullptr);
    
    // Scrollbar
    if (m_showScrollbar && static_cast<int>(m_items.size()) * m_itemHeight > m_bounds.h) {
        int totalHeight = static_cast<int>(m_items.size()) * m_itemHeight;
        float scrollRatio = static_cast<float>(m_bounds.h) / totalHeight;
        int scrollbarHeight = std::max(20, static_cast<int>(m_bounds.h * scrollRatio));
        float scrollPos = static_cast<float>(m_scrollOffset) / (totalHeight - m_bounds.h);
        int scrollbarY = m_bounds.y + static_cast<int>((m_bounds.h - scrollbarHeight) * scrollPos);
        
        SDL_Rect scrollbar = {m_bounds.x + m_bounds.w - 8, scrollbarY, 6, scrollbarHeight};
        SDL_SetRenderDrawColor(renderer, 80, 80, 100, 255);
        SDL_RenderFillRect(renderer, &scrollbar);
    }
}

bool ListView::handleEvent(const SDL_Event& event) {
    if (!m_state.visible || !m_state.enabled) return false;
    
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if (!containsPoint(event.button.x, event.button.y)) return false;
            
            int index = getItemAtY(event.button.y);
            if (index >= 0 && index < static_cast<int>(m_items.size())) {
                // Check for double click
                float currentTime = SDL_GetTicks() / 1000.0f;
                if (index == m_lastClickIndex && (currentTime - m_lastClickTime) < 0.3f) {
                    if (m_onDoubleClick) {
                        m_onDoubleClick(index, m_items[index]);
                    }
                } else {
                    setSelectedIndex(index);
                    AudioManager::instance().playSound("select");
                }
                
                m_lastClickIndex = index;
                m_lastClickTime = currentTime;
            }
            return true;
        }
        
        case SDL_MOUSEWHEEL: {
            if (!m_state.hovered) return false;
            
            int totalHeight = static_cast<int>(m_items.size()) * m_itemHeight;
            int maxScroll = std::max(0, totalHeight - m_bounds.h);
            
            m_scrollOffset -= event.wheel.y * m_itemHeight;
            m_scrollOffset = std::max(0, std::min(m_scrollOffset, maxScroll));
            return true;
        }
        
        case SDL_KEYDOWN: {
            if (!m_state.focused) return false;
            
            if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
                if (m_selectedIndex > 0) {
                    setSelectedIndex(m_selectedIndex - 1);
                    ensureVisible(m_selectedIndex);
                }
                return true;
            }
            
            if (event.key.keysym.scancode == SDL_SCANCODE_DOWN) {
                if (m_selectedIndex < static_cast<int>(m_items.size()) - 1) {
                    setSelectedIndex(m_selectedIndex + 1);
                    ensureVisible(m_selectedIndex);
                }
                return true;
            }
            break;
        }
    }
    
    return Widget::handleEvent(event);
}

void ListView::addItem(const ListViewItem& item) {
    m_items.push_back(item);
}

void ListView::removeItem(const std::string& id) {
    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
            [&](const ListViewItem& item) { return item.id == id; }),
        m_items.end()
    );
    
    if (m_selectedIndex >= static_cast<int>(m_items.size())) {
        m_selectedIndex = static_cast<int>(m_items.size()) - 1;
    }
}

void ListView::clearItems() {
    m_items.clear();
    m_selectedIndex = -1;
    m_scrollOffset = 0;
}

void ListView::setSelectedIndex(int index) {
    if (index < -1 || index >= static_cast<int>(m_items.size())) return;
    
    m_selectedIndex = index;
    
    if (m_onSelect && index >= 0) {
        m_onSelect(index, m_items[index]);
    }
}

void ListView::setSelectedId(const std::string& id) {
    for (size_t i = 0; i < m_items.size(); i++) {
        if (m_items[i].id == id) {
            setSelectedIndex(static_cast<int>(i));
            return;
        }
    }
}

const ListViewItem* ListView::getSelectedItem() const {
    if (m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_items.size())) {
        return nullptr;
    }
    return &m_items[m_selectedIndex];
}

int ListView::getItemAtY(int y) const {
    if (y < m_bounds.y || y >= m_bounds.y + m_bounds.h) return -1;
    
    int relativeY = y - m_bounds.y + m_scrollOffset;
    return relativeY / m_itemHeight;
}

void ListView::ensureVisible(int index) {
    if (index < 0 || index >= static_cast<int>(m_items.size())) return;
    
    int itemTop = index * m_itemHeight;
    int itemBottom = itemTop + m_itemHeight;
    
    if (itemTop < m_scrollOffset) {
        m_scrollOffset = itemTop;
    } else if (itemBottom > m_scrollOffset + m_bounds.h) {
        m_scrollOffset = itemBottom - m_bounds.h;
    }
}

} // namespace Editor
