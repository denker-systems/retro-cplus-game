/**
 * @file ListView.h
 * @brief Scrollable editor list widget
 */
#pragma once

#include "Widget.h"
#include <functional>

namespace Editor {

/**
 * @brief Item in a list view
 */
struct ListViewItem {
    std::string id;
    std::string text;
    std::string subtext;
    SDL_Color color = {255, 255, 255, 255};
    bool enabled = true;
};

/**
 * @brief Scrollable list widget
 */
class ListView : public Widget {
public:
    ListView(int x, int y, int width, int height);
    
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    bool handleEvent(const SDL_Event& event) override;
    
    // Items
    void addItem(const ListViewItem& item);
    void removeItem(const std::string& id);
    void clearItems();
    const std::vector<ListViewItem>& getItems() const { return m_items; }
    
    // Selection
    void setSelectedIndex(int index);
    void setSelectedId(const std::string& id);
    int getSelectedIndex() const { return m_selectedIndex; }
    const ListViewItem* getSelectedItem() const;
    void clearSelection() { m_selectedIndex = -1; }
    
    // Callbacks
    void setOnSelect(std::function<void(int, const ListViewItem&)> callback) { m_onSelect = callback; }
    void setOnDoubleClick(std::function<void(int, const ListViewItem&)> callback) { m_onDoubleClick = callback; }
    
    // Appearance
    void setItemHeight(int height) { m_itemHeight = height; }
    void setShowScrollbar(bool show) { m_showScrollbar = show; }

private:
    int getItemAtY(int y) const;
    void ensureVisible(int index);
    
    std::vector<ListViewItem> m_items;
    int m_selectedIndex = -1;
    int m_scrollOffset = 0;
    int m_itemHeight = 20;
    bool m_showScrollbar = true;
    
    float m_lastClickTime = 0.0f;
    int m_lastClickIndex = -1;
    
    std::function<void(int, const ListViewItem&)> m_onSelect;
    std::function<void(int, const ListViewItem&)> m_onDoubleClick;
    
    SDL_Color m_backgroundColor = {30, 30, 40, 255};
    SDL_Color m_selectedColor = {60, 60, 100, 255};
    SDL_Color m_hoverColor = {50, 50, 70, 255};
    SDL_Color m_borderColor = {80, 80, 100, 255};
};

} // namespace Editor
