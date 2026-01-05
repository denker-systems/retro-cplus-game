/**
 * @file HotspotCommands.cpp
 * @brief Implementation of hotspot editing commands
 */
#include "HotspotCommands.h"
#include "editor/EditorCore.h"
#include "editor/EditorContext.h"

static RoomData* findRoom(const std::string& roomId) {
    auto& ctx = EditorCore::instance().getContext();
    for (auto& room : ctx.rooms) {
        if (room.id == roomId) return &room;
    }
    return nullptr;
}

// MoveHotspotCommand
MoveHotspotCommand::MoveHotspotCommand(const std::string& roomId, int hotspotIndex,
    int oldX, int oldY, int newX, int newY)
    : m_roomId(roomId), m_hotspotIndex(hotspotIndex),
      m_oldX(oldX), m_oldY(oldY), m_newX(newX), m_newY(newY) {}

bool MoveHotspotCommand::execute() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_hotspotIndex >= static_cast<int>(room->hotspots.size())) return false;
    room->hotspots[m_hotspotIndex].x = m_newX;
    room->hotspots[m_hotspotIndex].y = m_newY;
    return true;
}

bool MoveHotspotCommand::undo() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_hotspotIndex >= static_cast<int>(room->hotspots.size())) return false;
    room->hotspots[m_hotspotIndex].x = m_oldX;
    room->hotspots[m_hotspotIndex].y = m_oldY;
    return true;
}

std::string MoveHotspotCommand::getDescription() const { return "Move hotspot"; }

bool MoveHotspotCommand::canMergeWith(const IEditorCommand* other) const {
    auto* cmd = dynamic_cast<const MoveHotspotCommand*>(other);
    return cmd && cmd->m_roomId == m_roomId && cmd->m_hotspotIndex == m_hotspotIndex;
}

void MoveHotspotCommand::mergeWith(const IEditorCommand* other) {
    if (auto* cmd = dynamic_cast<const MoveHotspotCommand*>(other)) {
        m_newX = cmd->m_newX; m_newY = cmd->m_newY;
    }
}

// AddHotspotCommand
AddHotspotCommand::AddHotspotCommand(const std::string& roomId, const HotspotData& hotspot)
    : m_roomId(roomId), m_hotspot(hotspot) {}

bool AddHotspotCommand::execute() {
    RoomData* room = findRoom(m_roomId);
    if (!room) return false;
    room->hotspots.push_back(m_hotspot);
    m_insertedIndex = static_cast<int>(room->hotspots.size()) - 1;
    return true;
}

bool AddHotspotCommand::undo() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_insertedIndex < 0) return false;
    room->hotspots.erase(room->hotspots.begin() + m_insertedIndex);
    return true;
}

std::string AddHotspotCommand::getDescription() const { return "Add hotspot"; }

// DeleteHotspotCommand
DeleteHotspotCommand::DeleteHotspotCommand(const std::string& roomId, int hotspotIndex)
    : m_roomId(roomId), m_hotspotIndex(hotspotIndex) {}

bool DeleteHotspotCommand::execute() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_hotspotIndex >= static_cast<int>(room->hotspots.size())) return false;
    m_deletedHotspot = room->hotspots[m_hotspotIndex];
    room->hotspots.erase(room->hotspots.begin() + m_hotspotIndex);
    return true;
}

bool DeleteHotspotCommand::undo() {
    RoomData* room = findRoom(m_roomId);
    if (!room) return false;
    room->hotspots.insert(room->hotspots.begin() + m_hotspotIndex, m_deletedHotspot);
    return true;
}

std::string DeleteHotspotCommand::getDescription() const { return "Delete hotspot"; }

// ResizeHotspotCommand
ResizeHotspotCommand::ResizeHotspotCommand(const std::string& roomId, int hotspotIndex,
    int oldX, int oldY, int oldW, int oldH, int newX, int newY, int newW, int newH)
    : m_roomId(roomId), m_hotspotIndex(hotspotIndex),
      m_oldX(oldX), m_oldY(oldY), m_oldW(oldW), m_oldH(oldH),
      m_newX(newX), m_newY(newY), m_newW(newW), m_newH(newH) {}

bool ResizeHotspotCommand::execute() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_hotspotIndex >= static_cast<int>(room->hotspots.size())) return false;
    auto& hs = room->hotspots[m_hotspotIndex];
    hs.x = m_newX; hs.y = m_newY; hs.w = m_newW; hs.h = m_newH;
    return true;
}

bool ResizeHotspotCommand::undo() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_hotspotIndex >= static_cast<int>(room->hotspots.size())) return false;
    auto& hs = room->hotspots[m_hotspotIndex];
    hs.x = m_oldX; hs.y = m_oldY; hs.w = m_oldW; hs.h = m_oldH;
    return true;
}

std::string ResizeHotspotCommand::getDescription() const { return "Resize hotspot"; }

// ModifyHotspotCommand
ModifyHotspotCommand::ModifyHotspotCommand(const std::string& roomId, int hotspotIndex,
    const HotspotData& oldData, const HotspotData& newData)
    : m_roomId(roomId), m_hotspotIndex(hotspotIndex), m_oldData(oldData), m_newData(newData) {}

bool ModifyHotspotCommand::execute() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_hotspotIndex >= static_cast<int>(room->hotspots.size())) return false;
    room->hotspots[m_hotspotIndex] = m_newData;
    return true;
}

bool ModifyHotspotCommand::undo() {
    RoomData* room = findRoom(m_roomId);
    if (!room || m_hotspotIndex >= static_cast<int>(room->hotspots.size())) return false;
    room->hotspots[m_hotspotIndex] = m_oldData;
    return true;
}

std::string ModifyHotspotCommand::getDescription() const { return "Modify hotspot"; }
