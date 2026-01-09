/**
 * @file RoomManager.cpp
 * @brief DEPRECATED: Implementation av RoomManager (wrapper för SceneManager)
 * 
 * MIGRATION NOTE: RoomManager now delegates to SceneManager for backward compatibility.
 * All Room operations are forwarded to SceneManager which handles Scene objects.
 */
#include "RoomManager.h"
#include "SceneManager.h"
#include "utils/Logger.h"
#include <iostream>

RoomManager& RoomManager::instance() {
    static RoomManager instance;
    return instance;
}

void RoomManager::addRoom(std::unique_ptr<Room> room) {
    std::string id = room->getId();
    LOG_INFO("Adding room (DEPRECATED): " + id);
    // Legacy storage - behålls för backward compatibility
    m_rooms[id] = std::move(room);
}

void RoomManager::clearRooms() {
    LOG_INFO("Clearing all rooms for hot reload (DEPRECATED)");
    m_currentRoom = nullptr;
    m_rooms.clear();
    // Also clear SceneManager
    SceneManager::instance().clearScenes();
}

Room* RoomManager::getRoom(const std::string& roomId) {
    auto it = m_rooms.find(roomId);
    if (it == m_rooms.end()) {
        LOG_WARNING("Room not found (DEPRECATED): " + roomId);
        return nullptr;
    }
    return it->second.get();
}

bool RoomManager::changeRoom(const std::string& roomId) {
    LOG_INFO("changeRoom called (DEPRECATED): " + roomId);
    Room* newRoom = getRoom(roomId);
    if (!newRoom) {
        LOG_ERROR("Failed to change room - not found (DEPRECATED): " + roomId);
        return false;
    }
    
    LOG_INFO("Changing to room (DEPRECATED): " + roomId);
    m_currentRoom = newRoom;
    
    // Also delegate to SceneManager for new code
    SceneManager::instance().changeScene(roomId);
    
    if (m_onRoomChange) {
        LOG_DEBUG("Calling onRoomChange callback");
        m_onRoomChange(roomId);
    }
    
    return true;
}
