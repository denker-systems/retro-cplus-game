/**
 * @file RoomManager.cpp
 * @brief Implementation av RoomManager
 */
#include "RoomManager.h"
#include "utils/Logger.h"
#include <iostream>

RoomManager& RoomManager::instance() {
    static RoomManager instance;
    return instance;
}

void RoomManager::addRoom(std::unique_ptr<Room> room) {
    std::string id = room->getId();
    LOG_INFO("Adding room: " + id);
    m_rooms[id] = std::move(room);
}

void RoomManager::clearRooms() {
    LOG_INFO("Clearing all rooms for hot reload");
    m_currentRoom = nullptr;
    m_rooms.clear();
}

Room* RoomManager::getRoom(const std::string& roomId) {
    auto it = m_rooms.find(roomId);
    if (it == m_rooms.end()) {
        LOG_WARNING("Room not found: " + roomId);
        return nullptr;
    }
    return it->second.get();
}

bool RoomManager::changeRoom(const std::string& roomId) {
    LOG_INFO("changeRoom called: " + roomId);
    Room* newRoom = getRoom(roomId);
    if (!newRoom) {
        LOG_ERROR("Failed to change room - not found: " + roomId);
        return false;
    }
    
    LOG_INFO("Changing to room: " + roomId);
    m_currentRoom = newRoom;
    
    if (m_onRoomChange) {
        LOG_DEBUG("Calling onRoomChange callback");
        m_onRoomChange(roomId);
    }
    
    return true;
}
