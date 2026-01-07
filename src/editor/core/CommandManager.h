/**
 * @file CommandManager.h
 * @brief Undo/Redo system using Command Pattern
 */
#pragma once

#include "IEditorCommand.h"
#include <vector>
#include <memory>
#include <string>

/**
 * @brief Manages undo/redo stack for editor commands
 */
class CommandManager {
public:
    static CommandManager& instance();
    
    /**
     * @brief Execute and push a command to undo stack
     */
    void executeCommand(std::unique_ptr<IEditorCommand> command);
    
    /**
     * @brief Undo last command
     */
    void undo();
    
    /**
     * @brief Redo last undone command
     */
    void redo();
    
    /**
     * @brief Can we undo?
     */
    bool canUndo() const { return !m_undoStack.empty(); }
    
    /**
     * @brief Can we redo?
     */
    bool canRedo() const { return !m_redoStack.empty(); }
    
    /**
     * @brief Get description of command that would be undone
     */
    std::string getUndoDescription() const;
    
    /**
     * @brief Get description of command that would be redone
     */
    std::string getRedoDescription() const;
    
    /**
     * @brief Clear all history
     */
    void clear();
    
    /**
     * @brief Set maximum history size (default: 100)
     */
    void setMaxHistory(size_t maxSize) { m_maxHistory = maxSize; }
    
private:
    CommandManager() = default;
    
    std::vector<std::unique_ptr<IEditorCommand>> m_undoStack;
    std::vector<std::unique_ptr<IEditorCommand>> m_redoStack;
    size_t m_maxHistory = 100;
};
