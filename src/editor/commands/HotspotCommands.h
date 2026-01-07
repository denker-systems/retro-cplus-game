/**
 * @file HotspotCommands.h
 * @brief Commands for hotspot editing (Command Pattern)
 */
#pragma once

#include "editor/core/IEditorCommand.h"
#include "engine/data/GameData.h"
#include <string>

/**
 * @brief Move a hotspot to a new position
 */
class MoveHotspotCommand : public IEditorCommand {
public:
    MoveHotspotCommand(const std::string& roomId, int hotspotIndex,
                       int oldX, int oldY, int newX, int newY);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;
    
    bool canMergeWith(const IEditorCommand* other) const override;
    void mergeWith(const IEditorCommand* other) override;

private:
    std::string m_roomId;
    int m_hotspotIndex;
    int m_oldX, m_oldY;
    int m_newX, m_newY;
};

/**
 * @brief Resize a hotspot
 */
class ResizeHotspotCommand : public IEditorCommand {
public:
    ResizeHotspotCommand(const std::string& roomId, int hotspotIndex,
                         int oldX, int oldY, int oldW, int oldH,
                         int newX, int newY, int newW, int newH);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    std::string m_roomId;
    int m_hotspotIndex;
    int m_oldX, m_oldY, m_oldW, m_oldH;
    int m_newX, m_newY, m_newW, m_newH;
};

/**
 * @brief Add a new hotspot
 */
class AddHotspotCommand : public IEditorCommand {
public:
    AddHotspotCommand(const std::string& roomId, const HotspotData& hotspot);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    std::string m_roomId;
    HotspotData m_hotspot;
    int m_insertedIndex = -1;
};

/**
 * @brief Delete a hotspot
 */
class DeleteHotspotCommand : public IEditorCommand {
public:
    DeleteHotspotCommand(const std::string& roomId, int hotspotIndex);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    std::string m_roomId;
    int m_hotspotIndex;
    HotspotData m_deletedHotspot;  // Stored for undo
};

/**
 * @brief Modify hotspot properties
 */
class ModifyHotspotCommand : public IEditorCommand {
public:
    ModifyHotspotCommand(const std::string& roomId, int hotspotIndex,
                         const HotspotData& oldData, const HotspotData& newData);
    
    bool execute() override;
    bool undo() override;
    std::string getDescription() const override;

private:
    std::string m_roomId;
    int m_hotspotIndex;
    HotspotData m_oldData;
    HotspotData m_newData;
};

