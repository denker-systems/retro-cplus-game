/**
 * @file EditorState.h
 * @brief Separat editor state för att visualisera och redigera game content
 * 
 * Öppnas från huvudmenyn, inte som overlay under gameplay.
 */
#pragma once

#include "game/states/IState.h"
#include "EditorContext.h"
#include "editor/data/EditorDataManager.h"

// Forward declarations
namespace engine {
    class Scene;
    class LayerManager;
    class World;
}
#include "engine/data/GameData.h"
#include "editor/legacy/VisualRoomEditor.h"
#include "editor/legacy/EditorTabRenderer.h"
#include <SDL.h>
#include <string>
#include <vector>
#include <memory>

// Forward declarations
struct RoomNode;
class IEditorPanel;
class HierarchyPanel;
class PropertiesPanel;
class ViewportPanel;
class AssetBrowserPanel;
class PlaceActorsPanel;
class ConsolePanel;
class DialogGraphPanel;
class QuestGraphPanel;
class BehaviorGraphPanel;
class SceneGraphPanel;
class LayerEditorPanel;
class TileMapEditorPanel;
class WorldViewPanel;
class LevelViewPanel;
class EditorInputHandler;
class EditorPanelManager;
class EditorWorldManager;
class EditorEventDispatcher;

/**
 * @brief Editor tabs/modes
 */
enum class EditorTab {
    Overview,     // Flowchart av alla rum
    Rooms,        // Redigera rum och hotspots
    Dialogs,      // Redigera dialogträd
    Quests,       // Redigera quests
    Items,        // Redigera items
    WorldState,   // Flags och counters
    Audio         // Musik och ljud
};

/**
 * @brief Separat editor state
 */
class EditorState : public IState {
public:
    EditorState();
    ~EditorState() override = default;
    
    void enter() override;
    void exit() override;
    void update(float deltaTime) override;
    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& event) override;

private:
    // Tab rendering
    void renderTabs(SDL_Renderer* renderer, int screenW, int screenH, float scale);
    void renderOverviewTab(SDL_Renderer* renderer);
    void renderRoomsTab(SDL_Renderer* renderer);
    void renderDialogsTab(SDL_Renderer* renderer);
    void renderQuestsTab(SDL_Renderer* renderer);
    void renderItemsTab(SDL_Renderer* renderer);
    void renderWorldStateTab(SDL_Renderer* renderer);
    void renderAudioTab(SDL_Renderer* renderer);
    
    // Flowchart rendering
    void renderRoomFlowchart(SDL_Renderer* renderer);
    void renderDialogTree(SDL_Renderer* renderer, const std::string& dialogId);
    
    // Input handling
    void handleTabSwitch(int direction);
    void handleMouseClick(int x, int y);
    
    // Tiled import/export
    void importFromTiled();
    void exportToTiled();
    void exportAllToTiled();
    
    // Manual room editing
    void openRoomEditor();
    void renderRoomEditor(SDL_Renderer* renderer);
    void renderVisualEditor(SDL_Renderer* renderer);
    void saveRoomChanges();
    void loadRoomPreview(SDL_Renderer* renderer);
    void syncScenesToRoomData();  // Sync engine::Scene changes back to RoomData
    
    void renderButton(SDL_Renderer* renderer, const char* text, int x, int y, int w, int h, bool highlight = false);
    
    // ImGui rendering (om HAS_IMGUI är definierad)
    void renderImGui();
    bool isButtonClicked(int btnX, int btnY, int btnW, int btnH, int clickX, int clickY);
    void renderTextField(SDL_Renderer* renderer, const std::string& label, const std::string& value, int x, int y, int w, bool selected);
    
    EditorTab m_currentTab = EditorTab::Overview;
    std::string m_statusMessage;
    float m_statusTimer = 0.0f;
    
    // Editor settings
    bool m_useTiledWorkflow = false;  // Toggle mellan Tiled och manuell redigering
    
    // Scroll och selection
    int m_scrollY = 0;
    std::string m_selectedRoom;
    std::string m_selectedDialog;
    std::string m_selectedQuest;
    std::string m_selectedItem;
    
    // Room flowchart positions (beräknade)
    std::vector<RoomNode> m_roomNodes;
    
    // Mouse state
    int m_mouseX = 0;
    int m_mouseY = 0;
    bool m_isDragging = false;
    int m_dragOffsetX = 0;
    int m_dragOffsetY = 0;
    
    // Room editor state
    bool m_editingRoom = false;
    bool m_visualEditor = false;  // Visuell editor mode
    int m_selectedField = 0;  // Vilket fält som är valt för redigering
    bool m_editingText = false;  // Om vi redigerar text just nu
    std::string m_textBuffer;  // Buffer för text-input
    
    // Visual editor
    std::unique_ptr<VisualRoomEditor> m_visualRoomEditor;
    SDL_Texture* m_roomPreviewTexture = nullptr;
    
    // Edit room data
    RoomData m_editRoomData;
    
    // Core systems
    EditorContext m_editorContext;
    std::unique_ptr<EditorPanelManager> m_panelManager;
    std::unique_ptr<EditorWorldManager> m_worldManager;
    std::unique_ptr<EditorEventDispatcher> m_eventDispatcher;
    
public:
    // Accessors for EditorContext
    EditorWorldManager* getWorldManager() const { return m_worldManager.get(); }
    
    // Data manager for all save/load operations
    editor::EditorDataManager m_dataManager;
};
