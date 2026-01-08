/**
 * @file EditorContext.h
 * @brief Shared editor state accessible to all panels
 * 
 * Contains:
 * - Current selection state
 * - Clipboard data
 * - Editor preferences
 * - Cached textures
 */
#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <any>
#include "engine/data/GameData.h"

/**
 * @brief Selection types
 */
enum class SelectionType {
    None,
    Room,
    Hotspot,
    Layer,
    Dialog,
    DialogNode,
    Quest,
    Item,
    NPC,
    Actor,
    CollisionBox
};

/**
 * @brief Current selection state
 */
struct EditorSelection {
    SelectionType type = SelectionType::None;
    std::string primaryId;      // e.g., room ID
    int secondaryIndex = -1;    // e.g., hotspot index within room
    
    void clear() {
        type = SelectionType::None;
        primaryId.clear();
        secondaryIndex = -1;
    }
    
    bool hasSelection() const { return type != SelectionType::None; }
};

/**
 * @brief Shared editor context
 */
class EditorState;  // Forward declaration

class EditorContext {
public:
    EditorContext() = default;
    ~EditorContext();
    
    // Set the editor state (for accessing managers)
    void setEditorState(EditorState* state) { m_editorState = state; }
    
    // Selection type enum
    enum class SelectionType {
        None, Room, Dialog, Quest, Item, NPC, Hotspot, Actor, CollisionBox
    };
    
    // Current selection (direkt åtkomst för enkelhets skull)
    SelectionType selectedType = SelectionType::None;
    std::string selectedRoomId;
    std::string selectedDialogId;
    std::string selectedQuestId;
    std::string selectedItemId;
    std::string selectedNPCId;
    std::string selectedLevelId;
    std::string selectedSceneId;
    int selectedHotspotIndex = -1;
    
    // Actor and CollisionBox selection
    std::string selectedActorId;  // Actor name within scene
    int selectedCollisionBoxIndex = -1;
    
    // Selection helpers
    EditorSelection& getSelection() { return m_selection; }
    const EditorSelection& getSelection() const { return m_selection; }
    void setSelection(::SelectionType type, const std::string& id, int index = -1);
    void clearSelection();
    
    // Clipboard
    void copyToClipboard(const std::string& type, const std::any& data);
    bool hasClipboardData(const std::string& type) const;
    std::any getClipboardData(const std::string& type) const;
    
    // Texture cache
    SDL_Texture* getTexture(SDL_Renderer* renderer, const std::string& path);
    void clearTextureCache();
    
    // Editor preferences
    bool showGrid = true;
    bool snapToGrid = true;
    int gridSize = 10;
    bool showHotspotNames = true;
    bool showWalkArea = true;
    
    // Status message
    std::string statusMessage;
    float statusTimer = 0.0f;
    
    // Dirty flag for unsaved changes
    bool isDirty = false;
    void markDirty() { isDirty = true; }
    
    // Current editing data (mutable copies)
    std::vector<RoomData> rooms;
    std::vector<DialogData> dialogs;
    std::vector<QuestData> quests;
    std::vector<ItemData> items;
    std::vector<NPCData> npcs;
    
    // Load/Save
    void loadFromDataLoader();
    void saveToFiles();
    
    // Sync Level/Scene data to RoomData
    void syncScenesToRoomData();

private:
    EditorState* m_editorState = nullptr;
    EditorSelection m_selection;
    std::unordered_map<std::string, std::any> m_clipboard;
    std::unordered_map<std::string, SDL_Texture*> m_textureCache;
};

