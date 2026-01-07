/**
 * @file RoomPanel.h
 * @brief Visual room editor panel
 * 
 * Features:
 * - Room list with selection
 * - Visual room preview with hotspots
 * - Drag-and-drop hotspot editing
 * - Property panel for selected items
 * - Layer management
 */
#pragma once

#include "../IEditorPanel.h"
#include "../EditorContext.h"
#include "../widgets/Widget.h"
#include <memory>
#include <vector>

class ListView;
class Button;
class TextField;

/**
 * @brief Editor tool modes
 */
enum class RoomEditorTool {
    Select,     // Select and move hotspots
    Resize,     // Resize hotspots
    Create,     // Create new hotspots
    Pan         // Pan the view
};

/**
 * @brief Room editor panel
 */
class RoomPanel : public IEditorPanel {
public:
    RoomPanel();
    ~RoomPanel() override;
    
    // IEditorPanel interface
    const std::string& getId() const override { return m_id; }
    const std::string& getDisplayName() const override { return m_displayName; }
    void onActivate() override;
    void onDeactivate() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    bool handleEvent(const SDL_Event& event) override;
    std::string getShortcutHint() const override;

private:
    // Rendering
    void renderRoomList(SDL_Renderer* renderer);
    void renderRoomPreview(SDL_Renderer* renderer);
    void renderHotspots(SDL_Renderer* renderer);
    void renderPropertyPanel(SDL_Renderer* renderer);
    void renderToolbar(SDL_Renderer* renderer);
    
    // Interaction
    void handlePreviewClick(int x, int y, int button);
    void handlePreviewDrag(int x, int y);
    void handlePreviewRelease(int x, int y);
    int getHotspotAtPosition(int x, int y);
    
    // Coordinate conversion (screen <-> room)
    void screenToRoom(int sx, int sy, int& rx, int& ry);
    void roomToScreen(int rx, int ry, int& sx, int& sy);
    
    // Data
    void loadRoomTexture(SDL_Renderer* renderer);
    void selectRoom(const std::string& roomId);
    void selectHotspot(int index);
    
    // Identity
    std::string m_id = "rooms";
    std::string m_displayName = "Rooms";
    
    // Layout
    static constexpr int LIST_WIDTH = 180;
    static constexpr int PROPERTY_WIDTH = 200;
    static constexpr int TOOLBAR_HEIGHT = 30;
    static constexpr int PREVIEW_X = LIST_WIDTH + 10;
    static constexpr int PREVIEW_Y = 90;
    static constexpr int PREVIEW_WIDTH = 420;
    static constexpr int PREVIEW_HEIGHT = 262;
    
    // State
    RoomData* m_currentRoom = nullptr;
    SDL_Texture* m_roomTexture = nullptr;
    RoomEditorTool m_currentTool = RoomEditorTool::Select;
    
    // Selection and dragging
    int m_selectedHotspotIndex = -1;
    bool m_isDragging = false;
    bool m_isResizing = false;
    int m_dragStartX = 0;
    int m_dragStartY = 0;
    int m_hotspotStartX = 0;
    int m_hotspotStartY = 0;
    int m_hotspotStartW = 0;
    int m_hotspotStartH = 0;
    int m_resizeHandle = 0;  // 0-7 for corners and edges
    
    // View
    float m_zoom = 1.0f;
    int m_panX = 0;
    int m_panY = 0;
};

