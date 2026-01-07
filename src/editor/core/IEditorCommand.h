/**
 * @file IEditorCommand.h
 * @brief Interface for undoable editor commands (Command Pattern)
 * 
 * Every user action that modifies data should be encapsulated in a command.
 * This enables:
 * - Undo/Redo functionality
 * - Action logging
 * - Macro recording
 */
#pragma once

#include <string>

/**
 * @brief Base interface for all editor commands
 * 
 * Commands should:
 * 1. Store the state needed to undo the action
 * 2. Be atomic (single logical operation)
 * 3. Be independent of UI state
 */
class IEditorCommand {
public:
    virtual ~IEditorCommand() = default;
    
    /**
     * @brief Execute the command
     * @return true if successful
     */
    virtual bool execute() = 0;
    
    /**
     * @brief Undo the command (reverse execute)
     * @return true if successful
     */
    virtual bool undo() = 0;
    
    /**
     * @brief Get human-readable description
     */
    virtual std::string getDescription() const = 0;
    
    /**
     * @brief Can this command be merged with another?
     * 
     * Used for combining rapid small changes (e.g., typing)
     */
    virtual bool canMergeWith(const IEditorCommand* other) const { 
        (void)other;
        return false; 
    }
    
    /**
     * @brief Merge another command into this one
     */
    virtual void mergeWith(const IEditorCommand* other) { 
        (void)other;
    }
};

