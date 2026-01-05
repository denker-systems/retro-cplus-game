/**
 * @file RoomManager.cpp
 * @brief Implementation av RoomManager
 */
#include "RoomManager.h"
#include <iostream>

RoomManager& RoomManager::instance() {
    static RoomManager instance;
    return instance;
}

void RoomManager::addRoom(std::unique_ptr<Room> room) {
    std::string id = room->getId();
    m_rooms[id] = std::move(room);
    std::cout << "Added room: " << id << std::endl;
}

Room* RoomManager::getRoom(const std::string& roomId) {
    auto it = m_rooms.find(roomId);
    return (it != m_rooms.end()) ? it->second.get() : nullptr;
}

bool RoomManager::changeRoom(const std::string& roomId) {
    Room* newRoom = getRoom(roomId);
    if (!newRoom) {
        std::cerr << "Room not found: " << roomId << std::endl;
        return false;
    }
    
    std::cout << "Changing to room: " << roomId << std::endl;
    m_currentRoom = newRoom;
    
    if (m_onRoomChange) {
        m_onRoomChange(roomId);
    }
    
    return true;
}
