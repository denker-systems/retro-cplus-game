/**
 * @file EditorCore.h
 * @brief Central editor coordinator with command history and undo/redo
 * 
 * Architecture Overview:
 * - EditorCore: Central coordinator, manages panels and command history
 * - IEditorPanel: Interface for editor panels (Rooms, Dialogs, etc.)
 * - IEditorCommand: Interface for undoable commands
 * - EditorContext: Shared state accessible to all panels
 * 
 * Design Patterns:
 * - Command Pattern: For undo/redo functionality
 * - Observer Pattern: For event communication between panels
 * - Strategy Pattern: For different editor tools
 */
#pragma once

#include <SDL.h>
#include <memory>
#include <vector>
#include <stack>
#include <string>
#include <functional>
#include <unordered_map>

// Forward declarations
class IEditorPanel;
class IEditorCommand;
class EditorContext;

/**
 * @brief Event types for editor communication
 */
enum class EditorEvent {
    SelectionChanged,
    DataModified,
    PanelSwitched,
    UndoPerformed,
    RedoPerformed,
    SaveRequested,
    RefreshRequested
};

/**
 * @brief Event data payload
 */
struct EditorEventData {
    EditorEvent type;
    std::string source;
    std::string payload;
};

using EditorEventCallback = std::function<void(const EditorEventData&)>;

/**
 * @brief Central editor coordinator
 * 
 * Manages:
 * - Panel registration and switching
 * - Command history (undo/redo)
 * - Event dispatch between panels
 * - Shared editor context
 */
class EditorCore {
public:
    static EditorCore& instance();
    
    // Lifecycle
    void init(SDL_Renderer* renderer);
    void shutdown();
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void handleEvent(const SDL_Event& event);
    
    // Panel management
    void registerPanel(const std::string& id, std::unique_ptr<IEditorPanel> panel);
    void switchToPanel(const std::string& id);
    IEditorPanel* getCurrentPanel();
    IEditorPanel* getPanel(const std::string& id);
    const std::vector<std::string>& getPanelIds() const { return m_panelOrder; }
    
    // Command history (undo/redo)
    void executeCommand(std::unique_ptr<IEditorCommand> command);
    bool canUndo() const;
    bool canRedo() const;
    void undo();
    void redo();
    void clearHistory();
    
    // Event system
    void subscribe(EditorEvent event, const std::string& subscriberId, EditorEventCallback callback);
    void unsubscribe(EditorEvent event, const std::string& subscriberId);
    void dispatch(const EditorEventData& eventData);
    
    // Context access
    EditorContext& getContext() { return *m_context; }
    SDL_Renderer* getRenderer() const { return m_renderer; }
    
    // Status
    void setStatus(const std::string& message, float duration = 2.0f);
    const std::string& getStatus() const { return m_statusMessage; }
    bool isDirty() const { return m_dirty; }
    void markDirty() { m_dirty = true; }
    void markClean() { m_dirty = false; }

private:
    EditorCore() = default;
    ~EditorCore() = default;
    
    SDL_Renderer* m_renderer = nullptr;
    std::unique_ptr<EditorContext> m_context;
    
    // Panels
    std::unordered_map<std::string, std::unique_ptr<IEditorPanel>> m_panels;
    std::vector<std::string> m_panelOrder;
    std::string m_currentPanelId;
    
    // Command history
    std::vector<std::unique_ptr<IEditorCommand>> m_undoStack;
    std::vector<std::unique_ptr<IEditorCommand>> m_redoStack;
    static constexpr size_t MAX_UNDO_LEVELS = 100;
    
    // Event system
    struct EventSubscription {
        std::string subscriberId;
        EditorEventCallback callback;
    };
    std::unordered_map<EditorEvent, std::vector<EventSubscription>> m_eventSubscribers;
    
    // Status
    std::string m_statusMessage;
    float m_statusTimer = 0.0f;
    bool m_dirty = false;
};

