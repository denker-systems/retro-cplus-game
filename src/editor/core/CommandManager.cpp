/**
 * @file CommandManager.cpp
 * @brief Implementation of CommandManager
 */
#include "CommandManager.h"
#include "engine/utils/Logger.h"

CommandManager& CommandManager::instance() {
    static CommandManager instance;
    return instance;
}

void CommandManager::executeCommand(std::unique_ptr<IEditorCommand> command) {
    if (!command) return;
    
    // Execute the command
    if (command->execute()) {
        // Add to undo stack
        m_undoStack.push_back(std::move(command));
        
        // Clear redo stack when new command is executed
        m_redoStack.clear();
        
        // Limit history size
        while (m_undoStack.size() > m_maxHistory) {
            m_undoStack.erase(m_undoStack.begin());
        }
        
        LOG_INFO("Command executed: " + m_undoStack.back()->getDescription());
    }
}

void CommandManager::undo() {
    if (m_undoStack.empty()) return;
    
    auto& command = m_undoStack.back();
    if (command->undo()) {
        // Move to redo stack
        m_redoStack.push_back(std::move(command));
        m_undoStack.pop_back();
        
        LOG_INFO("Undo: " + m_redoStack.back()->getDescription());
    }
}

void CommandManager::redo() {
    if (m_redoStack.empty()) return;
    
    auto& command = m_redoStack.back();
    if (command->execute()) {
        // Move to undo stack
        m_undoStack.push_back(std::move(command));
        m_redoStack.pop_back();
        
        LOG_INFO("Redo: " + m_undoStack.back()->getDescription());
    }
}

std::string CommandManager::getUndoDescription() const {
    if (m_undoStack.empty()) return "";
    return m_undoStack.back()->getDescription();
}

std::string CommandManager::getRedoDescription() const {
    if (m_redoStack.empty()) return "";
    return m_redoStack.back()->getDescription();
}

void CommandManager::clear() {
    m_undoStack.clear();
    m_redoStack.clear();
    LOG_INFO("Command history cleared");
}
