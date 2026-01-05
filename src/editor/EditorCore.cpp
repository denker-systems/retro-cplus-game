/**
 * @file EditorCore.cpp
 * @brief Implementation of central editor coordinator
 */
#include "EditorCore.h"
#include "IEditorPanel.h"
#include "IEditorCommand.h"
#include "EditorContext.h"
#include "../utils/Logger.h"

EditorCore& EditorCore::instance() {
    static EditorCore instance;
    return instance;
}

void EditorCore::init(SDL_Renderer* renderer) {
    m_renderer = renderer;
    m_context = std::make_unique<EditorContext>();
    m_context->loadFromDataLoader();
    
    LOG_INFO("EditorCore initialized");
}

void EditorCore::shutdown() {
    m_panels.clear();
    m_panelOrder.clear();
    m_undoStack.clear();
    m_redoStack.clear();
    m_context.reset();
    
    LOG_INFO("EditorCore shutdown");
}

void EditorCore::update(float deltaTime) {
    // Update status timer
    if (m_statusTimer > 0.0f) {
        m_statusTimer -= deltaTime;
        if (m_statusTimer <= 0.0f) {
            m_statusMessage.clear();
        }
    }
    
    // Update current panel
    if (!m_currentPanelId.empty()) {
        auto it = m_panels.find(m_currentPanelId);
        if (it != m_panels.end()) {
            it->second->update(deltaTime);
        }
    }
}

void EditorCore::render(SDL_Renderer* renderer) {
    // Render current panel
    if (!m_currentPanelId.empty()) {
        auto it = m_panels.find(m_currentPanelId);
        if (it != m_panels.end()) {
            it->second->render(renderer);
        }
    }
}

void EditorCore::handleEvent(const SDL_Event& event) {
    // Global shortcuts
    if (event.type == SDL_KEYDOWN) {
        bool ctrl = (event.key.keysym.mod & KMOD_CTRL) != 0;
        
        if (ctrl && event.key.keysym.scancode == SDL_SCANCODE_Z) {
            if (event.key.keysym.mod & KMOD_SHIFT) {
                redo();
            } else {
                undo();
            }
            return;
        }
        
        if (ctrl && event.key.keysym.scancode == SDL_SCANCODE_Y) {
            redo();
            return;
        }
        
        if (ctrl && event.key.keysym.scancode == SDL_SCANCODE_S) {
            m_context->saveToFiles();
            setStatus("Saved all changes");
            markClean();
            return;
        }
    }
    
    // Forward to current panel
    if (!m_currentPanelId.empty()) {
        auto it = m_panels.find(m_currentPanelId);
        if (it != m_panels.end()) {
            it->second->handleEvent(event);
        }
    }
}

// Panel management

void EditorCore::registerPanel(const std::string& id, std::unique_ptr<IEditorPanel> panel) {
    m_panelOrder.push_back(id);
    m_panels[id] = std::move(panel);
    
    // Set first panel as current
    if (m_currentPanelId.empty()) {
        switchToPanel(id);
    }
    
    LOG_INFO("Registered editor panel: " + id);
}

void EditorCore::switchToPanel(const std::string& id) {
    auto it = m_panels.find(id);
    if (it == m_panels.end()) {
        LOG_WARNING("Panel not found: " + id);
        return;
    }
    
    // Deactivate current
    if (!m_currentPanelId.empty()) {
        auto current = m_panels.find(m_currentPanelId);
        if (current != m_panels.end()) {
            current->second->onDeactivate();
        }
    }
    
    // Activate new
    m_currentPanelId = id;
    it->second->onActivate();
    
    dispatch({EditorEvent::PanelSwitched, "EditorCore", id});
}

IEditorPanel* EditorCore::getCurrentPanel() {
    if (m_currentPanelId.empty()) return nullptr;
    auto it = m_panels.find(m_currentPanelId);
    return it != m_panels.end() ? it->second.get() : nullptr;
}

IEditorPanel* EditorCore::getPanel(const std::string& id) {
    auto it = m_panels.find(id);
    return it != m_panels.end() ? it->second.get() : nullptr;
}

// Command history

void EditorCore::executeCommand(std::unique_ptr<IEditorCommand> command) {
    if (!command) return;
    
    // Try to merge with previous command
    if (!m_undoStack.empty()) {
        auto& last = m_undoStack.back();
        if (last->canMergeWith(command.get())) {
            last->mergeWith(command.get());
            last->execute();
            markDirty();
            return;
        }
    }
    
    // Execute and add to stack
    if (command->execute()) {
        m_undoStack.push_back(std::move(command));
        
        // Clear redo stack (new action invalidates redo history)
        m_redoStack.clear();
        
        // Limit undo stack size
        while (m_undoStack.size() > MAX_UNDO_LEVELS) {
            m_undoStack.erase(m_undoStack.begin());
        }
        
        markDirty();
    }
}

bool EditorCore::canUndo() const {
    return !m_undoStack.empty();
}

bool EditorCore::canRedo() const {
    return !m_redoStack.empty();
}

void EditorCore::undo() {
    if (m_undoStack.empty()) return;
    
    auto command = std::move(m_undoStack.back());
    m_undoStack.pop_back();
    
    if (command->undo()) {
        setStatus("Undo: " + command->getDescription());
        m_redoStack.push_back(std::move(command));
        dispatch({EditorEvent::UndoPerformed, "EditorCore", ""});
    }
}

void EditorCore::redo() {
    if (m_redoStack.empty()) return;
    
    auto command = std::move(m_redoStack.back());
    m_redoStack.pop_back();
    
    if (command->execute()) {
        setStatus("Redo: " + command->getDescription());
        m_undoStack.push_back(std::move(command));
        dispatch({EditorEvent::RedoPerformed, "EditorCore", ""});
    }
}

void EditorCore::clearHistory() {
    m_undoStack.clear();
    m_redoStack.clear();
}

// Event system

void EditorCore::subscribe(EditorEvent event, const std::string& subscriberId, EditorEventCallback callback) {
    m_eventSubscribers[event].push_back({subscriberId, callback});
}

void EditorCore::unsubscribe(EditorEvent event, const std::string& subscriberId) {
    auto it = m_eventSubscribers.find(event);
    if (it == m_eventSubscribers.end()) return;
    
    auto& subscribers = it->second;
    subscribers.erase(
        std::remove_if(subscribers.begin(), subscribers.end(),
            [&](const EventSubscription& sub) { return sub.subscriberId == subscriberId; }),
        subscribers.end()
    );
}

void EditorCore::dispatch(const EditorEventData& eventData) {
    auto it = m_eventSubscribers.find(eventData.type);
    if (it == m_eventSubscribers.end()) return;
    
    for (const auto& sub : it->second) {
        sub.callback(eventData);
    }
}

void EditorCore::setStatus(const std::string& message, float duration) {
    m_statusMessage = message;
    m_statusTimer = duration;
}

